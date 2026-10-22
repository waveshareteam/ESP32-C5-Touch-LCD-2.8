#include "photo_init.h"

#include "esp_jpeg_dec.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include <string.h>

static const char *TAG = "photo";

// 全局样式对象声明
static lv_style_t style_list;
static lv_style_t style_list_btn;
static lv_style_t style_list_text;
static lv_style_t style_list_btn_pressed;
static lv_style_t style_list_btn_hover;
static lv_style_t style_back_btn;        // 返回按钮样式
static lv_style_t style_back_btn_pressed;// 返回按钮按下样式

static lv_obj_t *photo_screen = NULL;
static lv_obj_t *list1 = NULL;
static lv_obj_t *photo_play_bg = NULL;
static generic_file_list_t *jpg_file_list = NULL;
static lv_display_t      *s_disp;
static int32_t current_photo_idx = 0;


static lv_image_dsc_t img_dsc;
static jpeg_dec_handle_t dec_handle = NULL;
static uint16_t *jpeg_out_framebuf = NULL;
static int jpeg_out_framebuf_size = 0;
static lv_obj_t * photo_img_obj = NULL;

#define PHOTO_SCREEN_W      240
#define PHOTO_SCREEN_H      320
#define PHOTO_HEADER_H      72
#define PHOTO_MARGIN        8
#define PHOTO_LIST_W        (PHOTO_SCREEN_W - PHOTO_MARGIN * 2)
#define PHOTO_LIST_H        (PHOTO_SCREEN_H - PHOTO_HEADER_H - PHOTO_MARGIN * 2)
#define VIDEO_W             240
#define VIDEO_H             320

static const char *photo_get_file_name(const char *path)
{
    const char *name = strrchr(path, '/');
    return (name != NULL) ? (name + 1) : path;
}

static bool ensure_jpeg_out_framebuf(int outbuf_len)
{
    if ((jpeg_out_framebuf != NULL) && (outbuf_len <= jpeg_out_framebuf_size)) {
        return true;
    }

    uint16_t *new_buf = (uint16_t *)jpeg_calloc_align((outbuf_len + 15) & ~15, 16);
    if (new_buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate JPEG output buffer: %d bytes", outbuf_len);
        return false;
    }

    if (jpeg_out_framebuf) {
        jpeg_free_align(jpeg_out_framebuf);
    }
    jpeg_out_framebuf = new_buf;
    jpeg_out_framebuf_size = outbuf_len;
    return true;
}

// 从 SD 卡读取文件到内存缓冲区
uint8_t* read_file_to_buffer(const char* path, size_t* size) 
{
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    fseek(f, 0, SEEK_SET);

    // ESP32-S3 要求输入缓冲区 16 字节对齐
    size_t aligned_size = (*size + 15) & ~15;
    uint8_t* buf = (uint8_t*)jpeg_calloc_align(aligned_size, 16); // 明确对齐到 16 字节
    
    if (buf) {
        fread(buf, 1, *size, f);
    }
    
    fclose(f);
    return buf;
}

void manual_decode_display(const char* file_path) 
{
    size_t jpeg_size = 0;
    uint8_t* jpeg_buf = read_file_to_buffer(file_path, &jpeg_size); 
    if (!jpeg_buf) {
        ESP_LOGE(TAG, "Failed to read file to buffer");
        return;
    }

    jpeg_dec_handle_t handle = NULL;
    jpeg_dec_config_t config = DEFAULT_JPEG_DEC_CONFIG();
    // 根据播放器参考，RGB565 格式在 S3 上建议配置
    config.output_type = JPEG_PIXEL_FORMAT_RGB565_LE; 
    
    if (jpeg_dec_open(&config, &handle) != JPEG_ERR_OK) {
        ESP_LOGE(TAG, "jpeg_dec_open failed");
        jpeg_free_align(jpeg_buf);
        return;
    }

    // 初始化 IO 结构体
    jpeg_dec_io_t io = {
        .inbuf = jpeg_buf,
        .inbuf_len = (int)jpeg_size,
    };

    // 1. 解析头信息
    jpeg_dec_header_info_t header_info;
    if (jpeg_dec_parse_header(handle, &io, &header_info) != JPEG_ERR_OK) {
        ESP_LOGE(TAG, "jpeg_dec_parse_header failed");
        goto _exit;
    }

    // 2. 根据解析出的尺寸获取所需的输出缓冲区大小
    int outbuf_len = 0;
    jpeg_dec_get_outbuf_len(handle, &outbuf_len);
    
    // 确保对齐
    size_t aligned_out_size = (outbuf_len + 15) & ~15;
    uint8_t* out_buf = (uint8_t *)jpeg_calloc_align(aligned_out_size, 16);
    if (out_buf == NULL) {
        ESP_LOGE(TAG, "Failed to allocate out_buf");
        goto _exit;
    }

    // 3. 关联输出缓冲区到 IO 结构体
    io.outbuf = out_buf;
    io.out_size = outbuf_len; // v0.6.1 成员名为 out_size

    // 4. 执行解码
    if (jpeg_dec_process(handle, &io) == JPEG_ERR_OK) {
        // 5. LVGL 显示逻辑
        static lv_image_dsc_t img_dsc;
        img_dsc.header.cf = LV_COLOR_FORMAT_RGB565;
        img_dsc.header.w = header_info.width;
        img_dsc.header.h = header_info.height;
        img_dsc.data_size = outbuf_len;
        img_dsc.data = out_buf;

        lv_obj_t * img_obj = lv_image_create(lv_screen_active());
        lv_image_set_src(img_obj, &img_dsc);
        lv_obj_set_size(img_obj, VIDEO_W, VIDEO_H);
        lv_obj_center(img_obj);

        lv_obj_remove_flag(img_obj, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_scrollbar_mode(img_obj, LV_SCROLLBAR_MODE_OFF);
        
        // 注意：out_buf 必须在图像对象存在期间保持有效，不能立即释放
    } else {
        ESP_LOGE(TAG, "jpeg_dec_process failed");
        jpeg_free_align(out_buf);
    }

_exit:
    if (handle) {
        jpeg_dec_close(handle);
    }
    if (jpeg_buf) {
        jpeg_free_align(jpeg_buf);
    }
}



void update_photo_display(int32_t path_index)
{
    size_t jpeg_size = 0;
    printf("%s\r\n",jpg_file_list->list[path_index]);
    uint8_t* jpeg_buf = read_file_to_buffer(jpg_file_list->list[path_index], &jpeg_size); 
    if (!jpeg_buf) {
        ESP_LOGE(TAG, "Failed to read file to buffer");
        return;
    }

    // 初始化 IO 结构体
    jpeg_dec_io_t io = {
        .inbuf = jpeg_buf,
        .inbuf_len = (int)jpeg_size,
        .outbuf = (uint8_t*)jpeg_out_framebuf,
        .out_size = jpeg_out_framebuf_size
    };

    // 1. 解析头信息
    jpeg_dec_header_info_t header_info;
    if (jpeg_dec_parse_header(dec_handle, &io, &header_info) != JPEG_ERR_OK) {
        ESP_LOGE(TAG, "jpeg_dec_parse_header failed");
        jpeg_free_align(jpeg_buf);
        return;
    }

    int outbuf_len = 0;
    jpeg_dec_get_outbuf_len(dec_handle, &outbuf_len);
    if (!ensure_jpeg_out_framebuf(outbuf_len)) {
        jpeg_free_align(jpeg_buf);
        return;
    }
    io.outbuf = (uint8_t*)jpeg_out_framebuf;
    io.out_size = jpeg_out_framebuf_size;
    

    // 2. 执行解码
    if (jpeg_dec_process(dec_handle, &io) == JPEG_ERR_OK) {
        // 5. LVGL 显示逻辑

        img_dsc.header.cf = LV_COLOR_FORMAT_RGB565;
        img_dsc.header.w = header_info.width;
        img_dsc.header.h = header_info.height;
        img_dsc.data_size = outbuf_len;
        img_dsc.data = (uint8_t*)jpeg_out_framebuf;

        lv_image_set_src(photo_img_obj, &img_dsc);
        lv_obj_center(photo_img_obj);
    } 
    else 
    {
        ESP_LOGE(TAG, "jpeg_dec_process failed");

    }

    jpeg_free_align(jpeg_buf);
}

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {

        current_photo_idx = (int32_t)lv_obj_get_index(obj);
        LV_UNUSED(obj);
        ESP_LOGI(TAG,"index = %ld,PHOTO PATH: %s",current_photo_idx, lv_list_get_button_text(list1, obj));

        //manual_decode_display(lv_list_get_button_text(list1, obj));
        update_photo_display(current_photo_idx);

        lv_obj_remove_flag(photo_play_bg, LV_OBJ_FLAG_HIDDEN);
        esp_lv_adapter_refresh_now(s_disp);
    }
}

static void photo_play_event_handler(lv_event_t * e) 
{
    if (lv_event_get_code(e) == LV_EVENT_GESTURE) 
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        
        if (dir == LV_DIR_LEFT ) 
        {
            current_photo_idx++;
            if(current_photo_idx > jpg_file_list->count-1)
            {
                current_photo_idx  = 0;
            }
            update_photo_display(current_photo_idx);
        } 
        else if (dir == LV_DIR_RIGHT) 
        {
            current_photo_idx--;
            if(current_photo_idx < 0)
            {
                current_photo_idx  = jpg_file_list->count-1;
            }
            update_photo_display(current_photo_idx);
        } 
        else if (dir == LV_DIR_TOP) 
        {
            lv_obj_add_flag(photo_play_bg, LV_OBJ_FLAG_HIDDEN);

            ESP_LOGI(TAG,"LV_DIR_TOP");
        }
    }
}

void init_photo_ui_screen(lv_obj_t *parent,generic_file_list_t *file_list)
{
    
    // ===================== 初始化样式对象 =====================
    lv_style_init(&style_list);
    lv_style_init(&style_list_btn);
    lv_style_init(&style_list_text);
    lv_style_init(&style_list_btn_pressed);
    lv_style_init(&style_list_btn_hover);
    lv_style_init(&style_back_btn);
    lv_style_init(&style_back_btn_pressed);

    // ===================== 列表容器样式 =====================
    lv_style_set_pad_all(&style_list, 6);
    lv_style_set_pad_row(&style_list, 5);
    lv_style_set_border_width(&style_list, 0);
    lv_style_set_radius(&style_list, 10);
    lv_style_set_bg_color(&style_list, lv_color_hex(0xF7F8FA));
    lv_style_set_bg_opa(&style_list, LV_OPA_COVER);
    lv_style_set_shadow_width(&style_list, 0);
    lv_style_set_shadow_color(&style_list, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(&style_list, LV_OPA_10);
    lv_style_set_shadow_offset_y(&style_list, 2);

    // ===================== 列表按钮默认样式 =====================
    lv_style_set_height(&style_list_btn, 40);
    lv_style_set_pad_left(&style_list_btn, 8);
    lv_style_set_pad_right(&style_list_btn, 8);
    lv_style_set_pad_column(&style_list_btn, 8);
    lv_style_set_border_width(&style_list_btn, 0);
    lv_style_set_radius(&style_list_btn, 8);
    lv_style_set_bg_color(&style_list_btn, lv_color_hex(0xFFFFFF));
    lv_style_set_text_font(&style_list_btn, &lv_font_montserrat_14);
    lv_style_set_text_color(&style_list_btn, lv_color_hex(0x1F2933));
    // 底部分隔线样式
    lv_style_set_border_width(&style_list_btn, 1);
    lv_style_set_border_side(&style_list_btn, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_border_color(&style_list_btn, lv_color_hex(0xD1D5DB));
    

    // ===================== 分类文本样式（浅灰标题） =====================
    lv_style_set_text_font(&style_list_text, &lv_font_montserrat_14);
    lv_style_set_text_color(&style_list_text, lv_color_hex(0x6B7280));
    lv_style_set_bg_color(&style_list_text, lv_color_hex(0xF7F8FA));
    lv_style_set_bg_opa(&style_list_text, LV_OPA_COVER);
    lv_style_set_pad_all(&style_list_text, 10);
    lv_style_set_pad_bottom(&style_list_text, 5);

    // ===================== 按钮按下样式（淡蓝反馈） =====================
    lv_style_set_bg_color(&style_list_btn_pressed, lv_color_hex(0xEAF4FF));
    lv_style_set_text_color(&style_list_btn_pressed, lv_color_hex(0x0D6EFD));
    lv_style_set_border_color(&style_list_btn_pressed, lv_color_hex(0xCFE6FF));

    // ===================== 按钮悬浮样式（轻微高亮） =====================
    lv_style_set_bg_color(&style_list_btn_hover, lv_color_hex(0xF0F6FF));
    lv_style_set_text_color(&style_list_btn_hover, lv_color_hex(0x1F2933));

    // ===================== 返回按钮样式 =====================
    lv_style_set_size(&style_back_btn, 40, 40);                // 圆形按钮尺寸
    lv_style_set_radius(&style_back_btn, 20);                  // 圆形圆角
    lv_style_set_bg_color(&style_back_btn, lv_color_hex(0xFFFFFF));  // 白色背景
    lv_style_set_border_width(&style_back_btn, 0);
    lv_style_set_shadow_width(&style_back_btn, 2);             // 轻微阴影
    lv_style_set_shadow_color(&style_back_btn, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(&style_back_btn, LV_OPA_10);
    lv_style_set_shadow_offset_y(&style_back_btn, 2);
    lv_style_set_text_font(&style_back_btn, &lv_font_montserrat_18);  // 图标字体
    lv_style_set_text_color(&style_back_btn, lv_color_hex(0x333333));  // 深灰图标
    lv_style_set_align(&style_back_btn, LV_ALIGN_CENTER);       // 图标居中

    // ===================== 返回按钮按下样式 =====================
    lv_style_set_bg_color(&style_back_btn_pressed, lv_color_hex(0xEFF6FF));  // 淡蓝背景
    lv_style_set_text_color(&style_back_btn_pressed, lv_color_hex(0x0D6EFD));  // 蓝色图标

    // ===================== 屏幕背景样式 =====================
    lv_obj_set_style_bg_color(parent, lv_color_hex(0xF7F8FA), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, LV_PART_MAIN);

    // ===================== 创建主屏幕容器（核心：禁止滚动） =====================

    photo_screen = lv_obj_create(parent);
    lv_obj_set_size(photo_screen, PHOTO_SCREEN_W, PHOTO_SCREEN_H);
    lv_obj_set_style_bg_color(photo_screen, lv_color_hex(0xF7F8FA), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(photo_screen, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(photo_screen, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(photo_screen, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(photo_screen, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(photo_screen, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(photo_screen, LV_OPA_10, LV_PART_MAIN);
    lv_obj_set_style_shadow_offset_y(photo_screen, 4, LV_PART_MAIN);
    lv_obj_align(photo_screen, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_scrollbar_mode(photo_screen, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(photo_screen, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title_label = lv_label_create(photo_screen);
    lv_label_set_text(title_label, "Photo");
    lv_obj_set_style_text_color(title_label, lv_color_hex(0x1F2933), 0);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_18, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 15);

    // ===================== 创建列表容器 =====================
    list1 = lv_list_create(photo_screen);
    lv_obj_add_style(list1, &style_list, LV_PART_MAIN);
    lv_obj_set_size(list1, PHOTO_LIST_W, PHOTO_LIST_H);
    lv_obj_align(list1, LV_ALIGN_BOTTOM_MID, 0, -PHOTO_MARGIN);
    lv_obj_set_style_pad_top(list1, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(list1, 0, LV_PART_MAIN);

    // ===================== 添加分类和文本 =====================
    ESP_LOGE(TAG,"file_list->count=%d",file_list->count);
    for (size_t i = 0; i < file_list->count; i++)
    {
        lv_obj_t *file_name = lv_list_add_button(list1, LV_SYMBOL_IMAGE, photo_get_file_name(file_list->list[i]));
        lv_obj_add_style(file_name, &style_list_btn, LV_PART_MAIN);
        lv_obj_add_style(file_name, &style_list_btn_pressed, LV_PART_MAIN | LV_STATE_PRESSED);
        lv_obj_add_style(file_name, &style_list_btn_hover, LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_add_event_cb(file_name, event_handler, LV_EVENT_CLICKED, NULL);
    }


    jpg_file_list = file_list;

    photo_play_bg = lv_obj_create(lv_screen_active());
    lv_obj_set_size(photo_play_bg, PHOTO_SCREEN_W, PHOTO_SCREEN_H);
    lv_obj_set_style_bg_color(photo_play_bg, lv_color_hex(0xF7F8FA), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(photo_play_bg, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(photo_play_bg, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(photo_play_bg, 0, LV_PART_MAIN);
    lv_obj_align(photo_play_bg, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(photo_play_bg, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_scrollbar_mode(photo_play_bg, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(photo_play_bg, LV_OBJ_FLAG_SCROLLABLE);

    

    img_dsc.header.cf = LV_COLOR_FORMAT_RGB565;
    img_dsc.data = NULL;

    photo_img_obj = lv_image_create(photo_play_bg);
    lv_image_set_src(photo_img_obj, &img_dsc);
    lv_obj_set_size(photo_img_obj, PHOTO_SCREEN_W, PHOTO_SCREEN_H);
    lv_image_set_inner_align(photo_img_obj, LV_IMAGE_ALIGN_CONTAIN);
    lv_obj_center(photo_img_obj);
    //lv_obj_add_flag(photo_img_obj, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(photo_img_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(lv_scr_act(), photo_play_event_handler, LV_EVENT_GESTURE, NULL);


}


void init_photo_player(lv_display_t *disp)
{
    s_disp = disp;

    jpeg_dec_config_t config = DEFAULT_JPEG_DEC_CONFIG();
    config.output_type = JPEG_PIXEL_FORMAT_RGB565_LE; 
    
    if (jpeg_dec_open(&config, &dec_handle) != JPEG_ERR_OK) {
        ESP_LOGE(TAG, "jpeg_dec_open failed");
        return;
    }

    jpeg_out_framebuf_size = VIDEO_W * VIDEO_H * sizeof(uint16_t);
    jpeg_out_framebuf = (uint16_t *)jpeg_calloc_align(jpeg_out_framebuf_size, 16);
}

/**
 * @brief 释放照片播放器资源
 */
void deinit_photo_player(void)
{
    // 1. 释放解码器硬件资源
    if (dec_handle) {
        jpeg_dec_close(dec_handle);
        dec_handle = NULL;
    }

    // 2. 释放对齐分配的输出帧缓存
    if (jpeg_out_framebuf) {
        jpeg_free_align(jpeg_out_framebuf);
        jpeg_out_framebuf = NULL;
        jpeg_out_framebuf_size = 0;
    }

    // 3. 释放样式资源 (LVGL 9 推荐显式注销)
    lv_style_reset(&style_list);
    lv_style_init(&style_list_btn);
    lv_style_init(&style_list_text);
    lv_style_init(&style_list_btn_pressed);
    lv_style_init(&style_list_btn_hover);
    lv_style_init(&style_back_btn);
    lv_style_init(&style_back_btn_pressed);

    // 4. 删除 UI 对象（如果父对象 parent 已被删除，此步可省略）
    if (photo_play_bg) {
        lv_obj_delete(photo_play_bg);
        photo_play_bg = NULL;
    }
    
    ESP_LOGI(TAG, "Photo player deinitialized");
}

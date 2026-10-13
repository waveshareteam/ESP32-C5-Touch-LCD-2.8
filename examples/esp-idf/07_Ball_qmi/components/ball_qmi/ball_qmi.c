#include "ball_qmi.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"

// Global variable definitions
float accel_bias_x = 0.0f;
float accel_bias_y = 0.0f;
bool calibration_done = false;
int display_width = 0;
int display_height = 0;
Shape shapes[MAX_SHAPES];
int shape_count = 0;

/**
 * @brief Get light green color (ball color)
 */
lv_color_t get_green_color(void) {
    return lv_color_hex(0x006400); // Light green
}

/**
 * @brief Create LVGL object for circular ball
 */
lv_obj_t *create_shape_obj(ShapeType type, int size, lv_color_t color) {
    lv_obj_t *obj = lv_obj_create(lv_screen_active());
    lv_obj_set_size(obj, size * 2, size * 2);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0); // Circular shape
    lv_obj_set_style_bg_color(obj, color, 0);          // Background color
    lv_obj_set_style_border_width(obj, 0, 0);          // No border
    lv_obj_set_style_shadow_width(obj, 3, 0);          // Shadow effect
    return obj;
}

/**
 * @brief Generate initial ball at screen center
 */
void generate_ball(void) {
    shape_count = 0;
    
    Shape new_shape;
    new_shape.type = SHAPE_CIRCLE;
    new_shape.radius = MIN_SHAPE_SIZE;
    new_shape.color = get_green_color();
    
    new_shape.obj = create_shape_obj(new_shape.type, new_shape.radius, new_shape.color);
    if (!new_shape.obj) return;
    
    // Initial position: screen center
    new_shape.x_pos = display_width / 2;
    new_shape.y_pos = display_height / 2;
    
    lv_obj_set_pos(new_shape.obj, new_shape.x_pos - new_shape.radius, 
                  new_shape.y_pos - new_shape.radius);
    shapes[shape_count] = new_shape;
    shape_count++;
}

/**
 * @brief QMI8658 accelerometer level calibration
 */
void ball_qmi_calibrate(qmi8658_dev_t *dev) {
    qmi8658_data_t data;
    const int CALIB_SAMPLES = 200;
    float sum_x = 0.0f, sum_y = 0.0f;
    float max_x = -10.0f, min_x = 10.0f;
    float max_y = -10.0f, min_y = 10.0f;
    
    ESP_LOGI(BALL_QMI_TAG, "Starting level calibration...");
    ESP_LOGI(BALL_QMI_TAG, "Please place device on a level surface");
    
    for (int i = 0; i < CALIB_SAMPLES; i++) {
        if (qmi8658_read_sensor_data(dev, &data) == ESP_OK) {
            sum_x += data.accelX;
            sum_y += data.accelY;
            
            if (data.accelX > max_x) max_x = data.accelX;
            if (data.accelX < min_x) min_x = data.accelX;
            if (data.accelY > max_y) max_y = data.accelY;
            if (data.accelY < min_y) min_y = data.accelY;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
    float range_x = max_x - min_x;
    float range_y = max_y - min_y;
    
    if (range_x > 0.1f || range_y > 0.1f) {
        ESP_LOGW(BALL_QMI_TAG, "Calibration unstable (X range: %.4f, Y range: %.4f). Retrying...", 
                 range_x, range_y);
        ball_qmi_calibrate(dev);
        return;
    }
    
    accel_bias_x = sum_x / CALIB_SAMPLES;
    accel_bias_y = sum_y / CALIB_SAMPLES;
    
    calibration_done = true;
    ESP_LOGI(BALL_QMI_TAG, "Calibration complete. Bias X: %.4f m/s², Bias Y: %.4f m/s²", 
             accel_bias_x, accel_bias_y);
    ESP_LOGI(BALL_QMI_TAG, "Device is now level. Ball should be stationary.");
}

/**
 * @brief Calibrate acceleration data (offset cancellation + dead zone filtering)
 */
void apply_calibration(qmi8658_data_t *data) {
    if (calibration_done) {
        data->accelX -= accel_bias_x;
        data->accelY -= accel_bias_y;
        
        // Dead zone filtering to avoid micro-jitter
        if (fabsf(data->accelX) < CALIBRATION_DEADZONE) data->accelX = 0.0f;
        if (fabsf(data->accelY) < CALIBRATION_DEADZONE) data->accelY = 0.0f;
    }
}

/**
 * @brief Constrain ball position to prevent out of screen rounded corner range
 */
void constrain_ball_pos(int *x, int *y) {
    float px_per_mm_x = (float)display_width / SCREEN_WIDTH_MM;
    float px_per_mm_y = (float)display_height / SCREEN_HEIGHT_MM;
    float px_per_mm = (px_per_mm_x < px_per_mm_y) ? px_per_mm_x : px_per_mm_y;
    
    int corner_radius_px = (int)(CORNER_RADIUS_MM * px_per_mm);
    if (corner_radius_px < MIN_SHAPE_SIZE) {
        corner_radius_px = MIN_SHAPE_SIZE + 5;
    }

    int safe_left = corner_radius_px;
    int safe_right = display_width - corner_radius_px;
    int safe_top = corner_radius_px;
    int safe_bottom = display_height - corner_radius_px;

    // Basic boundary constraint
    if (*x < MIN_SHAPE_SIZE) *x = MIN_SHAPE_SIZE;
    if (*x > display_width - MIN_SHAPE_SIZE) *x = display_width - MIN_SHAPE_SIZE;
    if (*y < MIN_SHAPE_SIZE) *y = MIN_SHAPE_SIZE;
    if (*y > display_height - MIN_SHAPE_SIZE) *y = display_height - MIN_SHAPE_SIZE;

    int corner_radius_effective = corner_radius_px - MIN_SHAPE_SIZE;
    
    // Rounded corner area constraint
    if (*x < safe_left && *y < safe_top) {
        float dx = safe_left - *x;
        float dy = safe_top - *y;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist > corner_radius_effective) {
            float ratio = corner_radius_effective / dist;
            *x = safe_left - (int)(dx * ratio);
            *y = safe_top - (int)(dy * ratio);
        }
    }
    else if (*x > safe_right && *y < safe_top) {
        float dx = *x - safe_right;
        float dy = safe_top - *y;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist > corner_radius_effective) {
            float ratio = corner_radius_effective / dist;
            *x = safe_right + (int)(dx * ratio);
            *y = safe_top - (int)(dy * ratio);
        }
    }
    else if (*x < safe_left && *y > safe_bottom) {
        float dx = safe_left - *x;
        float dy = *y - safe_bottom;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist > corner_radius_effective) {
            float ratio = corner_radius_effective / dist;
            *x = safe_left - (int)(dx * ratio);
            *y = safe_bottom + (int)(dy * ratio);
        }
    }
    else if (*x > safe_right && *y > safe_bottom) {
        float dx = *x - safe_right;
        float dy = *y - safe_bottom;
        float dist = sqrtf(dx*dx + dy*dy);
        if (dist > corner_radius_effective) {
            float ratio = corner_radius_effective / dist;
            *x = safe_right + (int)(dx * ratio);
            *y = safe_bottom + (int)(dy * ratio);
        }
    }
}

/**
 * @brief Ball update task (read acceleration, update position)
 */
void ball_qmi_task(void *arg) {
    qmi8658_dev_t *dev = (qmi8658_dev_t *)arg;
    qmi8658_data_t data;
    
    // Wait for screen size initialization
    while (display_width == 0 || display_height == 0) {
        display_width = lv_disp_get_hor_res(NULL);
        display_height = lv_disp_get_ver_res(NULL);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    
    // Generate initial ball
    generate_ball();

    while (1) {
        bool ready;
        esp_err_t ret = qmi8658_is_data_ready(dev, &ready);
        if (ret != ESP_OK) {
            vTaskDelay(pdMS_TO_TICKS(TASK_DELAY_MS));
            continue;
        }

        if (ready) {
            ret = qmi8658_read_sensor_data(dev, &data);
            if (ret == ESP_OK) {
                apply_calibration(&data);
                
                // Calculate ball movement offset (direction corrected)
                int move_x = (int)(data.accelY * ACCEL_SCALE_FACTOR);
                int move_y = -(int)(data.accelX * ACCEL_SCALE_FACTOR);
                
                // Lock LVGL display to avoid UI conflict
                bsp_display_lock(pdMS_TO_TICKS(100));
                
                // Update ball position
                for (int i = 0; i < shape_count; i++) {
                    int new_x = shapes[i].x_pos + move_x;
                    int new_y = shapes[i].y_pos + move_y;
                    
                    // Update position and constrain boundary
                    shapes[i].x_pos = new_x;
                    shapes[i].y_pos = new_y;
                    constrain_ball_pos(&shapes[i].x_pos, &shapes[i].y_pos);
                    
                    // Update LVGL object position
                    lv_obj_set_pos(shapes[i].obj, 
                                  shapes[i].x_pos - shapes[i].radius, 
                                  shapes[i].y_pos - shapes[i].radius);
                }
                
                // Unlock LVGL display
                bsp_display_unlock();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(TASK_DELAY_MS));
    }
}
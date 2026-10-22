/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if LV_USE_FREEMASTER
#include <time.h>
#endif

#include "lvgl.h"
#include "custom.h"

/*********************
 *      DEFINES
 *********************/
#define MATRIX_DIMEN            4
#define MAP_LENGTH (MATRIX_DIMEN * MATRIX_DIMEN + MATRIX_DIMEN)

#define BUTTON_COLOR_EMPTY      lv_color_hex(0xD8C9AC)
#define BUTTON_COLOR_2 		    lv_color_hex(0xEEE4DA)
#define BUTTON_COLOR_4 		    lv_color_hex(0xEBD8B6)
#define BUTTON_COLOR_8 		    lv_color_hex(0xF1AE72)
#define BUTTON_COLOR_16 		lv_color_hex(0xF6925F)
#define BUTTON_COLOR_32 		lv_color_hex(0xF67D61)
#define BUTTON_COLOR_64 		lv_color_hex(0xF75F3B)
#define BUTTON_COLOR_128 	    lv_color_hex(0xF2CF54)
#define BUTTON_COLOR_256 	    lv_color_hex(0xEDCC61)
#define BUTTON_COLOR_512 	    lv_color_hex(0xEDC850)
#define BUTTON_COLOR_1024 	    lv_color_hex(0xEDC53F)
#define BUTTON_COLOR_2048 	    lv_color_hex(0xEDC22E)
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES   
 **********************/
static void init_matrix(void);
static void init_map(void);
static void update_btnm_map(char * btnm_map[], uint32_t num_matrix[MATRIX_DIMEN][MATRIX_DIMEN]);
static void add_rand_num(uint32_t num_matrix[MATRIX_DIMEN][MATRIX_DIMEN]);
static bool is_game_over(void);
static bool is_win(void);
static bool move_left(void);
static bool move_right(void);
static bool move_up(void);
static bool move_down(void);
static lv_color_t get_btn_color(uint32_t num);
static void btnm_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t num_matrix[MATRIX_DIMEN][MATRIX_DIMEN];
static char * btnm_map[MAP_LENGTH];
static uint32_t score = 0;
static uint32_t best_score = 0;
static uint32_t moves = 0;

/**
 * Create a demo application
 */

void custom_init(lv_ui *ui)
{
    /* Add your codes here */
    /*Init matrix & map then update button matrix diaplay*/
    init_matrix();
    init_map();
    update_btnm_map(btnm_map, num_matrix);
    lv_buttonmatrix_set_map(ui->Game2048_btnm_2048, (const char * const *)btnm_map);

    /*Add special event callback and add flag to get event signal*/
    lv_obj_add_event_cb(ui->Game2048_btnm_2048, btnm_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_flag(ui->Game2048_btnm_2048, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
}

void movement_check(lv_ui *ui, gg_2048_move_direction direction)
{
    bool is_moved = false;
    switch (direction)
    {
        case GG_2048_MOVE_LEFT:
            is_moved = move_left();
            break;
        case GG_2048_MOVE_RIGHT:
            is_moved = move_right();
            break;
        case GG_2048_MOVE_UP:
            is_moved = move_up();
            break;
        case GG_2048_MOVE_DOWN:
            is_moved = move_down();
            break;
        default:
            break;
    }
    if (is_moved) {
        moves += 1;
        best_score = (score > best_score) ? score : best_score;
        lv_label_set_text_fmt(ui->Game2048_label_score,"%" LV_PRIu32, score);
        lv_label_set_text_fmt(ui->Game2048_label_best, "%" LV_PRIu32, best_score);

        add_rand_num(num_matrix);
        update_btnm_map(btnm_map, num_matrix);
        lv_buttonmatrix_set_map(ui->Game2048_btnm_2048, (const char * const *)btnm_map);

        if (is_win()) {
            lv_obj_remove_flag(ui->Game2048_cont_msgbox, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(ui->Game2048_label_warning, "You Win!");
            lv_label_set_text_fmt(ui->Game2048_label_results, "%" LV_PRIu32"\n%" LV_PRIu32"\n%" LV_PRIu32, moves, score, best_score);

        } else if (is_game_over()) {
            lv_obj_remove_flag(ui->Game2048_cont_msgbox, LV_OBJ_FLAG_HIDDEN);
            lv_label_set_text(ui->Game2048_label_warning, "Game Over");
            lv_label_set_text_fmt(ui->Game2048_label_results, "%" LV_PRIu32"\n%" LV_PRIu32"\n%" LV_PRIu32, moves, score, best_score);
        }
    }
}

void new_game(lv_ui *ui)
{
    init_matrix();

    score = 0;
    moves = 0;

    lv_label_set_text_fmt(ui->Game2048_label_score, "%" LV_PRIu32, score);
    update_btnm_map(btnm_map, num_matrix);
    lv_buttonmatrix_set_map(ui->Game2048_btnm_2048, (const char * const *)btnm_map);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * init matrix of all button matrix number
 */
static void init_matrix(void)
{
    for (uint32_t i = 0; i < MATRIX_DIMEN; i++) {
        for (uint32_t j = 0; j < MATRIX_DIMEN; j++) {
            num_matrix[i][j] = 0;
        }
    }

    add_rand_num(num_matrix);
    add_rand_num(num_matrix);
}

/**
 * init button matrix map
 */
static void init_map(void)
{
    uint32_t index;
    for (index = 0; index < MAP_LENGTH; index++) {

        if (((index + 1) % 5) == 0) {
            btnm_map[index] = lv_malloc(2);
            if ((index + 1) == MAP_LENGTH)
                lv_strcpy(btnm_map[index], "");
            else
            lv_strcpy(btnm_map[index], "\n");
        } else {
            btnm_map[index] = lv_malloc(5);
            lv_strcpy(btnm_map[index], " ");
        }
    }
}

/**
 * Update the button matrix map value
 * @param btnm_map      map set to button matrix 
 * @param num_matrix    matrix that save all buttons' number
 */
static void update_btnm_map(char * btnm_map[], uint32_t num_matrix[MATRIX_DIMEN][MATRIX_DIMEN])
{
    uint32_t x, y, index;
    index = 0;
    for (x = 0; x < MATRIX_DIMEN; x++) {
		for (y = 0; y < MATRIX_DIMEN; y++) {
            if (((index + 1) % 5) == 0)
                index++;

            if (num_matrix[x][y] != 0) {
                sprintf(btnm_map[index], "%" LV_PRIu32, num_matrix[x][y]);
            } else {
            	lv_strcpy(btnm_map[index], " ");
            }
            index++;
        }
    }
}

/**
 * Add radnom number to the matrix empty place
 * @param num_matrix    matrix that save all buttons' number
 */
static void add_rand_num(uint32_t num_matrix[MATRIX_DIMEN][MATRIX_DIMEN])
{
	static bool initialized = false;
	uint32_t x, y;
	uint32_t r, len = 0;
	uint32_t n, empty[MATRIX_DIMEN * MATRIX_DIMEN][2];

	if (!initialized) {
#if LV_USE_FREEMASTER
        lv_rand_set_seed(time(NULL));
#else
        lv_rand_set_seed(rand());
#endif
		initialized = true;
	}

	for (x = 0; x < MATRIX_DIMEN; x++) {
		for (y = 0; y < MATRIX_DIMEN; y++) {
			if (num_matrix[x][y] == 0) {
				empty[len][0] = x;
				empty[len][1] = y;
				len++; 
			}
		}
	}

	if (len > 0) {
		r = lv_rand(0, len - 1);
		x = empty[r][0];
		y = empty[r][1];
		n = ((lv_rand(0, 9) / 9) == 0) ? 2 : 4;
		num_matrix[x][y] = n;
	}
 }

/**
 * Check if the game is over
 * @return              true if game over
 */
static bool is_game_over(void)
{
    /*Check if there's empty button*/
    for (int i = 0; i < MATRIX_DIMEN; i++) {
        for (int j = 0; j < MATRIX_DIMEN; j++) {
            if (num_matrix[i][j] == 0) {
                return false;
            }
        }
    }
    /*Check if there're same button can be merged*/
    for (int i = 0; i < MATRIX_DIMEN; i++) {
        for (int j = 0; j < MATRIX_DIMEN; j++) {
            if ((j < MATRIX_DIMEN-1 && num_matrix[i][j] == num_matrix[i][j+1]) ||
                (i < MATRIX_DIMEN-1 && num_matrix[i][j] == num_matrix[i+1][j])) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Check if the game is win
 * @return              true if game win
 */
static bool is_win(void)
{
    for (int i = 0; i < MATRIX_DIMEN; i++) {
        for (int j = 0; j < MATRIX_DIMEN; j++) {
            if (num_matrix[i][j] == 2048) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Left direction movement
 * @return              true if moved
 */
static bool move_left(void)
{
    bool moved = false;
    for (int i = 0; i < MATRIX_DIMEN; i++) {
        /*Merge all adjacent buttons with the same value and record score*/
        for (int j = 0; j < MATRIX_DIMEN-1; j++) {
            if (num_matrix[i][j] != 0) {
                for (int k = j+1; k < MATRIX_DIMEN; k++) {
                    if (num_matrix[i][k] != 0) {
                        if (num_matrix[i][j] == num_matrix[i][k]) {
                            num_matrix[i][j] *= 2;
                            score += num_matrix[i][j];
                            num_matrix[i][k] = 0;
                            moved = true;
                        }
                        break;
                    }
                }
            }
        }
        /*move all numbers to left side*/
        for (int j = 0; j < MATRIX_DIMEN-1; j++) {
            if (num_matrix[i][j] == 0) {
                for (int k = j+1; k < MATRIX_DIMEN; k++) {
                    
                    if (num_matrix[i][k] != 0) {
                        num_matrix[i][j] = num_matrix[i][k];
                        num_matrix[i][k] = 0;
                        moved = true;
                        break;
                    }
                }
            }
        }
    }
    return moved;
}

/**
 * Right direction movement
 * @return              true if moved
 */
static bool move_right(void)
{
    bool moved = false;
    for (int i = 0; i < MATRIX_DIMEN; i++) {
        /*Merge all adjacent buttons with the same value and record score*/
        for (int j = MATRIX_DIMEN-1; j > 0; j--) {
            if (num_matrix[i][j] != 0) {
                for (int k = j-1; k >= 0; k--) {
                    if (num_matrix[i][k] != 0) {
                        if (num_matrix[i][j] == num_matrix[i][k]) {
                            num_matrix[i][j] *= 2;
                            score += num_matrix[i][j];
                            num_matrix[i][k] = 0;
                            moved = true;
                        }
                        break;
                    }
                }
            }
        }
        /*move all numbers to right side*/
        for (int j = MATRIX_DIMEN-1; j > 0; j--) {
            if (num_matrix[i][j] == 0) {
                for (int k = j-1; k >= 0; k--) {
                    if (num_matrix[i][k] != 0) {
                        num_matrix[i][j] = num_matrix[i][k];
                        num_matrix[i][k] = 0;
                        moved = true;
                        break;
                    }
                }
            }
        }
    }
    return moved;
}

/**
 * Up direction movement
 * @return              true if moved
 */
static bool move_up(void)
{
    bool moved = false;
    for (int j = 0; j < MATRIX_DIMEN; j++) {
        /*Merge all adjacent buttons with the same value and record score*/
        for (int i = 0; i < MATRIX_DIMEN-1; i++) {
            if (num_matrix[i][j] != 0) {
                for (int k = i+1; k < MATRIX_DIMEN; k++) {
                    if (num_matrix[k][j] != 0) {
                        if (num_matrix[i][j] == num_matrix[k][j]) {
                            num_matrix[i][j] *= 2;
                            score += num_matrix[i][j];
                            num_matrix[k][j] = 0;
                            moved = true;
                        }
                        break;
                    }
                }
            }
        }
        /*move all numbers to up side*/
        for (int i = 0; i < MATRIX_DIMEN-1; i++) {
            if (num_matrix[i][j] == 0) {
                for (int k = i+1; k < MATRIX_DIMEN; k++) {
                    if (num_matrix[k][j] != 0) {
                        num_matrix[i][j] = num_matrix[k][j];
                        num_matrix[k][j] = 0;
                        moved = true;
                        break;
                    }
                }
            }
        }
    }
    return moved;
}

/**
 * Down direction movement
 * @return              true if moved
 */
static bool move_down(void)
{
    bool moved = false;
    for (int j = 0; j < MATRIX_DIMEN; j++) {
        /*Merge all adjacent buttons with the same value and record score*/
        for (int i = MATRIX_DIMEN-1; i > 0; i--) {
            if (num_matrix[i][j] != 0) {
                for (int k = i-1; k >= 0; k--) {
                    if (num_matrix[k][j] != 0) {
                        if (num_matrix[i][j] == num_matrix[k][j]) {
                            num_matrix[i][j] *= 2;
                            score += num_matrix[i][j];
                            num_matrix[k][j] = 0;
                            moved = true;
                        }
                        break;
                    }
                }
            }
        }
        /*move all numbers to up side*/
        for (int i = MATRIX_DIMEN-1; i > 0; i--) {
            if (num_matrix[i][j] == 0) {
                for (int k = i-1; k >= 0; k--) {
                    if (num_matrix[k][j] != 0) {
                        num_matrix[i][j] = num_matrix[k][j];
                        num_matrix[k][j] = 0;
                        moved = true;
                        break;
                    }
                }
            }
        }
    }
    return moved;
}

/**
 * Get the button color regarding to number 
 * @param num           number of current button
 * @return              color of button
 */
static lv_color_t get_btn_color(uint32_t num)
{
    lv_color_t color;

    switch (num)
    {
    case 0:
        color = BUTTON_COLOR_EMPTY;
        break;
    case 2:
        color = BUTTON_COLOR_2;
        break;
    case 4:
        color = BUTTON_COLOR_4;
        break;
    case 8:
        color = BUTTON_COLOR_8;
        break;
    case 16:
        color = BUTTON_COLOR_16;
        break;   
    case 32:
        color = BUTTON_COLOR_32;
        break;
    case 64:
        color = BUTTON_COLOR_64;
        break;
    case 128:
        color = BUTTON_COLOR_128;
        break;
    case 256:
        color = BUTTON_COLOR_256;
        break;
    case 512:
        color = BUTTON_COLOR_512;
        break;
    case 1024:
        color = BUTTON_COLOR_1024;
        break;
    case 2048:
        color = BUTTON_COLOR_2048;
        break;
    
    default:
        break;
    }
    return color;
}

/**
 * the event callback to control all button color
 */
static void btnm_event_cb(lv_event_t * e)
{
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
    /*When the button matrix draws the buttons*/
    if (base_dsc->part == LV_PART_ITEMS) {
        /*Change the draw descriptor from first button*/
        if (base_dsc->id1 >= 0) {
            /*Get the corresponding number of this item*/
            static uint32_t x, y, num;
            x = (uint32_t)(base_dsc->id1) / 4;
            y = (base_dsc->id1) % 4;
            num = num_matrix[x][y];
            lv_draw_fill_dsc_t * fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
            if (fill_draw_dsc) {
                fill_draw_dsc->radius = 10;
                fill_draw_dsc->color = get_btn_color(num);
            }
            lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
            if (label_draw_dsc) {
                if (num >= 8) label_draw_dsc->color = lv_color_white();
                else label_draw_dsc->color = lv_color_hex(0x756452);
            }
        }
    }
}


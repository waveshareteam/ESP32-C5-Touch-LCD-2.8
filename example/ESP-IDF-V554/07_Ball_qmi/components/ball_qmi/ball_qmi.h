#ifndef BALL_QMI_H
#define BALL_QMI_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "qmi8658.h"
#include "esp_log.h"
#include "esp_err.h"
#include <math.h> 

// Log tag
#define BALL_QMI_TAG "ball_qmi"

// Ball configuration macros
#define MAX_SHAPES 1
#define MIN_SHAPE_SIZE 20
#define MAX_SHAPE_SIZE 20
#define ACCEL_SCALE_FACTOR 5
#define TASK_DELAY_MS 20
#define CALIBRATION_DEADZONE 0.05f

// Screen physical dimensions (adjust according to your hardware)
#define SCREEN_WIDTH_MM  33.09f
#define SCREEN_HEIGHT_MM 41.51f
#define CORNER_RADIUS_MM 9.2f

// Shape type enumeration
typedef enum {
    SHAPE_CIRCLE,
    SHAPE_COUNT
} ShapeType;

// Ball structure
typedef struct {
    lv_obj_t *obj;
    ShapeType type;
    int radius;
    int x_pos;
    int y_pos;
    lv_color_t color;
} Shape;

// Global variable declarations (accessible to main program)
extern float accel_bias_x;
extern float accel_bias_y;
extern bool calibration_done;
extern int display_width;
extern int display_height;
extern Shape shapes[MAX_SHAPES];
extern int shape_count;

// Function declarations
/**
 * @brief Get light green color (ball color)
 */
lv_color_t get_green_color(void);

/**
 * @brief Create LVGL object for circular ball
 */
lv_obj_t *create_shape_obj(ShapeType type, int size, lv_color_t color);

/**
 * @brief Generate initial ball at screen center
 */
void generate_ball(void);

/**
 * @brief QMI8658 accelerometer level calibration
 */
void ball_qmi_calibrate(qmi8658_dev_t *dev);

/**
 * @brief Calibrate acceleration data (offset cancellation + dead zone filtering)
 */
void apply_calibration(qmi8658_data_t *data);

/**
 * @brief Constrain ball position to prevent out of screen rounded corner range
 */
void constrain_ball_pos(int *x, int *y);

/**
 * @brief Ball update task (read acceleration, update position)
 */
void ball_qmi_task(void *arg);

#endif // BALL_QMI_H
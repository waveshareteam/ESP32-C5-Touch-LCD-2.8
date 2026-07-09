#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "custom_io_expander_ch32v003.h"

#define I2C_MASTER_SCL_IO   1          /*!< I2C clock pin */
#define I2C_MASTER_SDA_IO   0          /*!< I2C data pin */
#define I2C_MASTER_NUM      I2C_NUM_0   /*!< I2C port number */
#define I2C_ADDRESS         CUSTOM_IO_EXPANDER_I2C_CH32V003_ADDRESS  /*!< IO expander I2C address */

#define IO_EXP_PIN_0   IO_EXPANDER_PIN_NUM_0    
#define IO_EXP_PIN_1   IO_EXPANDER_PIN_NUM_1     
#define IO_EXP_PIN_2   IO_EXPANDER_PIN_NUM_2    
#define IO_EXP_PIN_3   IO_EXPANDER_PIN_NUM_3     
#define IO_EXP_PIN_4   IO_EXPANDER_PIN_NUM_4     
#define IO_EXP_PIN_5   IO_EXPANDER_PIN_NUM_5    
#define IO_EXP_PIN_6   IO_EXPANDER_PIN_NUM_6    
#define IO_EXP_PIN_7   IO_EXPANDER_PIN_NUM_7     

extern esp_io_expander_handle_t io_expander;
extern i2c_master_bus_handle_t i2c_handle;

void i2c_bus_init(void);
void i2c_dev_custom_io_init(void);
void ch32v003_init(void);
void ch32_test();
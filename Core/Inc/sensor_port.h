#ifndef SENSOR_PORT_H
#define SENSOR_PORT_H

#include "main.h" // For STM32 HAL and project definitions
#include "sensor_io.h"

// ============================================================================
// SPI CONNECTION CONTEXT
// ============================================================================
typedef struct {
  SPI_HandleTypeDef *hspi; // Which SPI unit to use (e.g. &hspi1)
  GPIO_TypeDef *cs_port;   // Chip Select Port (e.g. GPIOE)
  uint16_t cs_pin;         // Chip Select Pin (e.g. GPIO_PIN_3)
} SPI_Context_t;

/**
 * @brief SPI Write Wrapper
 *
 * SPI write process for LIS3DSH:
 * 1. CS Low
 * 2. Send Address (Address is sent directly since write bit is 0)
 * 3. Send Data
 * 4. CS High
 */
int8_t Sensor_SPI_Write(void *intf_ptr, uint8_t reg_addr, const uint8_t *data,
                        uint16_t len);

/**
 * @brief SPI Read Wrapper
 *
 * SPI read process for LIS3DSH:
 * 1. CS Low
 * 2. Send Address (MSB bit is set to 1, meaning reg_addr | 0x80)
 * 3. Read Data
 * 4. CS High
 */
int8_t Sensor_SPI_Read(void *intf_ptr, uint8_t reg_addr, uint8_t *data,
                       uint16_t len);

/*
// ============================================================================
// I2C CONNECTION CONTEXT (Example for future use)
// ============================================================================
typedef struct {
  I2C_HandleTypeDef *hi2c; // Which I2C unit to use (e.g. &hi2c1)
  uint16_t
      dev_addr; // Sensor I2C address (7-bit address shifted left by 1 bit)
} I2C_Context_t;

int8_t Sensor_I2C_Write(void *intf_ptr, uint8_t reg_addr, const uint8_t *data,
                        uint16_t len);
int8_t Sensor_I2C_Read(void *intf_ptr, uint8_t reg_addr, uint8_t *data,
                       uint16_t len);
*/

#endif /* SENSOR_PORT_H */

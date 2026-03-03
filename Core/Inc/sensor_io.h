#ifndef SENSOR_IO_H
#define SENSOR_IO_H

#include <stdint.h>

/**
 * @brief Generic read function pointer type for sensor communication.
 * @param intf_ptr Context pointer (e.g., SPI/I2C handle and configuration)
 * @param reg_addr Register address to read from
 * @param data Buffer to store the read data
 * @param len Number of bytes to read
 * @return 0 on success, non-zero on failure
 */
typedef int8_t (*Sensor_Read_Func)(void *intf_ptr, uint8_t reg_addr, uint8_t *data, uint16_t len);

/**
 * @brief Generic write function pointer type for sensor communication.
 * @param intf_ptr Context pointer (e.g., SPI/I2C handle and configuration)
 * @param reg_addr Register address to write to
 * @param data Data buffer to write
 * @param len Number of bytes to write
 * @return 0 on success, non-zero on failure
 */
typedef int8_t (*Sensor_Write_Func)(void *intf_ptr, uint8_t reg_addr, const uint8_t *data, uint16_t len);

/**
 * @brief Generic Sensor IO Interface structure.
 * This structure decouples the sensor driver from the MCU's specific hardware
 * interfaces (SPI/I2C).
 */
typedef struct {
  Sensor_Read_Func read; /*!< Pointer to the hardware-specific read function */
  Sensor_Write_Func write;      /*!< Pointer to the hardware-specific write function */
  void *intf_ptr; /*!< Pointer to the hardware-specific context (e.g., SPI_HandleTypeDef + CS Pin) */
} Sensor_IO_t;

#endif /* SENSOR_IO_H */

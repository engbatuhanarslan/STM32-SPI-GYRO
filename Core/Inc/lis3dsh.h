#ifndef LIS3DSH_H
#define LIS3DSH_H

#include "sensor_io.h"

// LIS3DSH Registers (Partial List for basic operation)
#define LIS3DSH_WHO_AM_I_ADDR 0x0F
#define LIS3DSH_CTRL_REG3_ADDR 0x23 // Interrupt Control Register
#define LIS3DSH_CTRL_REG4_ADDR 0x20
#define LIS3DSH_OUT_X_L_ADDR 0x28
#define LIS3DSH_OUT_X_H_ADDR 0x29
#define LIS3DSH_OUT_Y_L_ADDR 0x2A
#define LIS3DSH_OUT_Y_H_ADDR 0x2B
#define LIS3DSH_OUT_Z_L_ADDR 0x2C
#define LIS3DSH_OUT_Z_H_ADDR 0x2D

// LIS3DSH Expected Values
#define LIS3DSH_WHO_AM_I_VAL 0x3F

// Sensitivity for ±2g scale (mg/digit) -> ~0.061 mg/digit
#define LIS3DSH_SENSITIVITY_0_06G 0.061f

/**
 * @brief Accelerometer Object Context
 */
typedef struct {
  Sensor_IO_t io; /*!< Generic IO Interface for the sensor (MUST be initialized
                     by user) */

  // Processed Data (in mg)
  float x_mg;
  float y_mg;
  float z_mg;

  // Status
  uint8_t is_initialized;
  uint8_t chip_id; // FOR DEBUG - Read WHO_AM_I value
} Accelerometer_t;

/**
 * @brief Initializes the LIS3DSH accelerometer
 *
 * @param[in,out] dev Pointer to the accelerometer context
 * @return 0 on success, -1 on failure (e.g., ID mismatch or comm error)
 */
int8_t LIS3DSH_Init(Accelerometer_t *dev);

/**
 * @brief Reads the X, Y, Z acceleration data
 *
 * @param[in,out] dev Pointer to the accelerometer context
 * @return 0 on success, -1 on failure
 */
int8_t LIS3DSH_ReadData(Accelerometer_t *dev);

/**
 * @brief Routes Data Ready (DRDY) signal to INT1 pin
 *
 * @param[in,out] dev Pointer to the accelerometer context
 * @return 0 on success, -1 on failure
 */
int8_t LIS3DSH_EnableIT(Accelerometer_t *dev);

#endif /* LIS3DSH_H */

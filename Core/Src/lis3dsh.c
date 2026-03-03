#include "lis3dsh.h"
#include <stddef.h>

int8_t LIS3DSH_Init(Accelerometer_t *dev) {

  if (dev == NULL || dev->io.read == NULL || dev->io.write == NULL) {
    return -1; // Invalid context or interface
  }

  uint8_t chip_id = 0;

  // 1. Check WHO_AM_I register (0x0F address)
  if (dev->io.read(dev->io.intf_ptr, LIS3DSH_WHO_AM_I_ADDR, &chip_id, 1) != 0) {
    return -1; // Communication Error
  }

  // Save for debug
  dev->chip_id = chip_id;

  // Verify chip: LIS3DSH (0x3F) or LIS302DL (0x3B)
  if (chip_id != LIS3DSH_WHO_AM_I_VAL && chip_id != 0x3B) {
    return -1; // Wrong Chip ID
  }

  // 2. Configure Sensor
  // CTRL_REG4 (0x20): ODR=100Hz (0b0110), BDU=1 (1), ZEN=1, YEN=1, XEN=1 =>
  // 0x6F
  uint8_t ctrl_reg4_val = 0x6F;
  if (dev->io.write(dev->io.intf_ptr, LIS3DSH_CTRL_REG4_ADDR, &ctrl_reg4_val,
                    1) != 0) {
    return -1;
  }

  dev->is_initialized = 1;
  return 0; // Success
}

int8_t LIS3DSH_ReadData(Accelerometer_t *dev) {
  if (dev == NULL || !dev->is_initialized) {
    return -1;
  }

  uint8_t raw_data[6];

  // Read 6 bytes starting from OUT_X_L (X, Y, Z axes, 16-bit each)
  if (dev->io.read(dev->io.intf_ptr, LIS3DSH_OUT_X_L_ADDR, raw_data, 6) != 0) {
    return -1;
  }

  // Process raw data into mg
  int16_t x_raw = (int16_t)((raw_data[1] << 8) | raw_data[0]);
  int16_t y_raw = (int16_t)((raw_data[3] << 8) | raw_data[2]);
  int16_t z_raw = (int16_t)((raw_data[5] << 8) | raw_data[4]);

  dev->x_mg = (float)x_raw * LIS3DSH_SENSITIVITY_0_06G;
  dev->y_mg = (float)y_raw * LIS3DSH_SENSITIVITY_0_06G;
  dev->z_mg = (float)z_raw * LIS3DSH_SENSITIVITY_0_06G;

  return 0;
}

int8_t LIS3DSH_EnableIT(Accelerometer_t *dev) {
  if (dev == NULL || !dev->is_initialized) {
    return -1;
  }

  // CTRL_REG3 (0x23):
  // bit 6 (DR_EN): 1 (Data Ready interrupt enable on INT1)
  // bit 5 (IEA): 1 (Interrupt signal active HIGH)
  // bit 4 (IEL): 0 (Interrupt signal latched/pulsed -> 0 for pulsed, 1 latched)
  // bit 3 (INT2_EN): 0
  // bit 2-0: 0
  // Value: 0x48 (0100 1000)
  uint8_t ctrl_reg3_val = 0x48;
  if (dev->io.write(dev->io.intf_ptr, LIS3DSH_CTRL_REG3_ADDR, &ctrl_reg3_val,
                    1) != 0) {
    return -1;
  }

  return 0; // Success
}

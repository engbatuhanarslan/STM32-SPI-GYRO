#include "sensor_port.h"

// Maximum wait time (timeout) timer value
#define SENSOR_TIMEOUT 100

// ============================================================================
// SPI IMPLEMENTATIONS
// ============================================================================

int8_t Sensor_SPI_Write(void *intf_ptr, uint8_t reg_addr, const uint8_t *data,
                        uint16_t len) {
  if (intf_ptr == NULL)
    return -1;

  SPI_Context_t *ctx = (SPI_Context_t *)intf_ptr;

  // For writing to LIS3DSH, bit 7 = 0 (Write).
  // LIS3DSH uses a full 7-bit address, it doesn't host the "MS" (Multiple
  // Sequence) bit on the 6th bit like older generations. For Multi-byte, the
  // ADD_INC bit in CTRL_REG6(0x25) is used (default is already 1).
  uint8_t tx_addr = reg_addr;

  // To prevent broken clock signals, we combine the address and data
  // in a single array and send them together:
  uint8_t tx_buf[16] = {0};
  if ((len + 1) > sizeof(tx_buf))
    return -1;

  tx_buf[0] = tx_addr;
  for (uint16_t i = 0; i < len; i++) {
    tx_buf[i + 1] = data[i];
  }

  // 1. Pull Chip Select (CS) Pin Low (Communication starts)
  HAL_GPIO_WritePin(ctx->cs_port, ctx->cs_pin, GPIO_PIN_RESET);

  // 2. Send Address and Data in a single move
  if (HAL_SPI_Transmit(ctx->hspi, tx_buf, len + 1, SENSOR_TIMEOUT) != HAL_OK) {
    HAL_GPIO_WritePin(ctx->cs_port, ctx->cs_pin, GPIO_PIN_SET);
    return -1;
  }

  // 3. Pull Chip Select (CS) Pin High (Communication ends)
  HAL_GPIO_WritePin(ctx->cs_port, ctx->cs_pin, GPIO_PIN_SET);

  return 0; // Success
}

int8_t Sensor_SPI_Read(void *intf_ptr, uint8_t reg_addr, uint8_t *data,
                       uint16_t len) {
  if (intf_ptr == NULL)
    return -1;

  SPI_Context_t *ctx = (SPI_Context_t *)intf_ptr;

  // SPI Read Bit for LIS3DSH: Bit 7 (MSB) = 1 (Read)
  // For Multiple Read in LIS3DSH, the 6th bit MUST NOT BE 1 (It breaks OUT_X_L
  // address from 0x28 to 0x68). This process is executed automatically via
  // the hardware with the ADD_INC bit in CTRL_REG6.
  uint8_t tx_addr = reg_addr | 0x80; // Read Bit Only

  // The most reliable SPI read method in STM32 HAL is using Full-Duplex
  // (TransmitReceive). Calling Transmit and Receive functions separately might
  // cause problems as it can cut the clock in between.
  uint8_t tx_buf[16] = {
      0}; // Buffer to send (First byte is address, rest are Dummy bytes)
  uint8_t rx_buf[16] = {0}; // Buffer to receive

  if ((len + 1) > sizeof(tx_buf))
    return -1; // Prevent exceeding array bounds

  tx_buf[0] = tx_addr;

  // 1. Pull Chip Select (CS) Pin Low
  HAL_GPIO_WritePin(ctx->cs_port, ctx->cs_pin, GPIO_PIN_RESET);

  // 2. Start reading data simultaneously while sending the address
  if (HAL_SPI_TransmitReceive(ctx->hspi, tx_buf, rx_buf, len + 1,
                              SENSOR_TIMEOUT) != HAL_OK) {
    HAL_GPIO_WritePin(ctx->cs_port, ctx->cs_pin, GPIO_PIN_SET);
    return -1;
  }

  // 3. Pull Chip Select (CS) Pin High
  HAL_GPIO_WritePin(ctx->cs_port, ctx->cs_pin, GPIO_PIN_SET);

  // 4. Received data starts after the first reflection byte (rx_buf[0])
  for (uint16_t i = 0; i < len; i++) {
    data[i] = rx_buf[i + 1];
  }

  return 0; // Success
}

// ============================================================================
// I2C IMPLEMENTATIONS
// ============================================================================
/*
int8_t Sensor_I2C_Write(void *intf_ptr, uint8_t reg_addr, const uint8_t *data,
uint16_t len) { if (intf_ptr == NULL) return -1;

    I2C_Context_t *ctx = (I2C_Context_t *)intf_ptr;

    // Fill a specific register using HAL_I2C_Mem_Write
    if (HAL_I2C_Mem_Write(ctx->hi2c, ctx->dev_addr, reg_addr,
I2C_MEMADD_SIZE_8BIT, (uint8_t*)data, len, SENSOR_TIMEOUT) != HAL_OK) { return
-1;
    }
    return 0;
}

int8_t Sensor_I2C_Read(void *intf_ptr, uint8_t reg_addr, uint8_t *data, uint16_t
len) { if (intf_ptr == NULL) return -1;

    I2C_Context_t *ctx = (I2C_Context_t *)intf_ptr;

    // For reading multiple sequential registers in I2C, the LIS3DSH Sensor
requires
// the MSB (most significant bit) of the register address to be 1 if (len > 1) {
reg_addr |= 0x80;
    }

    if (HAL_I2C_Mem_Read(ctx->hi2c, ctx->dev_addr, reg_addr,
I2C_MEMADD_SIZE_8BIT, data, len, SENSOR_TIMEOUT) != HAL_OK) { return -1;
    }
    return 0;
}
*/

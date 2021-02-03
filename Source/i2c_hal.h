//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SF05 Sample Code (V1.1)
// File      :  i2c_hal.h (V1.0)
// Author    :  RFU
// Date      :  07-Nov-2012
// Controller:  STM32F100RB
// IDE       :  µVision V4.60.0.0
// Compiler  :  Armcc
// Brief     :  I2C hardware abstraction layer 
//==============================================================================

#ifndef I2C_HAL_H
#define I2C_HAL_H

//-- Includes ------------------------------------------------------------------
#include "system.h"

//-- Defines -------------------------------------------------------------------
// I2C IO-Pins
// SDA on port C, bit 6
#define SDA_LOW()  (GPIOC->BSRR = 0x00400000) // set SDA to low
#define SDA_OPEN() (GPIOC->BSRR = 0x00000040) // set SDA to open-drain
#define SDA_READ   (GPIOC->IDR  & 0x0040)     // read SDA

// SCL on port C, bit 7
#define SCL_LOW()  (GPIOC->BSRR = 0x00800000) // set SCL to low
#define SCL_OPEN() (GPIOC->BSRR = 0x00000080) // set SCL to open-drain
#define SCL_READ   (GPIOC->IDR  & 0x0080)     // read SCL

//-- Enumerations --------------------------------------------------------------
// I2C header
typedef enum{
  I2C_ADR     = 64,   // default sensor I2C address
  I2C_WRITE   = 0x00, // write bit in header
  I2C_READ    = 0x01, // read bit in header
  I2C_RW_MASK = 0x01  // bit position of read/write bit in header
}etI2cHeader;

// I2C acknowledge
typedef enum{
  ACK    = 0,
  NO_ACK = 1,
}etI2cAck;

//==============================================================================
void I2c_Init(void);
//==============================================================================
// Initializes the ports for I2C interface.
//------------------------------------------------------------------------------

//==============================================================================
void I2c_StartCondition(void);
//==============================================================================
// Writes a start condition on I2C-Bus.
//------------------------------------------------------------------------------
// remark: Timing (delay) may have to be changed for different microcontroller.
//       _____
// SDA:       |_____
//       _______
// SCL:         |___

//==============================================================================
void I2c_StopCondition(void);
//==============================================================================
// Writes a stop condition on I2C-Bus.
//------------------------------------------------------------------------------
// remark: Timing (delay) may have to be changed for different microcontroller.
//              _____
// SDA:   _____|
//            _______
// SCL:   ___|

//==============================================================================
etError I2c_WriteByte(u8t txByte);
//==============================================================================
// Writes a byte to I2C-Bus and checks acknowledge.
//------------------------------------------------------------------------------
// input:  txByte       transmit byte
//
// return: error:       ACK_ERROR = no acknowledgment from sensor
//                      NO_ERROR  = no error
//
// remark: Timing (delay) may have to be changed for different microcontroller.

//==============================================================================
u8t I2c_ReadByte(etI2cAck ack);
//==============================================================================
// Reads a byte on I2C-Bus.
//------------------------------------------------------------------------------
// input:  ack          Acknowledge: ACK or NO_ACK
//
// return: rxByte
//
// remark: Timing (delay) may have to be changed for different microcontroller.

#endif

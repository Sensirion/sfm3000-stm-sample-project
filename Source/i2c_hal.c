//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SF05 Sample Code (V1.1)
// File      :  i2c_hal.c (V1.0)
// Author    :  RFU
// Date      :  07-Nov-2012
// Controller:  STM32F100RB
// IDE       :  µVision V4.60.0.0
// Compiler  :  Armcc
// Brief     :  I2C hardware abstraction layer
//==============================================================================

//-- Includes ------------------------------------------------------------------
#include "i2c_hal.h"

//==============================================================================
void I2c_Init(void){
//==============================================================================
  RCC->APB2ENR |= 0x00000010;  // I/O port C clock enabled
  
  GPIOC->CRL   &= 0x00FFFFFF;  // set open-drain output for SDA and SCL
  GPIOC->CRL   |= 0x55000000;  // port C, bit 6,7
  
  SDA_OPEN();                  // I2C-bus idle mode SDA released
  SCL_OPEN();                  // I2C-bus idle mode SCL released
}

//==============================================================================
void I2c_StartCondition(void){
//==============================================================================
  SDA_OPEN();
  DelayMicroSeconds(1);
  SCL_OPEN();
  DelayMicroSeconds(1);
  SDA_LOW();
  DelayMicroSeconds(10);  // hold time start condition (t_HD;STA)
  SCL_LOW();
  DelayMicroSeconds(10);
}

//==============================================================================
void I2c_StopCondition(void){
//==============================================================================
  SCL_LOW();
  DelayMicroSeconds(1);
  SDA_LOW();
  DelayMicroSeconds(1);
  SCL_OPEN();
  DelayMicroSeconds(10);  // set-up time stop condition (t_SU;STO)
  SDA_OPEN();
  DelayMicroSeconds(10);
}

//==============================================================================
etError I2c_WriteByte(u8t txByte){
//==============================================================================
  u8t     mask;
  etError error = NO_ERROR;
  for(mask = 0x80; mask > 0; mask >>= 1)// shift bit for masking (8 times)
  {
    if((mask & txByte) == 0) SDA_LOW(); // masking txByte, write bit to SDA-Line
    else                     SDA_OPEN();
    DelayMicroSeconds(1);               // data set-up time (t_SU;DAT)
    SCL_OPEN();                         // generate clock pulse on SCL
    DelayMicroSeconds(5);               // SCL high time (t_HIGH)
    SCL_LOW();
    DelayMicroSeconds(1);               // data hold time(t_HD;DAT)
  }
  SDA_OPEN();                           // release SDA-line
  SCL_OPEN();                           // clk #9 for ack
  DelayMicroSeconds(1);                 // data set-up time (t_SU;DAT)
  if(SDA_READ) error = ACK_ERROR;       // check ack from i2c slave
  SCL_LOW();
  DelayMicroSeconds(20);                // wait to see byte package on scope
  return error;                         // return error code
}

//==============================================================================
u8t I2c_ReadByte(etI2cAck ack){
//==============================================================================
  u8t mask;
  u8t rxByte = NO_ERROR;
  SDA_OPEN();                            // release SDA-line
  for(mask = 0x80; mask > 0; mask >>= 1) // shift bit for masking (8 times)
  { 
    SCL_OPEN();                          // start clock on SCL-line
    DelayMicroSeconds(3);                // SCL high time (t_HIGH)
    if(SDA_READ) rxByte = rxByte | mask; // read bit
    SCL_LOW();
    DelayMicroSeconds(1);                // data hold time(t_HD;DAT)
  }
  if(ack == ACK) SDA_LOW();              // send acknowledge if necessary
  else           SDA_OPEN();
  DelayMicroSeconds(1);                  // data set-up time (t_SU;DAT)
  SCL_OPEN();                            // clk #9 for ack
  DelayMicroSeconds(5);                  // SCL high time (t_HIGH)
  SCL_LOW();
  SDA_OPEN();                            // release SDA-line
  DelayMicroSeconds(20);                 // wait to see byte package on scope
  return rxByte;                         // return error code
}

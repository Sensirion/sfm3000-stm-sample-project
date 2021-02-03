//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SF05 Sample Code (V1.1)
// File      :  sf05.c (V1.2)
// Author    :  RFU
// Date      :  20-Aug-2015
// Controller:  STM32F100RB
// IDE       :  µVision V4.60.0.0
// Compiler  :  Armcc
// Brief     :  Sensor Layer: Implementation of functions for sensor access.
//==============================================================================

//-- Includes ------------------------------------------------------------------
#include "sf05.h"
#include "i2c_hal.h"

//-- Global Variables ----------------------------------------------------------
u16t currentCommand = 0x0000;

//==============================================================================
void SF05_Init(void){
//==============================================================================
  I2c_Init(); // init I2C
}

//==============================================================================
etError SF05_WriteCommand(etCommands cmd){
//==============================================================================
  etError error; // error code
 
  // write command to sensor
  I2c_StartCondition();
  error  = I2c_WriteByte(I2C_ADR << 1 | I2C_WRITE);
  error |= I2c_WriteByte(cmd >> 8);
  error |= I2c_WriteByte(cmd & 0xFF);
  I2c_StopCondition();
  
  // if no error, store current command
  if(error == NO_ERROR)
    currentCommand = cmd;
  
  return error;
}

//==============================================================================
etError SF05_ReadCommandResult(u16t *result){
//==============================================================================
  etError error;    // error code
  u8t     checksum; // checksum byte
  u8t     data[2];  // read data array
 
  // read command result & checksum from sensor
  I2c_StartCondition();
  error    = I2c_WriteByte(I2C_ADR << 1 | I2C_READ);
  data[0]  = I2c_ReadByte(ACK);
  data[1]  = I2c_ReadByte(ACK);
  checksum = I2c_ReadByte(NO_ACK);
  I2c_StopCondition();
  
  // checksum verification
  error  |= SF05_CheckCrc (data, 2, checksum);
  
  // if no error, combine 16-bit result from the read data array
  if(error == NO_ERROR)
    *result = (data[0] << 8) | data[1];
  
  return error;
}

//==============================================================================
etError SF05_ReadCommandResultWithTimeout(u8t maxRetries, u16t *result){
//==============================================================================
  etError error; //variable for error code
  
  while(maxRetries--)
  {
    // try to read command result
    error = SF05_ReadCommandResult(result); 
    
    // if read command result was successful -> exit loop
    // it will only be successful if a new valid measurement was performed
    if(error == NO_ERROR) break;
    
    // if it was not successful -> wait a short time and then try it again
    DelayMicroSeconds(10000);
  }

  return error;
}
 
//==============================================================================
etError SF05_GetFlow(ft offset, ft scale, ft *flow){
//==============================================================================
  etError error = NO_ERROR; // error code
  u16t    result;           // read result from sensor
  
  // write command if it is not already set 
  if(currentCommand != FLOW_MEASUREMENT)
    error = SF05_WriteCommand(FLOW_MEASUREMENT);
  
  // if no error, read command result
  if(error == NO_ERROR)
    error = SF05_ReadCommandResultWithTimeout(20, &result);

  // if no error, compute the flow
  if(error == NO_ERROR)
    *flow = ((ft)result - offset) / scale;
  
  return error;
}

//==============================================================================
etError SF05_GetSerialNumber(u32t *serialNumber){
//==============================================================================
  etError error = NO_ERROR; // error code
  u16t result;              // read result from sensor
  
  // write command "read serial number (bit 31:16)"
  error = SF05_WriteCommand(READ_SERIAL_NUMBER_HIGH);
  
  // if no error, read command result
  if(error == NO_ERROR)
    error = SF05_ReadCommandResult(&result);
  
  // if no error, copy upper 16 bits to serial number
  if(error == NO_ERROR)
    *serialNumber = result << 16;
  
  // if no error, write command "read serial number (bit 15:0)"
  if(error == NO_ERROR)
    error = SF05_WriteCommand(READ_SERIAL_NUMBER_LOW);
  
  // if no error, read command result
  if(error == NO_ERROR)
    error = SF05_ReadCommandResult(&result);
  
  // if no error, copy lower 16 bits to serial number
  if(error == NO_ERROR)
    *serialNumber |= result;
  
  return error;
}

//==============================================================================
etError SF05_SoftReset(void){
//==============================================================================
  etError error; // error code
  
  error = SF05_WriteCommand(SOFT_RESET);
  
  return error;
}

//==============================================================================
etError SF05_CheckCrc(u8t data[], u8t nbrOfBytes, u8t checksum){
//==============================================================================
  u8t bit;     // bit mask
  u8t crc = 0; // calculated checksum
  u8t byteCtr; // byte counter
  
  // calculates 8-Bit checksum with given polynomial
  for(byteCtr = 0; byteCtr < nbrOfBytes; byteCtr++)
  {
    crc ^= (data[byteCtr]);
    for(bit = 8; bit > 0; --bit)
    {
      if(crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
      else           crc = (crc << 1);
    }
  }
  
  // verify checksum
  if(crc != checksum) return CHECKSUM_ERROR;
  else                return NO_ERROR;
}


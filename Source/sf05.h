//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SF05 Sample Code (V1.1)
// File      :  sf05.h (V1.2)
// Author    :  RFU
// Date      :  20-Aug-2015
// Controller:  STM32F100RB
// IDE       :  µVision V4.60.0.0
// Compiler  :  Armcc
// Brief     :  Sensor Layer: Definitions of commands and functions for sensor
//                            access.
//==============================================================================

#ifndef SF05_H
#define SF05_H

//-- Includes ------------------------------------------------------------------
#include "system.h"

//-- Defines -------------------------------------------------------------------
// CRC
#define POLYNOMIAL  0x131    // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

//-- Enumerations --------------------------------------------------------------
// Sensor Commands
typedef enum{
  FLOW_MEASUREMENT        = 0x1000, // command: flow measurement
  READ_SERIAL_NUMBER_HIGH = 0x31AE, // command: read serial number (bit 31:16)
  READ_SERIAL_NUMBER_LOW  = 0x31AF, // command: read serial number (bit 15:0)
  SOFT_RESET              = 0X2000  // command: soft reset
}etCommands;

//==============================================================================
void SF05_Init(void);
//==============================================================================
// Initializes the I2C bus for communication with the sensor.
//------------------------------------------------------------------------------

//==============================================================================
etError SF05_WriteCommand(etCommands cmd);
//==============================================================================
// Writes command to the sensor.
//------------------------------------------------------------------------------
// input:  cmd          command which is to be written to the sensor
//
// return: error:       ACK_ERROR = no acknowledgment from sensor
//                      NO_ERROR  = no error

//==============================================================================
etError SF05_ReadCommandResult(u16t *result);
//==============================================================================
// Reads command results from sensor.
//------------------------------------------------------------------------------
// input:  *result      pointer to an integer where the result will be stored
//
// return: errror:      ACK_ERROR      = no acknowledgment from sensor
//                      CHECKSUM_ERROR = checksum mismatch
//                      NO_ERROR       = no error

//==============================================================================
etError SF05_ReadCommandResultWithTimeout(u8t maxRetries, u16t *result);
//==============================================================================
// Reads command results from sensor. If an error occurs, then the read will be
// repeated after a short wait (approx. 10ms).
//------------------------------------------------------------------------------
// input:  maxRetries    maximum number of retries
//         *result       pointer to an integer where the result will be stored  
//  
// return: errror:       ACK_ERROR      = no acknowledgment from sensor
//                       CHECKSUM_ERROR = checksum mismatch
//                       NO_ERROR       = no error
//
// remark: This function is usefull for reading measurement results. If not yet
//         a new valid measurement was performed, an acknowledge error occurs
//         and the read will be automatical repeated until a valid measurement
//         could be read.

//==============================================================================
etError SF05_GetFlow(ft offset, ft scale, ft *flow);
//==============================================================================
// Gets the flow from the sensor in a predefined unit. The "flow measurement"
// command will be automatical written to the sensor, if it is not already set.
//------------------------------------------------------------------------------
// input:  offset         offset flow
//         scale          scale factor flow
//         *flow          pointer to a floating point value, where the calculated
//                        flow will be stored
// 
// return: errror:        ACK_ERROR      = no acknowledgment from sensor
//                        CHECKSUM_ERROR = checksum mismatch
//                        NO_ERROR       = no error
//
// remark: The result will be converted according to the following formula:
//         flow in predefined unit = (measurement_result - offset) / scale

//==============================================================================
etError SF05_GetSerialNumber(u32t *serialNumber);
//==============================================================================
// Gets the serial number from the sensor.
//------------------------------------------------------------------------------
// input:  *serialNumber  pointer to a 32-bit integer, where the serial number
//                        will be stored
//
// return: error:         ACK_ERROR      = no acknowledgment from sensor
//                        CHECKSUM_ERROR = checksum mismatch
//                        NO_ERROR       = no error

//==============================================================================
etError SF05_SoftReset(void);
//==============================================================================
// Forces a sensor reset without switching the power off and on again.
//------------------------------------------------------------------------------
// return: error:         ACK_ERROR      = no acknowledgment from sensor
//                        NO_ERROR       = no error

//==============================================================================
etError SF05_CheckCrc(u8t data[], u8t nbrOfBytes, u8t checksum);
//==============================================================================
// Calculates checksum for n bytes of data and compares it with expected
// checksum.
//------------------------------------------------------------------------------
// input:  data[]         checksum is built based on this data
//         nbrOfBytes     checksum is built for n bytes of data
//         checksum       expected checksum
//  
// return: error:         CHECKSUM_ERROR = checksum does not match
//                        NO_ERROR       = checksum matches

#endif

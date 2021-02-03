//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SF05 Sample Code (V1.1)
// File      :  main.c (V1.1)
// Author    :  RFU
// Date      :  26-Jul-2015
// Controller:  STM32F100RB
// IDE       :  µVision V4.60.0.0
// Compiler  :  Armcc
// Brief     :  This code shows how to implement the basic commands for a
//              flow or differential pressure senor based on SF05 sensor chip.
//              Due to compatibility reasons the I2C interface is implemented
//              as "bit-banging" on normal I/O's. This code is written for an
//              easy understanding and is neither optimized for speed nor code
//              size.
//
// Porting to a different microcontroller (uC):
//   - the definitions of basic types may have to be changed  in typedefs.h
//   - change the port functions / definitions for your uC    in i2c_hal.h/.c
//   - adapt the timing of the delay function for your uC     in system.c
//   - adapt the SystemInit()                                 in system.c
//   - change the uC register definition file <stm32f10x.h>   in system.h
//==============================================================================

//-- Includes ------------------------------------------------------------------
#include "system.h"
#include "sf05.h"

//-- Defines -------------------------------------------------------------------
// Offset and scale factors from datasheet (SFM3000).
#define OFFSET_FLOW 32000.0F   // offset flow
#define SCALE_FLOW    140.0F   // scale factor flow

//==============================================================================
void Led_Init(void){
//==============================================================================
  RCC->APB2ENR |= 0x00000010;  // I/O port C clock enabled
  GPIOC->CRH   &= 0xFFFFFF00;  // set general purpose output mode for LEDs
  GPIOC->CRH   |= 0x00000011;  //
  GPIOC->BSRR   = 0x03000000;  // LEDs off
}

//==============================================================================
void UserButton_Init(void){
//==============================================================================
  RCC->APB2ENR |= 0x00000004;  // I/O port A clock enabled
  GPIOA->CRH   &= 0xFFFFFFF0;  // set general purpose input mode for User Button
  GPIOA->CRH   |= 0x00000004;  //
}

//==============================================================================
void LedBlueOn(void){
//==============================================================================
  GPIOC->BSRR = 0x00000100;
}

//==============================================================================
void LedBlueOff(void){
//==============================================================================
  GPIOC->BSRR = 0x01000000;
}

//==============================================================================
void LedGreenOn(void){
//==============================================================================
  GPIOC->BSRR = 0x00000200;
}

//==============================================================================
void LedGreenOff(void){
//==============================================================================
  GPIOC->BSRR = 0x02000000;
}

//==============================================================================
u8t ReadUserButton(void){
//==============================================================================
  return (GPIOA->IDR & 0x00000001);
}

//==============================================================================
int main(void){
//==============================================================================
  etError  error;        // error code
  u32t     serialNumber; // sensor serial number
  ft       flow;         // measured flow value

  SystemInit();
  Led_Init();
  UserButton_Init();
  SF05_Init();
  
  // read serial number from sensor
  error = SF05_GetSerialNumber(&serialNumber);

  while(1)
  {
    error = NO_ERROR;
    
    if(ReadUserButton() == 0)
    // if the user button is not pressed
    { 
      // read flow from sensor
      error = SF05_GetFlow(OFFSET_FLOW, SCALE_FLOW, &flow);
      
      // the blue LED lights if a weak flow is detected
      if(flow > 1.0) LedBlueOn();
      else           LedBlueOff();
    }
    else
    // if the user button is pressed
    { 
      // green and blue LED off
      LedGreenOff();
      LedBlueOff();
      
      // perform a soft reset on the sensor
      error = SF05_SoftReset();
      
      // wait until button is released
      while(ReadUserButton() != 0);
    }
    
    // the green LED lights if no error occurs
    if(!error)     LedGreenOn();
    else           LedGreenOff();
    
    // wait 100ms
    DelayMicroSeconds(100000);
  }
}

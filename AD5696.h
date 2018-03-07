/**************************************************************************/
/*!
      @file     AD5696.h
      @author   A. Cooper (Aesc)
	    @license  None (see license.txt)

	This is a library for the A5696 4 Channel I2C DAC
	----> https://www.mouser.co.uk/ProductDetail/584-AD5696ARUZ?r 584-AD5696ARUZ


	@section  HISTORY

    v1.0  - First release
*/
/**************************************************************************/

#if ARDUINO >  100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

#define DAC_A    (0b0000)
#define DAC_B    (0b0001)
#define DAC_C    (0b0010)
#define DAC_D    (0b0011)

#define CH_NONE (0b0000)
#define CH_A    (0b0001)
#define CH_B    (0b0010)
#define CH_C    (0b0100)
#define CH_D    (0b1000)
#define CH_ALL  (0b1111)

#define ADDRMSB (0b01100)  //7 BIT ADDRESS FIRST FIVE MSB 00011 << 2

#define   NoOperation           (0b0000)
#define   WriteInputRegister    (0b0001)
#define   UpdateDacWithInput    (0b0010)
#define   WriteUpdate           (0b0011)

#define   stateNormal      (0b0000)
#define   stateGND1k       (0b0001)
#define   state100k     (0b0010)
#define   stateNC          (0b0011)



class AD5696{
 public:
  AD5696();
  void begin();
  void setDAC(uint8_t addr, uint8_t channel, uint8_t operation, uint16_t value);
  void readDAC(uint8_t addr, uint8_t channel, uint8_t operation, uint16_t *returnArray);
  void setPowerState(uint8_t addr, uint8_t A_State, uint8_t B_State, uint8_t C_State, uint8_t D_State);
  void setLDACMask(uint8_t addr, uint8_t channel);
  void reset(uint8_t addr);

};

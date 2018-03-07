/**************************************************************************/
/*!
    @file     AD5696.cpp
    @author   A. Cooper (Isotopash)
    @license  None (see license.txt)

	This is a library for the AD5696 4 Channel I2C DAC
	----> https://www.mouser.co.uk/ProductDetail/584-AD5696ARUZ?r=584-AD5696ARUZ

  I2C Address
  name    addr    A1    A0  (pins)
  DAC_A     0     GND   GND
  DAC_B     1     GND   V
  DAC_C     2     V     GND
  DAC_D     3     V     V

 i2c address is 7 bit. 5 Most Significant Bits are 00011. The last 2 Bits
 are the address given by A1 and A0 pins.


	@section  HISTORY

    v1.0  - First release
*/
/**************************************************************************/
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

#include "AD5696.h"

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static uint8_t i2cread(void) {
  #if ARDUINO >= 100
  return Wire.read();
  #else
  return Wire.receive();
  #endif
}

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library
*/
/**************************************************************************/
static void i2cwrite(uint8_t x) {
  #if ARDUINO >= 100
  Wire.write((uint8_t)x);
  #else
  Wire.send(x);
  #endif
}
/**************************************************************************/
/*!
    @brief  Returns number of bits set to 1 in an 8-bit byte.
*/
/**************************************************************************/
static uint8_t countSetBits(uint8_t n)
{
  uint8_t count = 0;
  while (n)
  {
    count += n & 1;
    n >>= 1;
  }
  return count;
}

/**************************************************************************/
/*!
    @brief  Instantiates a new AD5696 class
*/
/**************************************************************************/
AD5696::AD5696() {
}

/**************************************************************************/
/*!
    @brief  Setups the HW
*/
/**************************************************************************/
void AD5696::begin() {


  Wire.begin();

}

/**************************************************************************/
/*!
    @brief  Sets the output voltage to a fraction of source vref.  (Value
            can be 0..65,536)
    @param[in]  addr
                                                      A1    A0  (pins)    7 bit addr
                DAC_A               or  0         :   GND   GND           0x08
                DAC_B               or  1         :   GND   V             0x09
                DAC_C               or  2         :   V     GND           0x0A
                DAC_D               or  3         :   V     V             0x0B
    @param[in]  channel   - use "|" to select multiple channels
                CH_A                or  1         :   Channel A selected
                CH_A | CH_C         or  1|4       :   Channel A,B selected
                CH_A | CH_B | CH_D  or  1|2|8     :   Channel A,B,D selected
                ALL                 or  15        :   All Channels selected
    @param[in]  operation
                NoOperation         or  0         :   No Operation
                WriteInputRegister  or  1         :   Write to Input Register n (dependent on LDAC)
                UpdateDacWithInput  or  2         :   Update DAC Register n with contents of Input Register "channel"
                WriteUpdate         or  3         :   Write to and update DAC Channel "channel"
    @param[in]  value
                The 16-bit value representing the relationship between
                the DAC's input voltage and its output voltage.

*/
/**************************************************************************/
void AD5696::setDAC(uint8_t addr, uint8_t channel, uint8_t operation, uint16_t value)
{
  // Start I2C transmission
  Wire.beginTransmission(ADDRMSB + addr);
  // Command And Channel
  i2cwrite((operation << 4) + (channel));

  //Split 16 bit into two 8 Bits
  i2cwrite(value >> 8);  //MSB
  i2cwrite(value & 0x00FF); //LSB

  // Stop I2C transmission
  Wire.endTransmission();
}
/**************************************************************************/
/*!
    @brief  Gets the output voltage to a fraction of source vref.  (Value
            can be 0..65,536)
    @param[in]  addr
                                                      A1    A0  (pins)    7 bit addr
                DAC_A               or  0         :   GND   GND           0x08
                DAC_B               or  1         :   GND   V             0x09
                DAC_C               or  2         :   V     GND           0x0A
                DAC_D               or  3         :   V     V             0x0B
    @param[in]  channel   - use "|" to select multiple channels
                CH_A                or  1         :   Channel A selected
                CH_A | CH_B         or  1|2       :   Channel A,B selected
                CH_A | CH_B | CH_D  or  1|2|8     :   Channel A,B,D selected
                ALL                 or  15        :   All Channels selected
    @param[in]  operation
                NoOperation         or  0         :   No Operation
                WriteInputRegister  or  1         :   Write to Input Register n (dependent on LDAC)
                UpdateDacWithInput  or  2         :   Update DAC Register n with contents of Input Register "channel"
                WriteUpdate         or  3         :   Write to and update DAC Channel "channel"
    @param[out] *returnArray
                The 16-bit value representing the relationship between
                the DAC's input voltage and its output voltage.
                If 3 Dacs were selected an array of 3 uint16_t will be returned
                in alphabetical order.

*/
/**************************************************************************/
void AD5696::readDAC(uint8_t addr, uint8_t channel, uint8_t operation, uint16_t *returnArray)
{
  uint8_t i;
  // Start I2C transmission
  Wire.beginTransmission(ADDRMSB + addr);
  // Command And Channel
  i2cwrite((operation << 4) + (channel));
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)(ADDRMSB + addr), (uint8_t)2);

  for (i=0;i<countSetBits(channel);i++)
  {
    returnArray[i] = ((i2cread() << 8) | i2cread());
  }

}

/**************************************************************************/
/*!
    @brief  Any or all DACs (DAC A to DAC D) can be powered down to the
            selected mode by setting the corresponding bits in the input
            shift register.
            In normal power consumption pins supply is 0.59 mA at 5 V
            In power-down mode, the supply current falls to 4 μA at 5 V.
    @param[in]  addr
                                                      A1    A0  (pins)    7 bit addr
                DAC_A               or  0         :   GND   GND           0x08
                DAC_B               or  1         :   GND   V             0x09
                DAC_C               or  2         :   V     GND           0x0A
                DAC_D               or  3         :   V     V             0x0B
    @param[in]  X_State (Where X is Channel)
                0: Normal Operation
                1: 1 kΩ to GND
                2: 100 kΩ to GND
                3: Three-State (NC)
*/
/**************************************************************************/
void AD5696::setPowerState(uint8_t addr, uint8_t A_State, uint8_t B_State, uint8_t C_State, uint8_t D_State)
{
  Wire.beginTransmission(ADDRMSB + addr);

  //Power down/power up DAC command
  i2cwrite(0b01000000);

  //Empty line (Dont care bits)
  i2cwrite(0);
  //Select Outputs by Bit Shifting
  i2cwrite((D_State << 6) + (C_State << 4) + (B_State << 2) + (A_State));

  Wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  LDAC MASK REGISTER
            The default value of these bits is 0; that is, the LDAC pin works
            normally. Setting any of these bits to 1 forces the selected DAC
            channel to ignore transitions on the LDAC pin, regardless of the
            state of the hardware LDAC pin. This flexibility is useful in
            applications where the user wishes to select which channels
            respond to the LDAC pin. When LDAC is set HIGH either by software
            or LDAC_PIN state, the value in the input register will be copied
            to the DAC channel.
    @param[in]  addr
                                                      A1    A0  (pins)    7 bit addr
                DAC_A               or  0         :   GND   GND           0x08
                DAC_B               or  1         :   GND   V             0x09
                DAC_C               or  2         :   V     GND           0x0A
                DAC_D               or  3         :   V     V             0x0B
    @param[in]  channel   - use "|" to select multiple channels
                CH_NONE             or  0         :   All Channels as LDAC_PIN
                CH_A                or  1         :   Channel A LDAC set HIGH, Channel B,C,D LDAC as LDAC_PIN
                CH_A | CH_B         or  1|2       :   Channel A,B LDAC set HIGH, Channel C,D LDAC as LDAC_PIN
                CH_A | CH_B | CH_D  or  1|2|8     :   Channel A,B,D LDAC set HIGH, Channel B,C,D LDAC as LDAC_PIN
                CH_ALL              or  15        :   All Channels LDAC set HIGH

                -values:
                0: Determined by the LDAC pin.
                1: DAC channels are updated. (DAC channels see LDAC pin as 1.)

*/
/**************************************************************************/
void AD5696::setLDACMask(uint8_t addr, uint8_t channel)
{
  Wire.beginTransmission(ADDRMSB + addr);

  //Power down/power up DAC command
  i2cwrite(0b01010000);

  //Empty line (Dont care bits)
  i2cwrite(0);

  i2cwrite(channel);

  Wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  Software Reset
            Resets the DAC to the power-on reset code. Any events
            on LDAC during a power-on reset are ignored. If the RESET
            pin is pulled low at power-up, the device does not initialize
            correctly until the pin is released.
@param[in]  addr
                                                  A1    A0  (pins)    7 bit addr
            DAC_A               or  0         :   GND   GND           0x08
            DAC_B               or  1         :   GND   V             0x09
            DAC_C               or  2         :   V     GND           0x0A
            DAC_D               or  3         :   V     V             0x0B
*/
/**************************************************************************/
void AD5696::reset(uint8_t addr)
{
  Wire.beginTransmission(ADDRMSB + addr);
  //Software reset (power-on reset)
  i2cwrite(0b01100000);

  //Empty line (Dont care bits)
  i2cwrite(0);
  i2cwrite(0);

  Wire.endTransmission();
}

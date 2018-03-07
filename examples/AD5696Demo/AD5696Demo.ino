/**************************************************************************/
/*!
    @file     AD5696Demo.ino
    @author   A. Cooper (Isotopash)
    @license  None (see license.txt)

    Simple Example for the AD5696 DAC.

    AD5696 4 Channel I2C DAC
  	----> https://www.mouser.co.uk/ProductDetail/584-AD5696ARUZ?r=584-AD5696ARUZ
*/
/**************************************************************************/
#include <Wire.h>
#include <AD5696.h>

AD5696 dac;




void setup() {

  dac.begin();


  dac.setPowerState(DAC_A, stateNormal, stateNC, stateNormal, stateNC);
  //Only Channel A and C enabled, Channel B an D powered down
  //to save energy

}

void loop() {

  dac.setDAC(DAC_A, CH_A | CH_C, WriteUpdate, 0);  //DAC A, CH A and C to 0 Minimum
  delay(1000);

  dac.setDAC(DAC_A, CH_A | CH_C, WriteUpdate, 32768); //DAC A, CH A and C to 32768
  delay(1000);

  dac.setDAC(DAC_A, CH_A, WriteUpdate, 65535);  //DAC A, CH A to 65535 Maximum
  delay(1000);

// Output at 65535 is 1 x Vref with GAIN pin LOW, 2 x Vref with GAIN pin HIGH

}

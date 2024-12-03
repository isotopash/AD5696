#include <Wire.h>
#include "AD569X.h"

// Define DAC address (e.g., DAC_A = 0x08, DAC_B = 0x09, DAC_C = 0x0A, DAC_D = 0x0B)
#define DAC_ADDRESS 0x08

// Create an instance of the AD569X DAC class
AD569X dac;

void setup() {
  // Start serial communication for debugging
  Serial.begin(9600);

  // Initialize the DAC
  dac.begin();

  // Set DAC_A (channel A) to a value of 32768 (half of 65536)
  uint16_t dacValue = 32768;
  dac.setDAC(DAC_ADDRESS, CH_A, WriteUpdate, dacValue);
  Serial.println("DAC_A set to 32768 (half of max value)");

  // Set DAC_B and DAC_C to different values
  dac.setDAC(DAC_ADDRESS, CH_B, WriteUpdate, 16384); // Set DAC_B to 16384
  dac.setDAC(DAC_ADDRESS, CH_C, WriteUpdate, 49152); // Set DAC_C to 49152

  // Read the values of DAC_A and DAC_B
  uint16_t readValues[2]; // Array to store the returned values
  dac.readDAC(DAC_ADDRESS, CH_A | CH_B, WriteUpdate, readValues);

  // Print the read DAC values
  Serial.print("Read DAC_A value: ");
  Serial.println(readValues[0]);
  Serial.print("Read DAC_B value: ");
  Serial.println(readValues[1]);

  // Set power-down state for DAC_A (power down mode 100kΩ to GND)
  dac.setPowerState(DAC_ADDRESS, 0, 2, 0, 0); // Only DAC_B is in power-down mode
  Serial.println("DAC_A powered down to 100kΩ mode");

  // Reset the DAC to its default state
  dac.reset(DAC_ADDRESS);
  Serial.println("DAC reset to power-on state");

  delay(1000); // Delay to allow time for actions to take effect
}

void loop() {
  // Main loop, if needed for other logic

  // Set DAC_D to a new value (e.g., 4096)
  dac.setDAC(DAC_ADDRESS, CH_D, WriteUpdate, 4096);
  Serial.println("DAC_D set to 4096");

  delay(2000); // Wait 2 seconds before updating again
}

/**************************************************************************/
/*!
    @file     AD569X.cpp
    @author   A. Cooper (Isotopash)
    @license  None (see license.txt)

    This is a library for the AD569X 4-Channel I2C DAC.

    @section I2C Address
    - DAC_A: 0x08 (A1 = GND, A0 = GND)
    - DAC_B: 0x09 (A1 = GND, A0 = V)
    - DAC_C: 0x0A (A1 = V, A0 = GND)
    - DAC_D: 0x0B (A1 = V, A0 = V)

    The I2C address is 7-bit. The first 5 MSB are `00011`, and the last 2 bits
    are determined by the A1 and A0 pins.

    @section HISTORY
    v1.0 - First release
    V1.0.1 - Code Cleanup 
*/
/**************************************************************************/

#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include <Wire.h>
#include "AD569X.h"

/**************************************************************************/
/*!
    @brief  Abstract away platform differences in Arduino wire library for reading
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
    @brief  Abstract away platform differences in Arduino wire library for writing
*/
/**************************************************************************/
static void i2cwrite(uint8_t data) {
    #if ARDUINO >= 100
        Wire.write(data);
    #else
        Wire.send(data);
    #endif
}

/**************************************************************************/
/*!
    @brief  Returns the number of bits set to 1 in an 8-bit byte.
*/
/**************************************************************************/
static uint8_t countSetBits(uint8_t n) {
    uint8_t count = 0;
    while (n) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

/**************************************************************************/
/*!
    @brief  Instantiates a new AD569X object
*/
/**************************************************************************/
AD569X::AD569X() {
    // Constructor logic (if needed) goes here
}

/**************************************************************************/
/*!
    @brief  Initializes the hardware (I2C communication setup)
*/
/**************************************************************************/
void AD569X::begin() {
    Wire.begin(); // Initialize I2C communication
}

/**************************************************************************/
/*!
    @brief  Sets the output voltage to a fraction of the reference voltage.
            The value can range from 0 to 65,536.
    @param[in]  addr  The I2C address of the DAC (0 to 3)
    @param[in]  channel  The channel(s) to update (e.g., CH_A | CH_B | ...)
    @param[in]  operation  The operation to perform (e.g., WriteUpdate, etc.)
    @param[in]  value  The 16-bit DAC value representing the desired output voltage
*/
/**************************************************************************/
void AD569X::setDAC(uint8_t addr, uint8_t channel, uint8_t operation, uint16_t value) {
    Wire.beginTransmission(ADDRMSB + addr);  // Start I2C transmission
    i2cwrite((operation << 4) + channel);     // Send command and channel
    i2cwrite(value >> 8);                    // Send MSB of the 16-bit value
    i2cwrite(value & 0x00FF);                // Send LSB of the 16-bit value
    Wire.endTransmission();                  // End I2C transmission
}

/**************************************************************************/
/*!
    @brief  Reads the output voltage of the specified DAC channels.
            The value can range from 0 to 65,536.
    @param[in]  addr  The I2C address of the DAC (0 to 3)
    @param[in]  channel  The channel(s) to read (e.g., CH_A | CH_B | ...)
    @param[in]  operation  The operation to perform (e.g., UpdateDacWithInput, etc.)
    @param[out] *returnArray  An array to store the returned values (16-bit voltage values)
*/
/**************************************************************************/
void AD569X::readDAC(uint8_t addr, uint8_t channel, uint8_t operation, uint16_t *returnArray) {
    uint8_t i;

    Wire.beginTransmission(ADDRMSB + addr);  // Start I2C transmission
    i2cwrite((operation << 4) + channel);     // Send command and channel
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)(ADDRMSB + addr), (uint8_t)2); // Request 2 bytes of data

    // Store the read data in returnArray
    for (i = 0; i < countSetBits(channel); i++) {
        returnArray[i] = ((i2cread() << 8) | i2cread());
    }
}

/**************************************************************************/
/*!
    @brief  Sets the power state for the DAC channels (Normal, Power-Down, etc.)
    @param[in]  addr  The I2C address of the DAC (0 to 3)
    @param[in]  A_State, B_State, C_State, D_State  The power state for each channel (0 to 3)
*/
/**************************************************************************/
void AD569X::setPowerState(uint8_t addr, uint8_t A_State, uint8_t B_State, uint8_t C_State, uint8_t D_State) {
    Wire.beginTransmission(ADDRMSB + addr);  // Start I2C transmission
    i2cwrite(0b01000000);                    // Power control command
    i2cwrite(0);                             // Empty line (don't care bits)
    i2cwrite((D_State << 6) + (C_State << 4) + (B_State << 2) + A_State);  // Set the power states for the channels
    Wire.endTransmission();                  // End I2C transmission
}

/**************************************************************************/
/*!
    @brief  Configures the LDAC mask register to control which DACs respond to the LDAC pin
    @param[in]  addr  The I2C address of the DAC (0 to 3)
    @param[in]  channel  Channels to mask (e.g., CH_A | CH_B | CH_D)
*/
/**************************************************************************/
void AD569X::setLDACMask(uint8_t addr, uint8_t channel) {
    Wire.beginTransmission(ADDRMSB + addr);  // Start I2C transmission
    i2cwrite(0b01010000);                    // LDAC mask command
    i2cwrite(0);                             // Empty line (don't care bits)
    i2cwrite(channel);                        // Set the LDAC mask for the selected channels
    Wire.endTransmission();                  // End I2C transmission
}

/**************************************************************************/
/*!
    @brief  Resets the DAC to its power-on reset state (ignores LDAC events during reset)
    @param[in]  addr  The I2C address of the DAC (0 to 3)
*/
/**************************************************************************/
void AD569X::reset(uint8_t addr) {
    Wire.beginTransmission(ADDRMSB + addr);  // Start I2C transmission
    i2cwrite(0b01100000);                    // Software reset command
    i2cwrite(0);                             // Empty line (don't care bits)
    i2cwrite(0);                             // Empty line (don't care bits)
    Wire.endTransmission();                  // End I2C transmission
}

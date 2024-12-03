# Import the library
from ad569x import AD569x

# Initialize the DAC on I2C bus 1
dac = AD569x(bus_number=1)

# Initialize communication (optional in this case)
dac.begin()

# Set DAC channel A to mid-scale value
dac.set_dac(addr=0, channel=AD569x.CH_A, operation=AD569x.WRITE_UPDATE, value=32768)

# Read back the value from DAC channel A
value = dac.read_dac(addr=0, channel=AD569x.CH_A, operation=AD569x.UPDATE_DAC_WITH_INPUT)
print(f"Read DAC Value: {value}")

# Set power state for all channels
dac.set_power_state(addr=0, A_State=AD569x.NORMAL_OPERATION,
                    B_State=AD569x.ONE_KOHM_TO_GND,
                    C_State=AD569x.HUNDRED_KOHM_TO_GND,
                    D_State=AD569x.THREE_STATE)

# Reset the DAC
dac.reset(addr=0)

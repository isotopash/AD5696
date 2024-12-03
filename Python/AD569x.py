
#    @file     AD569x.py
#    @author   A. Cooper (Isotopash)
#    @license  None (see license.txt)
#
#    This is a library for the AD569X 4-Channel I2C DAC.
#
#    @section I2C Address
#    - DAC_A: 0x08 (A1 = GND, A0 = GND)
#    - DAC_B: 0x09 (A1 = GND, A0 = V)
#    - DAC_C: 0x0A (A1 = V, A0 = GND)
#    - DAC_D: 0x0B (A1 = V, A0 = V)
#
#    The I2C address is 7-bit. The first 5 MSB are `00011`, and the last 2 bits
#    are determined by the A1 and A0 pins.
#
#    @section HISTORY
#    v1.0 - First release
#    V1.0.1 - Code Cleanup 


import smbus

class AD569x:
    # Constants for the base address and operation
    ADDRMSB = 0x08

    # Operations
    NO_OPERATION = 0
    WRITE_INPUT_REGISTER = 1
    UPDATE_DAC_WITH_INPUT = 2
    WRITE_UPDATE = 3

    # Channels
    CH_A = 1
    CH_B = 2
    CH_C = 4
    CH_D = 8
    ALL_CHANNELS = CH_A | CH_B | CH_C | CH_D

    # Power states
    NORMAL_OPERATION = 0
    ONE_KOHM_TO_GND = 1
    HUNDRED_KOHM_TO_GND = 2
    THREE_STATE = 3

    def __init__(self, bus_number=1):
        """
        Initialize the I2C bus.
        :param bus_number: I2C bus number (default: 1 for Raspberry Pi)
        """
        self.bus = smbus.SMBus(bus_number)

    def begin(self):
        """Initialize the DAC. Placeholder for consistency."""
        pass

    def _write_command(self, addr, command, data):
        """
        Writes a command and associated data to the DAC.
        :param addr: Device I2C address.
        :param command: Command byte.
        :param data: List of bytes to write after the command.
        """
        try:
            self.bus.write_i2c_block_data(addr, command, data)
        except Exception as e:
            raise IOError(f"Failed to write to DAC at address {addr:#02X}: {e}")

    def set_dac(self, addr, channel, operation, value):
        """
        Sets the output voltage for the specified channel(s).
        :param addr: DAC address (0-3).
        :param channel: Channel(s) to update (use CH_A, CH_B, CH_C, CH_D, or combinations).
        :param operation: Operation type (e.g., WRITE_INPUT_REGISTER, WRITE_UPDATE).
        :param value: 16-bit DAC value (0-65535).
        """
        command = (operation << 4) | channel
        msb = (value >> 8) & 0xFF
        lsb = value & 0xFF
        self._write_command(self.ADDRMSB + addr, command, [msb, lsb])

    def read_dac(self, addr, channel, operation):
        """
        Reads the output voltage of the DAC.
        :param addr: DAC address (0-3).
        :param channel: Channel(s) to read.
        :param operation: Operation type.
        :return: List of 16-bit values corresponding to the selected channels.
        """
        command = (operation << 4) | channel
        try:
            self.bus.write_byte(self.ADDRMSB + addr, command)
            num_bytes = 2 * self._count_set_bits(channel)
            data = self.bus.read_i2c_block_data(self.ADDRMSB + addr, 0, num_bytes)
            return [(data[i] << 8) | data[i + 1] for i in range(0, len(data), 2)]
        except Exception as e:
            raise IOError(f"Failed to read from DAC at address {addr:#02X}: {e}")

    def set_power_state(self, addr, A_State, B_State, C_State, D_State):
        """
        Configures the power state of the DAC channels.
        :param addr: DAC address (0-3).
        :param A_State, B_State, C_State, D_State: Power state for each channel.
        """
        command = 0b01000000
        power_states = (D_State << 6) | (C_State << 4) | (B_State << 2) | A_State
        self._write_command(self.ADDRMSB + addr, command, [0, power_states])

    def set_ldac_mask(self, addr, channel):
        """
        Configures the LDAC mask register.
        :param addr: DAC address (0-3).
        :param channel: Channels to mask.
        """
        command = 0b01010000
        self._write_command(self.ADDRMSB + addr, command, [0, channel])

    def reset(self, addr):
        """
        Resets the DAC to its power-on state.
        :param addr: DAC address (0-3).
        """
        command = 0b01100000
        self._write_command(self.ADDRMSB + addr, command, [0, 0])

    @staticmethod
    def _count_set_bits(n):
        """Helper to count the number of 1s in the binary representation of an integer."""
        return bin(n).count('1')

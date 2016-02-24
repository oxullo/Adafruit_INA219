Adafruit_INA219
===============

INA219 Current Sensor

## Calibration and dividers

By default we use a pretty huge range for the input voltage,
which probably isn't the most appropriate choice for system
that don't use a lot of power.  But all of the calculations
are shown below if you want to change the settings.  You will
also need to change any relevant register settings, such as
setting the VBUS_MAX to 16V instead of 32V, etc.

Calculation example with:

VBUS_MAX = 32V             (Assumes 32V, can also be set to 16V)
VSHUNT_MAX = 0.32          (Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
RSHUNT = 0.1               (Resistor value in ohms)
MAX_EXPECTED_I = 2         (Our load seeks no more than 2A)

1. Determine max possible current
MaxPossible_I = VSHUNT_MAX / RSHUNT
MaxPossible_I = 3.2A

2. Determine max expected current
MaxExpected_I = MAX_EXPECTED_I

3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
MinimumLSB = MaxExpected_I/32767
MinimumLSB = 0.000061              (61uA per bit)
MaximumLSB = MaxExpected_I/4096
MaximumLSB = 0,000488              (488uA per bit)

4. Choose an LSB between the min and max values
   (Preferrably a roundish number close to MinLSB)
CurrentLSB = 0.0001 (100uA per bit)

5. Compute the calibration register
Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
Cal = 4096 (0x1000)

6. Calculate the power LSB
PowerLSB = 20 * CurrentLSB
PowerLSB = 0.002 (2mW per bit)

7. Compute the maximum current and shunt voltage values before overflow

Max_Current = Current_LSB * 32767
Max_Current = 3.2767A before overflow

If Max_Current > Max_Possible_I then
   Max_Current_Before_Overflow = MaxPossible_I
Else
   Max_Current_Before_Overflow = Max_Current
End If

Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
Max_ShuntVoltage = 0.32V

If Max_ShuntVoltage >= VSHUNT_MAX
   Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
Else
   Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
End If

8. Compute the Maximum Power
MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
MaximumPower = 3.2 * 32V
MaximumPower = 102.4W


## Configuration

Take the calibration value computed above (Cal).

	Adafruit_INA219 ina219;
	
	void setup()
	{
		ina219.begin();
		
		// Sample calibration value computed for 2A over 100mOhm shunt
		ina219.setCalibration(4096);
		
		ina219.setConfiguration(
					INA219_CONFIG_BVOLTAGERANGE_32V |		// Bus voltage range up to 32V
                    INA219_CONFIG_GAIN_8_320MV |			// Lower sensitivity (320mV)
                    INA219_CONFIG_BADCRES_12BIT |			// Bus ADC resolution
                    INA219_CONFIG_SADCRES_12BIT_1S_532US |	// Shunt ADC resolution
                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS);	// Free conversion
		
		// Normalisation factors
		// 1mA/LSB / x uA/LSB (1000uA/LSB / 100uA/LSB = 10)
		// 2mW/LSB / x mW/LSB (2mW/LSB / 1mW LSB = 2)
		ina219.setDividers(10, 2);
	}
  
<!-- START COMPATIBILITY TABLE -->

## Compatibility

MCU               | Tested Works | Doesn't Work | Not Tested  | Notes
----------------- | :----------: | :----------: | :---------: | -----
Atmega328 @ 16MHz |      X       |             |            | 
Atmega328 @ 12MHz |      X       |             |            | 
Atmega32u4 @ 16MHz |      X       |             |            | 
Atmega32u4 @ 8MHz |      X       |             |            | 
ESP8266           |      X       |             |            | 
Atmega2560 @ 16MHz |      X       |             |            | 
ATSAM3X8E         |      X       |             |            | Use D20/D21.
ATSAM21D          |      X       |             |            | 
ATtiny85 @ 16MHz  |      X       |             |            | Use SDA/SCL D0/D2
ATtiny85 @ 8MHz   |      X       |             |            | Use SDA/SCL D0/D2

  * ATmega328 @ 16MHz : Arduino UNO, Adafruit Pro Trinket 5V, Adafruit Metro 328, Adafruit Metro Mini
  * ATmega328 @ 12MHz : Adafruit Pro Trinket 3V
  * ATmega32u4 @ 16MHz : Arduino Leonardo, Arduino Micro, Arduino Yun, Teensy 2.0
  * ATmega32u4 @ 8MHz : Adafruit Flora, Bluefruit Micro
  * ESP8266 : Adafruit Huzzah
  * ATmega2560 @ 16MHz : Arduino Mega
  * ATSAM3X8E : Arduino Due
  * ATSAM21D : Arduino Zero, M0 Pro
  * ATtiny85 @ 16MHz : Adafruit Trinket 5V
  * ATtiny85 @ 8MHz : Adafruit Gemma, Arduino Gemma, Adafruit Trinket 3V

<!-- END COMPATIBILITY TABLE -->

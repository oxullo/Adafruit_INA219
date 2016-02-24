/**************************************************************************/
/*! 
    @file     Adafruit_INA219.cpp
    @author   K.Townsend (Adafruit Industries)
	@license  BSD (see license.txt)
	
	Driver for the INA219 current sensor

	This is a library for the Adafruit INA219 breakout
	----> https://www.adafruit.com/products/???
		
	Adafruit invests time and resources providing this open source code, 
	please support Adafruit and open-source hardware by purchasing 
	products from Adafruit!

	@section  HISTORY

    v1.0 - First release
*/
/**************************************************************************/
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

#include "Adafruit_INA219.h"

/**************************************************************************/
/*! 
    @brief  Sends a single command byte over I2C
*/
/**************************************************************************/
void Adafruit_INA219::wireWriteRegister (uint8_t reg, uint16_t value)
{
  Wire.beginTransmission(ina219_i2caddr);
  #if ARDUINO >= 100
    Wire.write(reg);                       // Register
    Wire.write((value >> 8) & 0xFF);       // Upper 8-bits
    Wire.write(value & 0xFF);              // Lower 8-bits
  #else
    Wire.send(reg);                        // Register
    Wire.send(value >> 8);                 // Upper 8-bits
    Wire.send(value & 0xFF);               // Lower 8-bits
  #endif
  Wire.endTransmission();
}

/**************************************************************************/
/*! 
    @brief  Reads a 16 bit values over I2C
*/
/**************************************************************************/
void Adafruit_INA219::wireReadRegister(uint8_t reg, uint16_t *value)
{

  Wire.beginTransmission(ina219_i2caddr);
  #if ARDUINO >= 100
    Wire.write(reg);                       // Register
  #else
    Wire.send(reg);                        // Register
  #endif
  Wire.endTransmission();
  
  delay(1); // Max 12-bit conversion time is 586us per sample

  Wire.requestFrom(ina219_i2caddr, (uint8_t)2);  
  #if ARDUINO >= 100
    // Shift values to create properly formed integer
    *value = ((Wire.read() << 8) | Wire.read());
  #else
    // Shift values to create properly formed integer
    *value = ((Wire.receive() << 8) | Wire.receive());
  #endif
}

void Adafruit_INA219::setConfiguration(uint16_t configMask)
{
    wireWriteRegister(INA219_REG_CONFIG, configMask);
}

void Adafruit_INA219::setCalibration(uint16_t calValue)
{
    ina219_calValue = calValue;
    wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);
}

void Adafruit_INA219::setDividers(uint32_t curDiv, uint32_t powDiv)
{
    ina219_currentDivider_mA = curDiv;
    ina219_powerDivider_mW = powDiv;
}

/**************************************************************************/
/*! 
    @brief  Instantiates a new INA219 class
*/
/**************************************************************************/
Adafruit_INA219::Adafruit_INA219(uint8_t addr) {
  ina219_i2caddr = addr;
  ina219_currentDivider_mA = 0;
  ina219_powerDivider_mW = 0;
}

/**************************************************************************/
/*! 
    @brief  Setups the HW (defaults to 32V and 2A for calibration values)
*/
/**************************************************************************/
void Adafruit_INA219::begin(uint8_t addr) {
  ina219_i2caddr = addr;
  begin();
}

void Adafruit_INA219::begin(void) {
  Wire.begin();    
  // Set chip to large range config values to start
  setCalibration_32V_2A();
}

/**************************************************************************/
/*! 
    @brief  Gets the raw bus voltage (16-bit signed integer, so +-32767)
*/
/**************************************************************************/
int16_t Adafruit_INA219::getBusVoltage_raw() {
  uint16_t value;
  wireReadRegister(INA219_REG_BUSVOLTAGE, &value);

  // Shift to the right 3 to drop CNVR and OVF and multiply by LSB
  return (int16_t)((value >> 3) * 4);
}

/**************************************************************************/
/*! 
    @brief  Gets the raw shunt voltage (16-bit signed integer, so +-32767)
*/
/**************************************************************************/
int16_t Adafruit_INA219::getShuntVoltage_raw() {
  uint16_t value;
  wireReadRegister(INA219_REG_SHUNTVOLTAGE, &value);
  return (int16_t)value;
}

/**************************************************************************/
/*! 
    @brief  Gets the raw current value (16-bit signed integer, so +-32767)
*/
/**************************************************************************/
int16_t Adafruit_INA219::getCurrent_raw() {
  uint16_t value;

  // Sometimes a sharp load will reset the INA219, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

  // Now we can safely read the CURRENT register!
  wireReadRegister(INA219_REG_CURRENT, &value);
  
  return (int16_t)value;
}

int16_t Adafruit_INA219::getPower_raw() {
  uint16_t value;

  // Sometimes a sharp load will reset the INA219, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

  // Now we can safely read the CURRENT register!
  wireReadRegister(INA219_REG_POWER, &value);
  
  return (int16_t)value;
}
 
/**************************************************************************/
/*! 
    @brief  Gets the shunt voltage in mV (so +-327mV)
*/
/**************************************************************************/
float Adafruit_INA219::getShuntVoltage_mV() {
  int16_t value;
  value = getShuntVoltage_raw();
  return value * 0.01;
}

/**************************************************************************/
/*! 
    @brief  Gets the shunt voltage in volts
*/
/**************************************************************************/
float Adafruit_INA219::getBusVoltage_V() {
  int16_t value = getBusVoltage_raw();
  return value * 0.001;
}

/**************************************************************************/
/*! 
    @brief  Gets the current value in mA, taking into account the
            config settings and current LSB
*/
/**************************************************************************/
float Adafruit_INA219::getCurrent_mA() {
  float valueDec = getCurrent_raw();
  valueDec /= ina219_currentDivider_mA;
  return valueDec;
}

float Adafruit_INA219::getPower_mW() {
  float valueDec = getPower_raw();
  valueDec /= ina219_powerDivider_mW;
  return valueDec;
}

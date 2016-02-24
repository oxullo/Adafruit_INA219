#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

void setup(void) 
{
  #ifndef ESP8266
    while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
  #endif
  uint32_t currentFrequency;
    
  Serial.begin(115200);
  Serial.println("Hello!");
  
  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  ina219.begin();

  // Sample calibration value computed for 2A over 100mOhm shunt
  ina219.setCalibration(4096);

  ina219.setConfiguration(
        INA219_CONFIG_BVOLTAGERANGE_32V |		    // Bus voltage range up to 32V
        INA219_CONFIG_GAIN_8_320MV |			    // Lower sensitivity (320mV)
        INA219_CONFIG_BADCRES_12BIT |			    // Bus ADC resolution
        INA219_CONFIG_SADCRES_12BIT_1S_532US |	    // Shunt ADC resolution
        INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS);	// Free conversion

  // Normalisation factors
  // 1mA/LSB / x uA/LSB (1000uA/LSB / 100uA/LSB = 10)
  // 2mW/LSB / x mW/LSB (2mW/LSB / 1mW LSB = 2)
  ina219.setDividers(10, 2);

  Serial.println("Measuring voltage and current with INA219 ...");
}

void loop(void) 
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float power_mW = 0;
  float loadvoltage = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power  :       "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");

  delay(2000);
}

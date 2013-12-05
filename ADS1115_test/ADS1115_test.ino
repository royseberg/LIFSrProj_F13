#include <Adafruit_ADS1015.h>
#include <i2c_t3.h>

#define MCP4726_CMD_WRITEDAC            (0x40)  // Writes data to the DAC


Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

uint16_t data;
float v_ref = 5.02575;          // Reference Voltage, 5.0 Volt for LT1021 or 3.0 for LP2950-3
const int dac_resolution = 0x0FFF;
const long int adc_resolution = 0x0FFFF;

union 
{
  int16_t i;
  byte b[sizeof(int16_t)]; 
} uAdc;

void setup(void) 
{
  Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("ADS1115 ADC Test");
  // Setup for Master mode, pins 18/19, external pullups, 400kHz
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);  
  data = 0x0;
  
  Serial.println("Hello!");
  
  Serial.println("Getting single-ended readings from AIN0");
  Serial.println("ADC Range: +/- 6.144V (0.1875mV/ADS1115)");
  
  ads.begin();
}

void loop(void) 
{
  uAdc adc0;

  adc0.i = ads.readADC_SingleEnded(0);
  
  Serial.print("Should be: " );
  float expected = v_ref / dac_resolution * data;
  Serial.print(expected, 5);
  Serial.println(" V");

  float adcValue = adc0 * 0.0001875;
  Serial.print("Actual is: ");
  Serial.print(adcValue, 5);
  Serial.println(" V\r\n");
  Serial.println(" ");
  data += 512-1;
  if (data >= 4096) {
    data = 0x0;
  }
  writeValue(data);
  
}


void writeValue(uint16_t output) {
	Wire.beginTransmission(0x62); // slave addr
	Wire.write(MCP4726_CMD_WRITEDAC);
	Wire.write(output / 16);                   // Upper data bits          (D11.D10.D9.D8.D7.D6.D5.D4)
	Wire.write((output % 16) << 4);            // Lower data bits          (D3.D2.D1.D0.x.x.x.x)
        Wire.endTransmission();
}

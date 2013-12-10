#include <Adafruit_ADS1015.h>
#include <i2c_t3.h>

#define MCP4726_CMD_WRITEDAC            (0x40)  // Writes data to the DAC


Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

uint16_t data;
float v_ref = 5.02575;          // Reference Voltage, 5.0 Volt for LT1021 or 3.0 for LP2950-3
const int dac_resolution = 0x0FFF;
const long int adc_resolution = 0x0FFFF;

union UADC
{
  int16_t i;
  byte b[sizeof(int16_t)]; 
};

void setup(void) 
{
  Serial.begin(9600);
  // Setup for Master mode, pins 18/19, external pullups, 400kHz 
  data = 0x1000;
  ads.begin();
  // Setup for Master mode, pins 18/19, external pullups, 400kHz
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_2400);  
  writeValue(data);
}

byte dataArray[] = {0xAA, 0x00, 0x00, 0x55};

void loop(void) 
{
  UADC adc0;

  adc0.i = ads.readADC_SingleEnded(0);
  dataArray[1] = adc0.b[0];
  dataArray[2] = adc0.b[1];
  
  data += 1;
  if (data >= 4095) {
    data = 0x0;
  }
  writeValue(data);
  Serial.write(dataArray, 4);
  
  
}


void writeValue(uint16_t output) {
	Wire.beginTransmission(0x62); // slave addr
	Wire.write(MCP4726_CMD_WRITEDAC);
	Wire.write(output / 16);                   // Upper data bits          (D11.D10.D9.D8.D7.D6.D5.D4)
	Wire.write((output % 16) << 4);            // Lower data bits          (D3.D2.D1.D0.x.x.x.x)
        Wire.endTransmission();
}

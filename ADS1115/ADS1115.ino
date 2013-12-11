#include <Adafruit_ADS1015.h>
#include <i2c_t3.h>
#include <HVRelays.h>

#define MCP4726_CMD_WRITEDAC            (0x40)  // Writes data to the DAC
#define BUFFER_SIZE                     (20)

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

uint16_t data;
float v_ref = 5.02575;          // Reference Voltage, 5.0 Volt for LT1021 or 3.0 for LP2950-3
const int dac_resolution = 0x0FFF;
const long int adc_resolution = 0x0FFFF;

boolean _go = false;
boolean _test = false;

HVRelays relays;

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
  writeValue(0x62, data);
}

byte dataArray[] = {0xAA, 0x00, 0x00, 0x55};

char buffer[BUFFER_SIZE];

int voltage = 0;

void loop(void) 
{
  int numAvailable = Serial.available();
  if (numAvailable > 0) {
     int numToRead = numAvailable > BUFFER_SIZE ? BUFFER_SIZE : numAvailable;
     Serial.readBytes(buffer, numToRead);
     for (int i = 0; i < numToRead; ++i) {
        char next = buffer[i];
        Serial.print(next);
        switch (next) {
          case '1': // 1
            relays.flowBuffer();
            break;
          case '2': // 2
            relays.loadSample();
            break;
          case '3': // 3
            relays.flowSample();
            break;
          case '4': // 4
            relays.allOff();
            break;
          case '5': // 5
            voltage -= 128;
            Serial.println(voltage);
            if (voltage < 0)
              voltage = 0;
            writeValue(0x62, voltage);
            break;
          case '6': // 6
            voltage += 128;
            Serial.println(voltage);
            if (voltage > 4095)
              voltage = 4095;
            writeValue(0x62, voltage);
            break;
          case '7': // 7
            voltage = 1234;
            writeValue(0x62, voltage);
            break;
          case 'm': // 'm'
            printMenu();
            break;
          case 's':
            _go = !_go;
            break;
          case 't':
            _test = !_test;
          default:
            Serial.print('s');
            Serial.println(F("invalid entry"));
            break;
          }
     }
  }
  
  if (_go) {
    sendSamples();
  }
  if (_test) {   
    incrementDac();
  }
}


void printMenu() {
  Serial.print(F("MENU:\n\r1: flow buffer, 2: load sample, 3: flow sample,  4: all relays off\n")); 
  Serial.print(F("5: dec DAC voltage, 6: inc DAC voltage, 7: enter raw DAC value, m: menu\n"));
  Serial.write(0xff);
}

void incrementDac() {
  data += 1;
  if (data >= 4095) {
    data = 0x0;
  }
  writeValue(0x63, data);
}

void sendSamples() {
  UADC adc0;
  adc0.i = ads.readADC_SingleEnded(0);
  dataArray[1] = adc0.b[0];
  dataArray[2] = adc0.b[1];
  Serial.write(dataArray, 4);
}

void writeValue(int address, uint16_t output) {
	Wire.beginTransmission(address); // slave addr
	Wire.write(MCP4726_CMD_WRITEDAC);
	Wire.write(output / 16);                   // Upper data bits          (D11.D10.D9.D8.D7.D6.D5.D4)
	Wire.write((output % 16) << 4);            // Lower data bits          (D3.D2.D1.D0.x.x.x.x)
        Wire.endTransmission();
}

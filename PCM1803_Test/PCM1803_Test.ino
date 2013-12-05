

#include <SPI.h>
#include <i2c_t3.h>
#include "mk20dx128.h"
#include "core_pins.h"
#include "arduino.h"
#include "t3spi.h"

#define LRCK     (10)

const uint32_t dac_resolution = 0x0FFF;
const uint32_t adc_resolution = 0x0FFFFFF;



float v_ref=5;          // Reference Voltage, 5.0 Volt for LT1021 or 3.0 for LP2950-3

uint16_t cnt = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(LRCK, OUTPUT);

  // Setup for Master mode, pins 18/19, external pullups, 400kHz
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);
  writeValue(cnt);
  
  // corresponding to SCK pin and DRDY/DOUT pin on ADC
  
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV4);    // ~1.211  MHz     2.202 MHz      2.351 MHz  <-- Default Teensy 3 SPI ?
  
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("PCM1803 ADC Test");

  cnt = 4000;
  writeValue(cnt);
}

void loop()
{
  read_adc();
  // "sort of" an interrupt to go to read_adc routine;
  // can use hardware interrupt in future but now just poll
}

void read_adc()
{
  digitalWriteFast(LRCK, HIGH);
  byte dummy = SPI.transfer(0x00) | SPI.transfer(0x00) | SPI.transfer(0x00);
  dummy = SPI.transfer(0x00) | SPI.transfer(0x00) | SPI.transfer(0x00);
  // go to drdy_wait routine, where we wait for
  // DRDY phase to pass, and thus for DOUT phase to begin
  digitalWriteFast(LRCK, LOW);
  byte msb = SPI.transfer(0x00); //load msb first 
  byte csb = SPI.transfer(0x00); //load second 8 bit segment
  byte lsb = SPI.transfer(0x00); //load lsb last
  
  dummy = SPI.transfer(0x00) | SPI.transfer(0x00) | SPI.transfer(0x00);

  uint32_t data = ((msb & 0xff) << 16) | ((csb & 0xff) << 8) | (lsb & 0xff);
  // read in adc data (sending out don't care bytes)
  // and store read data into three bytes */
  
  Serial.print("Should be: " );
  float expected = v_ref / 4096 * cnt;
  Serial.print(expected, 5);
  Serial.println(" V");
  
  float adcValue = v_ref / adc_resolution * data;
  Serial.print("Actual is: ");
  Serial.print(adcValue, 5);
  Serial.println(" V\r\n");
//  cnt += 512-1;
//  if (cnt >= 4096) {
//      cnt = 0x0;
//  }
//  writeValue(cnt);
  delay(1000);
}


void writeValue(uint16_t output) {
	Wire.beginTransmission(0x62); // slave addr
	Wire.write(0x40);
	Wire.write(output / 16);                   // Upper data bits          (D11.D10.D9.D8.D7.D6.D5.D4)
	Wire.write((output % 16) << 4);            // Lower data bits          (D3.D2.D1.D0.x.x.x.x)
        Wire.endTransmission();
}

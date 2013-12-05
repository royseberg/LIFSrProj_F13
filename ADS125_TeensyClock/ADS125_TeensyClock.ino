

#include <SPI.h>
#include <i2c_t3.h>

#define MISOPIN 10
#define SCLKPIN 13

#define CLOCK_RATE (2000000)

const uint32_t dac_resolution = 0x0FFF;
const uint32_t adc_resolution = 0x0FFFFFF;

float usClock = 1000000.0 / CLOCK_RATE;

uint32_t t1 = 6 * usClock;
uint32_t t2 = 6 * usClock;
uint32_t t3 = 6 * usClock;
uint32_t wait = t2 + t3;
uint32_t tdrdy = 384 * usClock;
uint32_t drdy = 36 * usClock;
uint32_t dout = 328 * usClock;
float t14 = 2042.5 * usClock;

float v_ref=3.3;          // Reference Voltage, 5.0 Volt for LT1021 or 3.0 for LP2950-3

uint16_t cnt = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(3, OUTPUT);
  analogWriteResolution(5);
  analogWriteFrequency(3, CLOCK_RATE);
  analogWrite(3, 17);

	// Setup for Master mode, pins 18/19, external pullups, 400kHz
	Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);
  writeValue(cnt);
  
  pinMode(MISOPIN, INPUT);
  // corresponding to SCK pin and DRDY/DOUT pin on ADC
  
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV4);    // ~1.211  MHz     2.202 MHz      2.351 MHz  <-- Default Teensy 3 SPI ?
  // initialize SPI (with default settings, including...
  // CPOL = 0: so that SCLK is normally LOW
  // CPHA = 0: data sampled on rising edge (LOW to HIGH)
  // perhaps try changing CPHA ??
  
  
	while (!Serial) {
		; // wait for serial port to connect. Needed for Leonardo only
	}

	Serial.println("LTC2400 ADC Test");

	cnt = 0x0;
  
//  reset_adc();
  // put ADC on reset at the outset
  
}

void loop()
{
  if (digitalReadFast(MISOPIN) == HIGH) read_adc();
  // "sort of" an interrupt to go to read_adc routine;
  // can use hardware interrupt in future but now just poll
}

void reset_adc()
// to reset ADC, we need SCLK HIGH for min of 4 CONVCYCLES
// so here, hold SCLK HIGH for 5 CONVCYCLEs = 1440 usec
{
  SPI.setDataMode(SPI_MODE1);
  delay(tdrdy * 4);
  SPI.setDataMode(SPI_MODE0);
}

void read_adc()
{
  drdy_wait();
  // go to drdy_wait routine, where we wait for
  // DRDY phase to pass, and thus for DOUT phase to begin

  uint32_t data = 0xff &  SPI.transfer(0x00);
  data = (data << 8) | (SPI.transfer(0x00) & 0xff);
  data = (data << 8) | (SPI.transfer(0x00) & 0xff);
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
  cnt += 512-1;
  if (cnt >= 4096) {
      cnt = 0x0;
  }
  writeValue(cnt);
  delay(1000);
}

void drdy_wait()
// wait for DRDY to pass and to reach start-point of DOUT
{
  delayMicroseconds(wait);
  // to be safe, 30 usec, instead of 27 usec, which is
  // the expected period of DRDY phase
}

void writeValue(uint16_t output) {
	Wire.beginTransmission(0x62); // slave addr
	Wire.write(0x40);
	Wire.write(output / 16);                   // Upper data bits          (D11.D10.D9.D8.D7.D6.D5.D4)
	Wire.write((output % 16) << 4);            // Lower data bits          (D3.D2.D1.D0.x.x.x.x)
        Wire.endTransmission();
}

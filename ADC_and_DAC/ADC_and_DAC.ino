//LTC2400 24bit ADC Module 
//
//Application Demo: 7 digit voltmeter 
//24bit ADC IC: LTC2400
//4.096 precision reference: TI REF3040
//
//By coldtears electronics
//
//LTC2400 code is adapted from Martin Nawrath
//Kunsthochschule fuer Medien Koeln
//Academy of Media Arts Cologne
//


#include <SPI.h>
#include <i2c_t3.h>

// Command definitions
#define MCP4726_CMD_WRITEDAC            (0x40)  // Writes data to the DAC

#define LTC_CS 10         // LTC2400 Chip Select Pin  on Portb 2
#define LTC_MISO  14      // LTC2400 SDO Select Pin  on Portb 4
#define LTC_SCK  13       // LTC2400 SCK Select Pin  on Portb 5

uint16_t data;

void setup() {


	Serial.begin(38400);

	// init SPI Hardware
	SPI.begin(); 

	SPI.setClockDivider(SPI_CLOCK_DIV8);    //   ~754  KHz     1.420 MHz      1.516 MHz

	pinMode(LTC_MISO, INPUT);
	pinMode(LTC_CS, OUTPUT);
	
	while (!Serial) {
		; // wait for serial port to connect. Needed for Leonardo only
	}

	Serial.println("LTC2400 ADC Test");


	// Setup for Master mode, pins 18/19, external pullups, 400kHz
	Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);
	data = 0x0;

}
float volt;
float v_ref=3.3;          // Reference Voltage, 5.0 Volt for LT1021 or 3.0 for LP2950-3
const int dac_resolution = 0x0FFF;
const int adc_resolution = 0x0FFFFFF;

long int ltw = 0;         // ADC Data ling int
int cnt;                  // counter
byte b0, b1, b2, b3;                  //
byte sig;                 // sign bit flag
char st1[20];             // float voltage text

/********************************************************************/
void loop() {

	digitalWriteFast(LTC_CS, LOW);             // LTC2400 CS Low
	delayMicroseconds(1);
	if (!(digitalReadFast(LTC_MISO))) {    // ADC Converter ready ?
		//    cli();
		ltw=0;
		sig=0;
		//delayMicroseconds(1);

		b0 = SPI.transfer(-1);             // read 4 bytes adc raw data with SPI
		if ((b0 & 0x20) ==0) sig=1;  // is input negative ?
		//b0 &=0x1F;                   // discard bit 25..31
		ltw |= (b0 & 0x1F);
		ltw <<= 8;
		b1 = SPI.transfer(-1);
		ltw |= b1;
		ltw <<= 8;
		b2 = SPI.transfer(-1);
		ltw |= b2;
		ltw <<= 8;
		b3 = SPI.transfer(-1);
		ltw |= b3;

		//delayMicroseconds(1);

		digitalWriteFast(LTC_CS, HIGH);           // LTC2400 CS Low


                Serial.print("Should be: " );
                float expected = v_ref / 4096 * data;
                Serial.print(expected, 5);
                Serial.println(" V");

		if (sig) ltw |= 0xf0000000;    // if input negative insert sign bit
                ltw = ltw >> 4;
                float adcValue = v_ref / adc_resolution * ltw;
                Serial.print("Actual is: ");
                Serial.print(adcValue, 5);
                Serial.println(" V\r\n");
                
                data += 512-1;
		if (data >= 4096) {
			data = 0x0;
		}
		writeValue(data);
                delay(1000);
	}

	digitalWriteFast(LTC_CS, HIGH);           // LTC2400 CS Low
	delayMicroseconds(10);

}


void writeValue(uint16_t output) {
	Wire.beginTransmission(0x62); // slave addr
	Wire.write(MCP4726_CMD_WRITEDAC);
	Wire.write(output / 16);                   // Upper data bits          (D11.D10.D9.D8.D7.D6.D5.D4)
	Wire.write((output % 16) << 4);            // Lower data bits          (D3.D2.D1.D0.x.x.x.x)
        Wire.endTransmission();
}


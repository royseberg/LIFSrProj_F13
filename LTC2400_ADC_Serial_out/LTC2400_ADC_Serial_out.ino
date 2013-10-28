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


#define LTC_CS 10         // LTC2400 Chip Select Pin  on Portb 2
#define LTC_MISO  14      // LTC2400 SDO Select Pin  on Portb 4
#define LTC_SCK  13       // LTC2400 SCK Select Pin  on Portb 5


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

}
float volt;
float v_ref=4.094;          // Reference Voltage, 5.0 Volt for LT1021 or 3.0 for LP2950-3

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

   //digitalWriteFast(LTC_CS, HIGH);           // LTC2400 CS Low
   
   if (sig) ltw |= 0xf0000000;    // if input negative insert sign bit
   ltw=ltw/16;                    // scale result down , last 4 bits have no information
   volt = ltw * v_ref / 16777216; // max scale
   char tmp[10];
   dtostrf(volt,6,6,tmp);
   tmp[8]='V';
   tmp[9]='\n';
  Serial.print(cnt++);
  Serial.print(";  ");
  printFloat(volt,6);           // print voltage as floating number
  Serial.println("  ");
 }

   //digitalWriteFast(LTC_CS, HIGH);           // LTC2400 CS Low
  delayMicroseconds(10);

}
/********************************************************************/

/********************************************************************/
//  printFloat from  tim / Arduino: Playground
// printFloat prints out the float 'value' rounded to 'places' places
//after the decimal point
void printFloat(float value, int places) {
 // this is used to cast digits
 int digit;
 float tens = 0.1;
 int tenscount = 0;
 int i;
 float tempfloat = value;

 // if value is negative, set tempfloat to the abs value

   // make sure we round properly. this could use pow from
 //<math.h>, but doesn't seem worth the import
 // if this rounding step isn't here, the value  54.321 prints as

 // calculate rounding term d:   0.5/pow(10,places)
 float d = 0.5;
 if (value < 0)
   d *= -1.0;
 // divide by ten for each decimal place
 for (i = 0; i < places; i++)
   d/= 10.0;
 // this small addition, combined with truncation will round our

 tempfloat +=  d;

 if (value < 0)
   tempfloat *= -1.0;
 while ((tens * 10.0) <= tempfloat) {
   tens *= 10.0;
   tenscount += 1;
 }

 // write out the negative if needed
 if (value < 0)
   Serial.print('-');

 if (tenscount == 0)
   Serial.print(0, DEC);

 for (i=0; i< tenscount; i++) {
   digit = (int) (tempfloat/tens);
   Serial.print(digit, DEC);
   tempfloat = tempfloat - ((float)digit * tens);
   tens /= 10.0;
 }

 // if no places after decimal, stop now and return
 if (places <= 0)
   return;

 // otherwise, write the point and continue on
 Serial.print(',');

 for (i = 0; i < places; i++) {
   tempfloat *= 10.0;
   digit = (int) tempfloat;
   Serial.print(digit,DEC);
   // once written, subtract off that digit
   tempfloat = tempfloat - (float) digit;
 }
}


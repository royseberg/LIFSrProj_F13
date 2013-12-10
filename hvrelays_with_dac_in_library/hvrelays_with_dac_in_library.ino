/*
*  hvrelay_test_1
*  Uses the HVRelays library to control relays,
*  Also uses Wire library for the I2C communication with the DAC
*
*  Daniel Andrews
*
*  Most Current Control Program - written 12/9/13
*/

#include <HVRelays.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>

HVRelays relays;

void setup() {
  //DAC is done in the relay library (HVRelays.cpp setup() function)
  
  //digitalWrite(13, 1);
  Serial.begin(9600);
  delay(3000);
  printMenu();
}

void printMenu() {
  Serial.print(F("MENU:\n\r1: flow buffer, 2: load sample, 3: flow sample,  4: all relays off\n\r")); 
  Serial.print(F("5: dec DAC voltage, 6: inc DAC voltage, 7: enter raw DAC value, m: menu\n\r"));
}

int incomingByte = 0;
int voltage = 0;

void loop() {
 
  while (Serial.available() == 0) {}

  incomingByte = Serial.read();
  
  switch (incomingByte) {
  case 49: // 1
    Serial.println(F("Flow Buffer..."));
    relays.flowBuffer();
    break;
  case 50: // 2
    Serial.println(F("Load Sample..."));
    relays.loadSample();
    break;
  case 51: // 3
    Serial.println(F("Flow Sample..."));
    relays.flowSample();
    break;
  case 52: // 4
    Serial.println(F("All Relays Turned Off"));
    relays.allOff();
    break;
  case 53: // 5
    voltage -= 128;
    if (voltage < 0)
      voltage = 0;
    Serial.print(F("DAC value: "));
    Serial.println(voltage);
    relays.setHV(voltage);
    break;
  case 54: // 6
    voltage += 128;
    if (voltage > 4095)
      voltage = 4095;
    Serial.print(F("DAC value: "));
    Serial.println(voltage);
    relays.setHV(voltage);
    break;
  case 55: // 7
    voltage = 1234;
    relays.setHV(voltage);
    Serial.println(F("DAC set to 1234"));
    break;
  case 109: // 'm'
    printMenu();
    break;
  default:
    Serial.println(F("invalid entry"));
    break;
  }
 
}

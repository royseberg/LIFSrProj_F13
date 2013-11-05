// -------------------------------------------------------------------------------------------
// Teensy3 I2C Master Test
// 08Mar13 Brian (nox771 at gmail.com)
// -------------------------------------------------------------------------------------------
//
// This creates an I2C master device which talks to the simple I2C slave device given in the
// i2c_slave_t3 sketch.
//
// This code assumes the slave config has 256byte memory and I2C addr is 0x40.
// The various tests are started by pulling one of the control pins low.
//
// This example code is in the public domain.
//
// -------------------------------------------------------------------------------------------
// Slave protocol is as follows:
// -------------------------------------------------------------------------------------------
// WRITE - The I2C Master can write to the device by transmitting the WRITE command,
//         a memory address to store to, and a sequence of data to store.
//         The command sequence is:
//
// START|I2CADDR+W|WRITE|MEMADDR|DATA0|DATA1|DATA2|...|STOP
//
// where START     = I2C START sequence
//       I2CADDR+W = I2C Slave address + I2C write flag
//       WRITE     = WRITE command
//       MEMADDR   = memory address to store data to
//       DATAx     = data byte to store, multiple bytes are stored to increasing address
//       STOP      = I2C STOP sequence
// -------------------------------------------------------------------------------------------
// READ - The I2C Master can read data from the device by transmitting the READ command,
//        a memory address to read from, and then issuing a STOP/START or Repeated-START,
//        followed by reading the data.  The command sequence is:
//
// START|I2CADDR+W|READ|MEMADDR|REPSTART|I2CADDR+R|DATA0|DATA1|DATA2|...|STOP
//
// where START     = I2C START sequence
//       I2CADDR+W = I2C Slave address + I2C write flag
//       READ      = READ command
//       MEMADDR   = memory address to read data from
//       REPSTART  = I2C Repeated-START sequence (or STOP/START if single Master)
//       I2CADDR+R = I2C Slave address + I2C read flag
//       DATAx     = data byte read by Master, multiple bytes are read from increasing address
//       STOP      = I2C STOP sequence
// -------------------------------------------------------------------------------------------

#include <i2c_t3.h>
#ifdef I2C_DEBUG
    #include <rbuf.h> // linker fix
#endif

// Command definitions
#define MCP4726_CMD_WRITEDAC            (0x40)  // Writes data to the DAC

// Function prototypes
void print_i2c_status(void);

uint16_t data;
void setup()
{
    pinMode(13,OUTPUT);       // LED
    Serial.begin(115200);

    // Setup for Master mode, pins 18/19, external pullups, 400kHz
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);
    data = 0x0;
}


void loop()
{

    // The LED is turned on during I2C operations.  If it gets stuck on then the
    // ISR likely had a problem.  This can happen with excessive baud rate.
    //
    
    if (data) {
       data = 0x0;
    }
    else {
       data = 0xFFFF;
    }
    
    Serial.print("---------------------------------------------------\r\n");
    Serial.print("I2C WRITE upper: ");
    Serial.print(data / 16);
    Serial.print("\r\n");
    Serial.print("I2C WRITE lower: ");
    Serial.print((data % 16) << 4);
    Serial.print("\r\n");
    
    Serial.print("Writing: ");
    
    Wire.beginTransmission(0x62); // slave addr
    writeValue(data);
    Serial.print(" ");
    digitalWrite(13,HIGH);          // LED on
    Wire.endTransmission();         // blocking I2C Tx (when not specified I2C_STOP is implicit)
    digitalWrite(13,LOW);           // LED off
    
    Serial.print("\r\n");
    print_i2c_status();             // print I2C final status
    
    
    delay(2000); // delay to space out tests
}

void writeValue(uint16_t output) {
  Wire.write(MCP4726_CMD_WRITEDAC);
  
  Wire.write(output / 16);                   // Upper data bits          (D11.D10.D9.D8.D7.D6.D5.D4)
  Wire.write((output % 16) << 4);            // Lower data bits          (D3.D2.D1.D0.x.x.x.x)
}

//
// print I2C status
//
void print_i2c_status(void)
{
    switch(Wire.status())
    {
    case I2C_WAITING:  Serial.print("I2C waiting, no errors\r\n"); break;
    case I2C_ADDR_NAK: Serial.print("Slave addr not acknowledged\r\n"); break;
    case I2C_DATA_NAK: Serial.print("Slave data not acknowledged\r\n"); break;
    case I2C_ARB_LOST: Serial.print("Bus Error: Arbitration Lost\r\n"); break;
    default:           Serial.print("I2C busy\r\n"); break;
    }
}

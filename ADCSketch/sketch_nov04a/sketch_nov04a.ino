

#include <i2c_t3.h>

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
    //digitalWrite(13,HIGH);          // LED on
    //Wire.endTransmission();         // blocking I2C Tx (when not specified I2C_STOP is implicit)
    //digitalWrite(13,LOW);           // LED off
    
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




uint32_t sample = 0;

void setup(){
    Serial.begin(11520);
    // Use the pin to clock the duration of Serial communication.
    pinMode(13,OUTPUT);       // LED
    while (!Serial) {
	; // wait for serial port to connect. Needed for Leonardo only
    }
}



void loop() {
    for (uint32_t i=0; ; ++i) {
   digitalWriteFast(13, HIGH); // Start the transfer
       Serial.println(i, HEX);
   digitalWriteFast(13, LOW); // End the transfer
   
    }
}

void printHex(int value) {
   digitalWriteFast(13, HIGH); // Start the transfer
       Serial.println(value, HEX);
   digitalWriteFast(13, LOW); // End the transfer
}

void sendData(byte *bytes) {
   digitalWriteFast(13, HIGH); // Start the transfer
   Serial.write(bytes, sizeof(uint32_t)); // Transfer the array
   digitalWriteFast(13, LOW); // End the transfer
}

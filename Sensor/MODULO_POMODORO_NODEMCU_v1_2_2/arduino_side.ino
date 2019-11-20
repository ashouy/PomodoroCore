#include <Wire.h>

void setup() {
 Wire.begin(8);                /* join i2c bus with address 8 */
 Wire.onReceive(receiveEvent); /* register receive event */
 Wire.onRequest(requestEvent); /* register request event */
 Serial.begin(9600);           /* start serial for debug */
}

void loop() {
 delay(100);
}

// function that executes whenever data is received from master
void receiveEvent(int howMany) {
 while (0 <Wire.available()) {
    char c = Wire.read();      /* receive byte as a character */
    Serial.print(c);           /* print the character */
  }
 Serial.println();             /* to newline */
}
void FazLeituraCanaisADC(void){}

// function that executes whenever data is requested from master
void requestEvent() {
 uint16_t valor = analogRead(A0);
 Wire.write((byte*)&valor, 2);  /*send string on request */
}

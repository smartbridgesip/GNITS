#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7,8); // CE, CSN
const byte address[6] = "RSV76";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  pinMode(4, OUTPUT);
}
void loop() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
    delay(1000);
    
  }
  digitalWrite(2, HIGH);
}

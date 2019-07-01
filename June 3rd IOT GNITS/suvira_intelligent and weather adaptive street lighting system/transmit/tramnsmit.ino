#include <ESP8266WiFi.h>--------
#include <PubSubClient.h>
 #include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
String command;
RF24 radio(D4,D8); // CE, CSN

const byte address[6] = "RSV76";

 void callback(char* topic, byte* payload, unsigned int payloadLength);
//-------- Customise these values -----------
const char* ssid = "Bala";
const char* password = "suma1899";
#include "DHT.h"
#define DHTPIN D2    // what pin we're connected to
#define DHTTYPE DHT11   // define type of sensor DHT 11
DHT dht (DHTPIN, DHTTYPE);
 
#define ORG "pgx60q"
#define DEVICE_TYPE "vineela"
#define DEVICE_ID "1229"
#define TOKEN "123456789"
//-------- Customise the above values --------
 
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/Data/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
 
WiFiClient wifiClient;
PubSubClient client(server, 1883,wifiClient);

void setup() {
 Serial.begin(115200);
 Serial.println();
 dht.begin();
 Serial.print("Connecting to ");
 Serial.print(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 } 
 Serial.println("");
 
 Serial.print("WiFi connected, IP address: ");
 Serial.println(WiFi.localIP());
 radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening(); //disabling receiving mode
  Serial.begin(9600);
  pinMode(D0,OUTPUT);
}
 
void loop() {
int ldrvalue=analogRead(A0);
PublishData(ldrvalue);
radio.write(&ldrvalue, sizeof(ldrvalue));
  if(ldrvalue<200)
{
  digitalWrite(D0,LOW);
  }
else
digitalWrite(D0,HIGH);
delay(1000);

}

void PublishData(int ldrvalue){
 if (!!!client.connected()) {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
  String payload = "{\"d\":{\"ldrvalue\":";
  payload += ldrvalue;
  
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(topic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
}

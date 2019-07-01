#include <ESP8266WiFi.h>
#include <PubSubClient.h>
int value,fdata;
const char* ssid = "Sanjana Karra";
const char* password = "nandu624";

#define ORG "qfbvzt"
#define DEVICE_TYPE "karra"
#define DEVICE_ID "6919"
#define TOKEN "1234567890"

String command;
String flag;
String area;
String both;
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/cmd/home/fmt/String";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
void callback(char* topic, byte* payload, unsigned int payloadLength);
//Serial.println(clientID);
#define LED D2
#define OP D3
String command1;
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);
void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(LED,OUTPUT);
  pinMode(OP,INPUT);
  wifiConnect();
  mqttConnect();
}

void loop() {

  
    flag=command;
    
if(flag=="lighton" ||fdata==1){
value=analogRead(A0);

  if(value>=900){
  int OP=digitalRead(OP);
  value=(value/20);
  analogWrite(LED,value);
  if(OP==1){
    digitalWrite(LED,HIGH);
    
  }
  }else
  digitalWrite(LED,LOW);
 
}
if(flag=="lightoff"||fdata==0)
digitalWrite(LED,LOW);

  if (!client.loop()) {
    
    mqttConnect();
  }
delay(100);

}

void wifiConnect() {
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("nWiFi connected, IP address: "); Serial.println(WiFi.localIP());
}

void mqttConnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    initManagedDevice();
    Serial.println();
  }
}
void initManagedDevice() {
  if (client.subscribe(topic)) {
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
Serial.print("callback invoked for topic: "); Serial.println(topic);
int value=analogRead(A0);
  Serial.print("the resistor value is");
  Serial.println(value);
 // digitalWrite(LED,value);
  for (int i = 0; i < payloadLength; i++) {
    //Serial.println((char)payload[i]);
    command1 += (char)payload[i];
  }
loop();
command=getValue(command1,'$',0);
area=getValue(command1,'$',1);
if(area=="area2"||"both"){
Serial.println(command);
Serial.println(area);
if(command=="lighton"){
  fdata=1;
}
else if(command=="lightoff"){
  fdata=0;
}
}
command="";
area="";
command1="";
Serial.println(area);
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

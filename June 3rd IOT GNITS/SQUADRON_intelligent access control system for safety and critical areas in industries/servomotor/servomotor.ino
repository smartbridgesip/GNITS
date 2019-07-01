#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
const char* ssid = "GUEST";
const char* password = "12345678";

#define ORG "gfzlqp"
#define DEVICE_TYPE "GNITS"
#define DEVICE_ID "1006"
#define TOKEN "987654321"
String command;
Servo myservo;
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/cmd/home/fmt/String";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
//Serial.println(clientID);
void callback(char* topic, byte* payload, unsigned int payloadLength);

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);
void setup() 
{
  Serial.begin(115200);
  Serial.println();
  myservo.attach(D2);
  pinMode(D1,OUTPUT);
  wifiConnect();
  mqttConnect();
}

void loop()
{
  if (!client.loop()) 
  {
    mqttConnect();
  }
delay(100);
}

void wifiConnect() 
{
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("nWiFi connected, IP address: "); Serial.println(WiFi.localIP());
}

void mqttConnect() 
{
  if (!client.connected()) 
  {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) 
    {
      Serial.print(".");
      delay(500);
    }
    initManagedDevice();
    Serial.println();
  }
}
void initManagedDevice() 
{
  if (client.subscribe(topic)) 
  {
    Serial.println("subscribe to cmd OK");
  } 
  else 
  {
    Serial.println("subscribe to cmd FAILED");
  }
}

void callback(char* topic, byte* payload, unsigned int payloadLength) 
{
  Serial.print("callback invoked for topic: "); Serial.println(topic);

  for (int i = 0; i < payloadLength; i++) 
  {
    //Serial.println((char)payload[i]);
    command += (char)payload[i];
  }
Serial.println(command);
if(command == "dooropen")//command given in node red
 {
  //digitalWrite(D1,HIGH);
  myservo.write(135);
  delay(1000);
  myservo.write(0);
  Serial.println("door is open");
 }
else if(command == "doorclose")
 {
  //digitalWrite(D1,LOW);
  myservo.write(0);
  Serial.println("door is closed");
 }
command ="";// after giving command we are emptying it/ erasing the memory basically
}

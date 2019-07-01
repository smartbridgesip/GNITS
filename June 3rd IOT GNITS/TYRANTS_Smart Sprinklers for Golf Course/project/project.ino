#include <ESP8266WiFi.h>
#include <PubSubClient.h>
const int sensor_pin=A0;
void callback(char* topic, byte* payload, unsigned int payloadLength);

 
const char* ssid = "GUEST";
const char* password = "12345678";

#define ORG "0ui3ae"
#define DEVICE_TYPE "shreya"
#define DEVICE_ID "1234"
#define TOKEN "1234567890"

 float moist;
String data3;
String data="";

const char publishTopic[] = "iot-2/evt/robo/fmt/json";
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/cmd/home/fmt/String";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
//Serial.println(clientID);
void callback(char* topic, byte* payload, unsigned int payloadLength);

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

int publishInterval = 5000; // 30 seconds
long lastPublishMillis;
void publishData();

void setup()
{
  Serial.begin(9600);
   pinMode(D2,OUTPUT);
   digitalWrite(D2,LOW);
  wifiConnect();
  mqttConnect();
}

void loop() 
{
 
  moist=(100.00-((analogRead(sensor_pin)/1023.00)*100));
  Serial.print("Soil Moisture(in percentage)=");
  Serial.print(moist);
  Serial.println("%");
  delay(1000);
  if (millis() - lastPublishMillis > publishInterval)
  {
    publishData();
    lastPublishMillis = millis();
  }
 if (!client.loop()) {
    mqttConnect();
  }
}

void wifiConnect() {
  Serial.print("Connecting to "); 
  Serial.print(ssid);
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
void publishData() 
{
  
  String payload = "{\"d\":{\"moist\":";
  payload += moist;
  payload += "}}";


  Serial.print("\n");
  Serial.print("Sending payload: "); Serial.println(payload);

  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish OK");
  } else {
    Serial.println("Publish FAILED");
  }
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
  
  Serial.print("callback invoked for topic: ");
  Serial.println(topic);

  for (int i = 0; i < payloadLength; i++) {
    //Serial.print((char)payload[i]);
    data3 += (char)payload[i];
  }
  
  Serial.print("data: "+ data3);
  control_func();
  data3 = "";
}

void control_func()
{
  
   
  if(data3 == "spon")
 {
   if(moist<65)
   { 
    digitalWrite(D2,HIGH); 
    delay(1000);
    Serial.println(".......SPRINKLER ON..........");

   }
   else
   { 
    digitalWrite(D2,LOW); 
    delay(1000);
    Serial.println(".......SPRINKLER OFF..........");

   }
 }
  else if(data3 == "spoff")
  {
    digitalWrite(D2,LOW);

    delay(1000);
    Serial.println(".......SPRINKLER OFF..........");

  }

  else
  {
    Serial.println(".....NO COMMAND INPUT..........");
    }

}

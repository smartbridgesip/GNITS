#include <ESP8266WiFi.h>
#include <PubSubClient.h>

void callback(char* topic, byte* payload, unsigned int payloadLength);

// CHANGE TO YOUR WIFI CREDENTIALS
const char* ssid = "iPhone";
const char* password = "password5600";

#define ORG "bj4nz4"
#define DEVICE_TYPE "Project"
#define DEVICE_ID "5600"
#define TOKEN "aneesharao13"

const char publishTopic[] = "iot-2/evt/robo/fmt/json";
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/cmd/test/fmt/String";// cmd  REPRESENT command type AND COMMAND IS TEST OF FORMAT STRING
char topic1[]="iot-2/cmd/auto/fmt/String";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

const char* host = "api.msg91.com";

#include "DHT.h"  
#define DHTPIN D3   
#define DHTTYPE DHT11   
DHT dht (DHTPIN, DHTTYPE);
float temperature;
int humidity;
int soil;
int moisture;
String data3;
int light;
String data="";
boolean flag;
#define LED D4
#define SM D5
#define SPRINKLER D6

long publishInterval = 15000;
long lastPublishMillis;
void publishData();

long notificationInterval=600000;
long lastNotificationHumidity;
long lastNotificationTemperature;

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);


void setup() {
  // put your setup code here, to run once:
  wifiConnect();
  mqttConnect();
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(SPRINKLER, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
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
   // Serial.println(client.subscribe(topic));
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
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


  if(data3=="lton")
    digitalWrite(LED, HIGH);
    flag=0;
  if(data3=="ltoff")
    digitalWrite(LED, LOW);
    flag=0;
  if(data3=="ltauto")
    analogWrite(LED, analogRead(A0));
    flag=1;
  if(data3=="mon")
    digitalWrite(SPRINKLER, HIGH);
  if(data3=="moff")
    digitalWrite(SPRINKLER, LOW);
  if(data3=="mauto")
  {
    if(moisture==0)
    {
      digitalWrite(SPRINKLER, LOW);
    }
    if(moisture==1)
    {
      digitalWrite(SPRINKLER, HIGH);
    }
  }
  if(flag==1&&data3="ltofft")
   digitalWrite(LED, HIGH);
  if(flag==1&&data3="ltautot")
   analogWrite(LED, analogRead(A0));
   
  data3 = "";
}

void publishData() 
{
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  moisture=digitalRead(SM);
  light=map(analogRead(A0), 0, 1023, 0, 100);

  if(moisture==0)
   soil=1;
  else
   soil=0;

   if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  String payload = "{\"d\":{\"temperature\":";
  payload += temperature;
  payload += ",""\"humidity\":";
  payload +=  humidity;
  payload += ",""\"soilMoisture\":";
  payload +=  soil;
  payload += ",""\"avaliableLightPercentage\":";
  payload +=  light;
  payload += "}}";

  if(humidity>40&&(millis() - lastNotificationHumidity > notificationInterval))
  {
    WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/api/sendhttp.php?route=4&sender=TESTIN&mobiles=7093308895&authkey=282040Ad6VPGNU5d0c5cd6&message=Humidity very high, check Greenhouse&country=91";
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  }

  if(temperature>40&&(millis() - lastNotificationTemperature > notificationInterval))
  {
     WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/api/sendhttp.php?route=4&sender=TESTIN&mobiles=7093308895&authkey=282040Ad6VPGNU5d0c5cd6&message=Temperature very high, check Greenhouse&country=91";
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  }
  


  Serial.print("\n");
  Serial.print("Sending payload: "); Serial.println(payload);

  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish OK");
  } else {
    Serial.println("Publish FAILED");
  }
}

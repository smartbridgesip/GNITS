#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include<SoftwareSerial.h>
#include <PubSubClient.h>
#include<Servo.h>

#define sw D1
#define ORG "om097i"
#define DEVICE_TYPE "ramya"
#define DEVICE_ID "4444"
#define TOKEN "9550044733"

String msg91_data;
String data,url,line;
double latitude, longitude;
int l; 
String command;

void mqttConnect();
void publishData(double lati,double longi);
void callback(char* topic, byte* payload, unsigned int payloadLength);
const char publishTopic[] = "iot-2/evt/theft/fmt/json";
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/cmd/test/fmt/String";// cmd  REPRESENT command type AND COMMAND IS TEST OF FORMAT STRING
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

SoftwareSerial mySerial(D2,D3); // incoming sensor data
SoftwareSerial mySerial1(D5 ,D6); // retrieve mesg from these pins
String talkBackCommand,tbRequest,hostm,msgurl,url1;
Servo servo;
long randNumber;
int State ;

const char* ssid     = "GUEST";//Enter the ssid of your router
const char* password = "12345678";//Enter the password of your router   // changes must be done accordingly

int publishInterval = 5000; // 30 seconds
long lastPublishMillis;
void publishData();

void setup() {
  Serial.begin(115200);
  pinMode(D1,INPUT);
  mySerial.begin(115200);

   mySerial1.begin(115200);
  servo.attach(D4);

  
  delay(10);
  

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
//  l=1;
  msg91();
  mqttConnect();
}

void loop() 
{
 Sensor_Data();
delay(1000);

delay(1000);

 
delay(10000);

msg91();
 if (millis() - lastPublishMillis > publishInterval){
  {
    publishData();
    lastPublishMillis = millis();
  }
  
  if (!client.loop()) {
    mqttConnect();
  }
}
delay(2000);

}
 



/*......................................Sensor Data.....................................*/
void Sensor_Data()
{
   State = digitalRead(D1);
  Serial.println(State);
  if ( State == 0){
    randNumber = random(1000,9999);
  Serial.println(randNumber);
  url1="/api/sendhttp.php?mobiles=7013558600&authkey=282071A4LUSvtnxpl5d0c7c93&route=4&sender=TESTIN&message="+String(randNumber)+"&country=91";
   msgurl = url1;
  msggg();
  }
  delay(1000);  
}
/*.....................................Uploading to Cloud...............................*/


void msg91(){

  
url1="/api/sendhttp.php?mobiles=7013558600&authkey=282071A4LUSvtnxpl5d0c7c93&route=4&sender=TESTIN&message="+String(randNumber)+"&country=91";
 msgurl = url1;
// thingspeak_host="api.msg91.com";
  if( mySerial1.available()>0){
    
 
//   msg91_data = mySerial1.readStringUntil('*');
//   Serial.println(msg91_data);

  if (mySerial1.find('#')){
     msg91_data = mySerial1.readStringUntil('*');
     Serial.println("data is available for message");
    Serial.println("found #");
    
l=1;
     Serial.println(msg91_data);
  }}
    if(l==1){
    
msggg();
    }
  l=0;
}


void msggg(){
  hostm="api.msg91.com";
  Serial.print("connecting to ");
  Serial.println(hostm);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect("api.msg91.com", httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request

 

  Serial.print("msgurl: ");
  Serial.println(msgurl);
  
  // This will send the request to the server
  client.print(String("GET ") + msgurl + " HTTP/1.1\r\n" +
               "Host: " +hostm + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(1000);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
}
void publishData()
{
  if(!!!client.connected()){//if client is connected or not
    Serial.print("Reconnectng client to");
    Serial.println(server);
    while(!!!client.connect(clientId,authMethod,token)){ //if client not connected to abov credentials
      Serial.print(".");
      delay(500);
      delay(500);
    }
    Serial.println();
    }
  
  String payload = "{\"d\":{\"latitude\":";
  payload += latitude;
  payload += ",""\"longitude\":";
  payload +=  longitude;
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
  Serial.print("callback invoked for topic: "); Serial.println(topic);

  for (int i = 0; i < payloadLength; i++) {
    //Serial.println((char)payload[i]);
    command += (char)payload[i];
  }
-
Serial.println(command);
if(command==String(randNumber))
{
    servo.write(180);
  Serial.println("srvoo");
  delay(30000);
 servo.write(0);
  }
command ="";
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

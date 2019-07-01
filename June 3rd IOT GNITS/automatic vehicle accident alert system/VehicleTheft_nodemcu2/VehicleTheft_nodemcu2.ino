//nodemcu
#include <SoftwareSerial.h>                             // Software Serial Library so we can use other Pins for communication with the GPS module
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#define ORG "gabjnx"
#define DEVICE_TYPE "niharika"
#define DEVICE_ID "2016"
#define TOKEN "17251A0468"
//-------- Customise the above values --------
float  latitude = 17.4195 ;
float longitude = 78.4486;
void msg();
String url;
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/Data/fmt/json";//its the topic that is created for a mqtt protocol since it has publish and subscribe method of data access
char authMethod[] = "use-token-auth";
char token[] = TOKEN;//this is defined in pre processor commands above
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
const char* ssid     = "GUEST";
const char* password = "12345678";

const char* host = "api.msg91.com";

SoftwareSerial mySerial(D1,D8);            // D2=RX, D3=TX,ARDUINO
String u;
String url2;
WiFiClient wificlient;
PubSubClient client(server, 1883,wificlient);


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
  mySerial.begin(9600);
Serial.println();
  Serial.println("hi");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void loop() {
  

//if (mySerial.find("$"))
//{
//url2=mySerial.readStringUntil('!');
  //Serial.println(url2);
 //u = "/api/sendhttp.php?route=4&sender=TESTIN&mobiles=8790438259&authkey=280146ArgtqFMG5cfb6063&message=accident occured"+url2;
u = "/api/sendhttp.php?route=4&sender=TESTIN&mobiles=8790438259&authkey=280146ArgtqFMG5cfb6063&message=accident occured:http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
u+=(String)latitude;

u+="+";
u+=(String)longitude;
Serial.print("Requesting URL: ");
  Serial.println(u);
url=u;
msg();
//}

//else{
  //Serial.println("no data from arduino1");
//}

  delay(5000);
PublishData(latitude,longitude);
 delay(5000);


}

void PublishData(float lati, float longi){
 if (!!!client.connected()) {//if client is connected or  not
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) {//if client is not connected  to the above credentials then the dots are printed
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
  String payload = "{\"d\":{\"latitude\":";//it is stired in the payload string
  payload += lati;
  payload+="," "\"longitude\":";
  payload += longi;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);//the payload is stored cgaracter by character
  
 if (client.publish(topic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
}

void msg(){
const int httpPort = 80;
  if (!wificlient.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  wificlient.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  
  // Read all the lines of the reply from server and print them to Serial
  while(wificlient.available()){
    String line = wificlient.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");

}

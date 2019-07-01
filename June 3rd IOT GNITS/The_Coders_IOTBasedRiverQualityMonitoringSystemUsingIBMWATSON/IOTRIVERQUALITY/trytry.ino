#include <OneWire.h>
#include <DallasTemperature.h>
//the pin you connect the ds18b20 to
#include <ESP8266WiFi.h>-------- //nodemcu to wifi
#include <PubSubClient.h> //this publish and subscribe model(mqtt) protocol
 #include <Wire.h> //display using I2C communication.
#include <Adafruit_SSD1306.h> //ic driver
#include <Adafruit_GFX.h> //graphical interface
#define SSD1306_LCDHEIGHT 64 //defining the height of the display
#define DS18B20 D5
OneWire ourWire(DS18B20);
DallasTemperature sensors(&ourWire);

// OLED display TWI address
#define OLED_ADDR   0x3C //address is by default

Adafruit_SSD1306 display(-1); //libraray file defining instance.use sti communication we have reset pin.since we dont have reset pin we use -1

#if (SSD1306_LCDHEIGHT != 64) //height is not 64 it give the error
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

int sensorPin = A0;
float volt;
float ntu;
//-------- Customise these values -----------
const char* ssid = "GUEST";
const char* password = "12345678";
const char* host = "api.msg91.com";
#include "DHT.h" //publish temperature and humidity through dht sensors connected to nodemcu.dht output pin to D2 nodemcu
#define DHTPIN D6    // what pin we're connected to
#define DHTTYPE DHT11   // define type of sensor DHT 11,creating instance and passing the parameters
DHT dht (DHTPIN, DHTTYPE);
 
#define ORG "gu488j"
#define DEVICE_TYPE "anonymous_shree"
#define DEVICE_ID "9999"
#define TOKEN "9705101809"
//-------- Customise the above values --------
 
char server[] = ORG ".messaging.internetofthings.ibmcloud.com"; //server name
char topic[] = "iot-2/evt/Data/fmt/json"; //some topics in mqtt topic name is iot
char authMethod[] = "use-token-auth"; //use token authentication
char token[] = TOKEN; //token is already stored in this token varaiable
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID; 
 
WiFiClient wifiClient; //instance
PubSubClient client(server, 1883,wifiClient); //server name and port number of mqtt and passing variable wifi client

void setup() {
   pinMode(D7,OUTPUT);
  pinMode(D8,OUTPUT);
 Serial.begin(115200); //baud statement
 delay(1000);
 Serial.println();
 dht.begin();
 Serial.print("Connecting to "); //connecting to a perticular ssid
 Serial.print(ssid);
 WiFi.begin(ssid, password); //connecting
 while (WiFi.status() != WL_CONNECTED) { //checking wifi status.the wifi is connected
 delay(500);
 Serial.print("."); //after connecting if we are getting dots then we will be printed with dots.
 }
 Serial.println("");
 
 Serial.print("WiFi connected, IP address: ");
 Serial.println(WiFi.localIP());
 display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR); //intialising ssd1306-internally generate 3.3v of voltage and then intially we clear display with cleardisplay
  display.clearDisplay();
  sensors.begin();

  
// display a line of text
  display.setTextSize(1); //1 to 8 test size
  display.setTextColor(BLACK,WHITE); //text color is set as white
  display.setCursor(0,10); //we are setting the cursor in which row and column
  display.print("SMART BRIDGE!!"); //to display
 
 
  // update display with all of the above graphics
  display.display();
}
 
void loop() {
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  volt = 0;
    for(int i=0; i<800; i++)
    {
        volt += ((float)analogRead(sensorPin)/1023)*5;
    }
    volt = volt/800;
    volt = round_to_dp(volt,1);
    if(volt < 2.5){
      ntu = 3000;
    }else{
      ntu = -1120.4*sq(volt)+5742.3*volt-4353.8; 
    }
  delay(1000);
  display.clearDisplay();
  // display a line of text
  display.setTextSize(1); //1 to 8 test size
  display.setTextColor(BLACK,WHITE); //text color is set as white
  display.setCursor(3,10); //we are setting the cursor in which row and column
  display.print("VOLT"); //to display
  display.print(" ");
  display.print(volt);
  display.setCursor(2,20);
  display.print("NTU"); //to display
  display.print(" ");
  display.print(ntu);
// update display with all of the above graphic 
  display.display(); //to apply the changing on the display. cant see anything if u miss this.

  
  
float h = dht.readHumidity(); //read values
float t = dht.readTemperature(); //getting temperature values and storing
if(h<=35 && t<=35)
{
digitalWrite(D7,HIGH);
digitalWrite(D8,LOW);
}
else if(h>=35 || t>=35)
{
  digitalWrite(D7,LOW);
  digitalWrite(D8,HIGH);
}
/*
if( h>=35 || t>=35 || volt>=2.5){
  // We now create a URI for the request
  String url = "/api/sendhttp.php?mobiles=9705210981&authkey=280125AFHo1HYin1n25cfb6099&route=4&sender=TESTIN&message=ALERT!%20The%20river%20water%20is%20not%20clean&country=india";

  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(5000);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
    
  } 
  
  Serial.println();
  Serial.println("closing connection");
} */
sensors.requestTemperatures();
float wt=sensors.getTempCByIndex(0);
delay(1000);
if (isnan(h) || isnan(t)) //valid functions or not is-not-a-number(isan)
{
Serial.println("Failed to read from DHT sensor!");
delay(1000);
return;
}
PublishData(t,h,ntu,volt,wt); //passing 2 values
delay(100);

}

void PublishData(float temp, float humid,float ntu,float volt,float wt){ //2 parameters as temp and humid,t value is stored in temp and h into humidity
 if (!!!client.connected()) { //we will client to client if the client is not connected the it will print reconnecting client to server name.
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) { //if client is not connected and if server name is wrong. or it will not be connected to cloud
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
  String payload = "{\"d\":{\"temperature\":"; //string as payload
  payload += temp; 
  payload+="," "\"humidity\":";
  payload += humid;
  payload+="," "\"ntu\":";
   payload += ntu;
   payload+="," "\"volt\":";
   payload += volt;
   payload+="," "\"watertemp\":";
   payload += wt;
   payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload); //printing data on serial monitor
  
 if (client.publish(topic, (char*) payload.c_str())) { //passing th etopic name and passing the variable as payroll
 Serial.println("Publish ok"); //if data is correct then the data is published according to topic varaiable
 } else {
 Serial.println("Publish failed");
 }
   
}
 float round_to_dp( float in_value, int decimal_place )
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}

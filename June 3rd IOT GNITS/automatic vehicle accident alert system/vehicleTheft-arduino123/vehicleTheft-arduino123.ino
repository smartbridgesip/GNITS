//arduino code

#include <TinyGPS++.h> 
#include <SoftwareSerial.h>
TinyGPSPlus gps;                                        // Create an Instance of the TinyGPS++ object called gps
SoftwareSerial ss(5,6);    
SoftwareSerial mySerial(7,8); //D2-RX, D3-TX
String value,url1;
float latitude=17.4195;
float longitude=78.4486;

int a=A1;
int b=A3;


void setup() {
  // put your setup code here, to run once:
//pinMode(a,INPUT);
//pinMode(b,INPUT);
//pinMode(c,INPUT);
Serial.begin(9600);
mySerial.begin(9600);
ss.begin(9600);

}
void loop() {
  int x,y,z;
y=analogRead(a);
z=analogRead(b);
Serial.println("y value");
Serial.println(y);
Serial.println(" z value");
Serial.println(z);
delay(1000);

if (y<=395||y>=400 ||z<=395||z>=400) 
{
  //latitude=gps.location.lat();
  //longitude=gps.location.lng();
    Serial.println(latitude);
 Serial.println(longitude);
 delay(1000);
 //smartDelay(1000); 
 String p="http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
 
 String url1="$";
 url1 += p ;
  url1 += String(latitude,DEC);
  
  url1 += String(longitude,DEC);
  
  
  url1 +="!";
  mySerial.println(url1);
  Serial.println(url1);
    
}   
    
    if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
delay(2000);

}


//static void smartDelay(unsigned long ms)                // This custom version of delay() ensures that the gps object is being "fed".
//{
 // unsigned long start = millis();
  //do 
  //{
   // while (ss.available())
     // gps.encode(ss.read());
 // } while (millis() - start < ms);

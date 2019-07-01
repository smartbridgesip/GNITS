


int flowPin = 2;    //This is the input pin on the Arduino
double flowRate;    //This is the value we intend to calculate. 
volatile int count; //This integer needs to be set as volatile to ensure it updates correctly during the interrupt process. 

#include <GSMSim.h>


#define RX 8 
#define TX 7 
#define BAUD 9600
const int soil_sensor = A0;  // Analog input pin that the soil moisture sensor is attached to
int sensorValue = 0;    

 

GSMSim gsm; 
 
void setup() {
  // put your setup code here, to run once:
  pinMode(flowPin, INPUT);           //Sets the pin as an input
   attachInterrupt(0, Flow, RISING); //Configures interrupt 0 (pin 2 on the Arduino Uno) to run the function "Flow"  
  Serial.begin(9600);  //Start Serial
  
  gsm.start(); 
}
void loop() {

  // put your main code here, to run repeatedly:  

  count = 0;      // Reset the counter so we start counting from 0 again
  interrupts();   //Enables interrupts on the Arduino
  delay (1000);   //Wait 1 second 
  noInterrupts(); //Disable the interrupts on the Arduino
   
  //Start the math
  flowRate = (count * 2.25);        //Take counted pulses in the last second and multiply by 2.25m 
  flowRate = flowRate * 60;         //Convert seconds to minutes, giving you mL / Minute
  flowRate = flowRate / 1000;       //Convert mL to Liters, giving you Liters / Minute
 
  Serial.println(flowRate);         //Print the variable flowRate to Serial
 if(flowRate>=20)
 {

  Serial.println("GSMSim Library - SMS Example");
  Serial.println("");
  delay(1000);

  Serial.println("Changing to text mode.");
  gsm.smsTextMode(true); // TEXT or PDU mode. TEXT is readable :)

  char* number = "9618662007";
  char* message = "WATER IS COMING!!! SWITCH ON THE PUMP"; // message lenght must be <= 160. Only english characters.

  Serial.println("Sending Message --->");
  Serial.println(gsm.smsSend(number, message)); // if success it returns true (1) else false (0)
  delay(2000);

  Serial.println("Listing unread message(s).");
  Serial.println(gsm.smsListUnread()); // if not unread messages have it returns "NO_SMS"

  Serial.println("Read SMS on index no = 1");
  Serial.println(gsm.smsRead(1)); // if no message in that index, it returns IXDEX_NO_ERROR
  
}
else
{
  sensorValue = analogRead(soil_sensor);            
  
  // print the sensor results to the serial monitor:
  Serial.print("Moisture Value = " );                       
  Serial.println(sensorValue);      

  // delay of one second
  delay(1000);  
  if(sensorValue<=200)
  {
    Serial.println("GSMSim Library - SMS Example");
  Serial.println("");
  delay(1000);

//  gsm.start(); // baud default 9600
  //gsm.start(BAUD);

  Serial.println("Changing to text mode.");
  gsm.smsTextMode(true); // TEXT or PDU mode. TEXT is readable :)

  char* number = "9618662007";
  char* message = "IRRIGATION DONE!!! SWITCH OFF THE PUMP"; // message lenght must be <= 160. Only english characters.

  Serial.println("Sending Message --->");
  Serial.println(gsm.smsSend(number, message)); // if success it returns true (1) else false (0)
  delay(2000);

  Serial.println("Listing unread message(s).");
  Serial.println(gsm.smsListUnread()); // if not unread messages have it returns "NO_SMS"

  Serial.println("Read SMS on index no = 1");
  Serial.println(gsm.smsRead(1)); // if no message in that index, it returns IXDEX_NO_ERROR

  
}
}
}
void Flow()
{
   count++; //Every time this function is called, increment "count" by 1
}

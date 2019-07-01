#include <SPI.h>//include the SPI bus library
#include <MFRC522.h>//include the RFID reader library
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define SS_PIN D8  //slave select pin
#define RST_PIN D3  //reset pin
#define  yfsensor1 D2

MFRC522 mfrc522(SS_PIN, RST_PIN);        // instatiate a MFRC522 reader object.
MFRC522::MIFARE_Key key;//create a MIFARE_Key struct named 'key', which will hold the card information
Servo myservo;

String datax;
int block=2;
byte blockcontent[16] = {"100"};
byte blockcontent1[16] = {"100"};
byte readbackblock[18];//This array is used for reading out a block. The MIFARE_Read method requires a buffer that is at least 18 bytes to hold the 16 bytes of a block.
byte read1;
String data2;

volatile int flowing_frequency;
float waterquantity_prhour;
unsigned long currentTime1;
unsigned long cloopTime1;
int duration;
int level;

const char* ssid = "GUEST";
const char* password = "12345678";
#define ORG "khp5vn"
#define DEVICE_TYPE "bruno2.0"
#define DEVICE_ID "01"
#define TOKEN "123456789"
//-------- Customise the above values --------
 
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/Data/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
 
WiFiClient wifiClient;
PubSubClient client(server, 1883,wifiClient);

ICACHE_RAM_ATTR void rpm()
{
 flowing_frequency++; 
}

void setup() {
  Serial.begin(9600);
  Serial.println();
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

 SPI.begin();               // Init SPI bus
        mfrc522.PCD_Init();        // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device)
        Serial.println("Scan a card");
        for (byte i = 0; i < 6; i++) {
                key.keyByte[i] = 0xFF;//keyByte is defined in the "MIFARE_Key" 'struct' definition in the .h file of the library
        }
        myservo.attach(D1);
        pinMode(yfsensor1,INPUT);

}
 
void loop() {

if ( ! mfrc522.PICC_IsNewCardPresent()) {//if PICC_IsNewCardPresent returns 1, a new card has been found and we continue
    return;//if it did not find a new card is returns a '0' and we return to the start of the loop
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {//if PICC_ReadCardSerial returns 1, the "uid" struct (see MFRC522.h lines 238-45)) contains the ID of the read card.
    return;//if it returns a '0' something went wrong and we return to the start of the loop
  }

   Serial.print(F("Card UID:"));
 dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println();
          Serial.println("card detected and the details are");
         namedata();
         readBlock(block, readbackblock);//read the block back
         Serial.println("read block: ");
         for (int j=0 ; j<6 ; j++)//print the block contents
         {
          
          Serial.write (readbackblock[j]);
         }
         read1 = (((readbackblock[0]-'0')*100) + ((readbackblock[1]-'0')*10) +((readbackblock[2]-'0')*1));
         Serial.println("the available amount is");
         Serial.println(read1);
         Serial.println("\n**End Reading**\n");
           //writeBlock(block,blockcontent1);
         amountDeduction();
          mfrc522.PICC_HaltA();
         mfrc522.PCD_StopCrypto1();
         float r=read1;

 delay(5000);
 int pos;

  for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }

   attachInterrupt(4, rpm, RISING); // Setup Interrupt m
   
   currentTime1 = millis();
   // Every second, calculate and print litres/hour
   if(currentTime1 >= (cloopTime1 + 1000))
   { Serial.println("yes");    
      cloopTime1 = currentTime1;              // Updates cloopTime
      waterquantity_prhour = (flowing_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flow rate in L/hour 
      flowing_frequency = 0;                   // Reset Counter
      Serial.print(waterquantity_prhour);            // Print litres/hour
      Serial.println(" L/hour");
   }
float w=waterquantity_prhour;   
delay(3000);
if(w>=300)
{
  for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

PublishData(w,r);
delay(100);

}


void PublishData(float w, float r ){
 if (!!!client.connected()) {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
  String payload = "{\"d\":{\"CARD ID\":";
  //payload += ;                                                                                                                    
  payload+="," "\"available balance\":";
  payload += r;
  payload+="," "\"litres per hour\":";
  payload += w;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(topic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
}

void amountDeduction()
{
         read1=read1-10;
         Serial.println("amount after detucting 10rs");
        data2=String(read1);
        data2.getBytes(blockcontent, 16);
        writeByteArray(blockcontent, 16);
         writeBlock(block,blockcontent);
         Serial.println("done updating the amount to card");
          delay(1000); 
}
int writeBlock(int blockNumber, byte arrayAddress[]) 
{
  //this makes sure that we only write into data blocks. Every 4th block is a trailer block for the access/security info.
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector
  if (blockNumber > 2 && (blockNumber+1)%4 == 0){Serial.print(blockNumber);Serial.println(" is a trailer block:");return 2;}//block number is a trailer block (modulo 4); quit and send error code 2
  Serial.print(blockNumber);
  Serial.println(" is a data block:");
  /*****************************************authentication of the desired block for access***********************************************************/
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
         Serial.print("PCD_Authenticate() failed: ");
               mfrc522.GetStatusCodeName((MFRC522::StatusCode)status);
          Serial.println( mfrc522.GetStatusCodeName((MFRC522::StatusCode)status));    
         return 3;//return "3" as error message
  }
 
  /*****************************************writing the block***********************************************************/
        
  status = mfrc522.MIFARE_Write(blockNumber, arrayAddress, 16);//valueBlockA is the block number, MIFARE_Write(block number (0-15), byte array containing 16 values, number of bytes in block (=16))
  //status = mfrc522.MIFARE_Write(9, value1Block, 16);
  if (status != MFRC522::STATUS_OK) {
           Serial.print("MIFARE_Write() failed: ");
                     mfrc522.GetStatusCodeName((MFRC522::StatusCode)status);
          Serial.println( mfrc522.GetStatusCodeName((MFRC522::StatusCode)status));
           return 4;//return "4" as error message
  }
  Serial.println("block was written");
}

int readBlock(int blockNumber, byte arrayAddress[]) 
{
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector

  /*****************************************authentication of the desired block for access***********************************************************/
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
         Serial.print("PCD_Authenticate() failed (read): ");
                   mfrc522.GetStatusCodeName((MFRC522::StatusCode)status);
          Serial.println( mfrc522.GetStatusCodeName((MFRC522::StatusCode)status));
         return 3;//return "3" as error message
  }

  byte buffersize = 18;//we need to define a variable with the read buffer size, since the MIFARE_Read method below needs a pointer to the variable that contains the size... 
  status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);//&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number
  if (status != MFRC522::STATUS_OK) {
          Serial.print("MIFARE_read() failed: ");
          mfrc522.GetStatusCodeName((MFRC522::StatusCode)status);
          Serial.println( mfrc522.GetStatusCodeName((MFRC522::StatusCode)status));          
          return 4;//return "4" as error message
  }
  Serial.println("block was read");
}

void namedata(){
    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;
  Serial.println(F("**Card Detected:**"));
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); //dump some details about the card
  Serial.print(F("Name: "));
  byte buffer1[18];
  block = 4;
  len = 18;
  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    // Serial.print(F("Authentication failed: "));
    //Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    //Serial.print(F("Reading failed: "));
   // Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  //PRINT FIRST NAME
  for (uint8_t i = 0; i < 16; i++)
  {
    if (buffer1[i] != 32)
    {
      Serial.write(buffer1[i]);
    }
  }
  Serial.print(" ");
  delay(1000); //change value if you want to read cards faster
}

void writeByteArray(byte array[], int arrlength)
{
  for (int j = 0 ; j < arrlength ; j++) //print the block contents
  {
    blockcontent[j]=array[j];
    Serial.write (array[j]);//Serial.write() transmits the ASCII numbers as human readable characters to serial monitor
  }
  Serial.println("");
}
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

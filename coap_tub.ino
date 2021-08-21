#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <coap-simple.h>
#include <MFRC522.h>

#define WATERLEVEL A0 
#define TLED D1
#define DLED D8
#define BLED D0

MFRC522 mfrc522(D4, D3);
const char* ssid     = "home";
const char* password = "34275767";
int temperature=0;
int depth =0;

void callback_response(CoapPacket &packet, IPAddress ip, int port);
WiFiUDP udp;
Coap coap(udp);

void callback_response(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("[Coap Response got]");
  
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
//  Serial.println(p);
  bool temp = false;
  String tempera = "";
  String depptt ="";
  char sep = 'm';
  for (int i = 0; i <  packet.payloadlen; i++) 
  {
    if ((char)p[i] != sep && temp == false){
      tempera = tempera + (char)p[i];
    }
      
    else if ((char)p[i] == sep && temp == false){
      temp = true;
    }
    else if ((char)p[i] != sep && temp == true){
  
      depptt = depptt + (char)p[i];
      
    }
     
  }
  temperature =tempera.toInt();
  depth = depptt.toInt();
  Serial.print(tempera);
  Serial.print(" ");
  Serial.print(depptt);
  analogWrite(TLED, temperature);
  Serial.println();
  Serial.println("-----------------------");
  
}

void setup() {
  pinMode( TLED, OUTPUT);
  pinMode( DLED, OUTPUT);
  pinMode( BLED, OUTPUT);
  digitalWrite(BLED, HIGH);
  digitalWrite(DLED, LOW);
  pinMode( WATERLEVEL, INPUT);
  Serial.begin(115200);
  while(!Serial);
  Serial.setDebugOutput(false);
  Serial.println();
  Serial.println();
  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
    Serial.print("Connected to WiFi :");
  Serial.println(WiFi.SSID());

  
  Serial.println("Setup Response Callback");
  coap.response(callback_response);
  coap.start();

  analogWriteRange(100);

  SPI.begin();           
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();
}
char readCard[5];
int waterValue = 0;
unsigned long previousMillis = 0;       
const long interval = 1000; 
int ledState = LOW;  
void array_to_string(byte array[], unsigned int len, char buffer[])
{
   for (unsigned int i = 0; i < len; i++)
   {
      byte nib1 = (array[i] >> 4) & 0x0F;
      byte nib2 = (array[i] >> 0) & 0x0F;
      buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
      buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
   }
   buffer[len*2] = '\0';
}
void loop() {
  unsigned long currentMillis = millis();
  if (mfrc522.PICC_IsNewCardPresent()){
    if(mfrc522.PICC_ReadCardSerial()){
      Serial.println(">>RFID TAG UID:");
      for (byte i =0; i < mfrc522.uid.size; ++i){
        readCard[i] =(char)mfrc522.uid.uidByte[i];
        Serial.print(mfrc522.uid.uidByte[i] , HEX);
        Serial.print(" ");
      }
      char str[32] = "";
       array_to_string(mfrc522.uid.uidByte, 4, str);
       mfrc522.PICC_HaltA();
      int msgid = coap.put(IPAddress(192, 168, 1, 106), 5683,"uid", str);
      Serial.println("request sent");
      delay(1000);
  }
  else;
  coap.loop();
  }
  else{
  waterValue = analogRead(WATERLEVEL);
  waterValue = map(waterValue, 0, 1023, 0, 100);
  analogWrite(DLED, waterValue); 
  delay(100);
//  Serial.println(depth);
//  Serial.println(temperature);
//  Serial.println(waterValue);
  int maxi;
  int mini;
  if (waterValue < depth){
    maxi = depth;
    mini = waterValue;
  }
   else{
    maxi = waterValue;
    mini = depth;
   }
  if((maxi-mini) > 10){
        if (currentMillis - previousMillis >= interval) {
          previousMillis = currentMillis;
          if (ledState == LOW) {
            ledState = HIGH;
          } else {
            ledState = LOW;
          }
          digitalWrite(BLED, ledState);
        }

  }
  }

}

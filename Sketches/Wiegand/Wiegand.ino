#include <SPI.h>
#include <Ethernet.h>
#define MESSAGE_LEN 26
#define CARDNUM_LEN 24

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,20,177);
IPAddress gateway(192,168,20,1);
IPAddress subnet(255, 255, 255, 0);
const int serverPort = 9600;
EthernetServer server = EthernetServer(serverPort);

int toVertX_D0 = 5; // output DATA0 (Green) to Controller
int toVertX_D1 = 6; // output DATA1 (White) to Controller

unsigned long cardValue = 0;

void setup(){
   Serial.begin(9600); 
 // Ethernet.begin(mac, ip, gateway, subnet);
 // server.begin();
    pinMode(toVertX_D0,OUTPUT);
    digitalWrite(toVertX_D0, HIGH);

    pinMode(toVertX_D1,OUTPUT);
    digitalWrite(toVertX_D1, HIGH);
    
    resetState();
}

void writeCard(unsigned long sendValue) {
    const byte sendDelay=200;
     for (short x=MESSAGE_LEN-1; x>=0; x--) {
      if ( bitRead(sendValue,x) == 1 ) {
        digitalWrite(toVertX_D1, LOW);
        delayMicroseconds(sendDelay);
        digitalWrite(toVertX_D1, HIGH);
      } else {
        digitalWrite(toVertX_D0, LOW);
        delayMicroseconds(sendDelay);
        digitalWrite(toVertX_D0, HIGH);
      }
      Serial.print(bitRead(sendValue,x));
      delayMicroseconds(2000);
    }
     Serial.println();
 }
 void resetState() {
    cardValue=0;
 }
 
 
// void AppendByteToUint(byte index, byte value)
// {
//   const byte byteSize=4;
//   for(  byte i=0;i <byteSize; i++  )
//     bitWrite(cardValue, lastBit + byteSize*index+i,bitRead(value,i));
// }
 
 void AppendCheckSum()
 {

    byte summ=0;
    byte middlePoint = CARDNUM_LEN/2;
  
    for(  short i=CARDNUM_LEN-1;i >= middlePoint; i--)
    {
      Serial.print( bitRead(cardValue,i), DEC);
      summ += bitRead( cardValue,i );
    }
    
    if(summ%2==0)
     bitClear( cardValue,CARDNUM_LEN);
    else
      bitSet(cardValue,CARDNUM_LEN);
    Serial.println( summ,DEC );
    
    summ=0;
    
    for(  short i=middlePoint-1; i >= 0; i--)
    {
       Serial.print(bitRead( cardValue,i), DEC);
       summ += bitRead( cardValue,i );
    }
    cardValue = cardValue << 1;
    if(summ%2 == 0)
        bitSet( cardValue, 0);
    else
        bitClear(cardValue,0);
    Serial.println( summ, DEC);  
 }

 
void loop(){
  //Serial.println("Hello world!"); 
 // EthernetClient client = server.available();
 delay(200);
 if (Serial.available() ) {
  //if (client) {
    while (Serial.available()) cardValue = cardValue * 10 + (Serial.read() - '0');
       
       Serial.println(cardValue, DEC);
       AppendCheckSum();

       Serial.println(cardValue, DEC);
       writeCard(cardValue);
       delay(200);
       resetState();
  }

}


#include <SPI.h>
#include <EthernetV2_0.h>
#include <avr/wdt.h>
#define MESSAGE_LEN_CARD 26
#define CARDNUM_LEN 24
#define MESSAGE_LEN_PIN 8
#define ENTER_PIN 11
#define W5200_CS  10
#define SDCARD_CS 4

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xAB, 0xFE, 0xED };
IPAddress ip(192, 168, 20, 195);
IPAddress gateway(192, 168, 20, 1);
IPAddress subnet(255, 255, 255, 0);
const int serverPort = 9600;
const int ledPin=13;
EthernetServer server = EthernetServer(serverPort);

//unsigned long cardValue = 0;
//iny pinValue = 0;

enum AccessType {
    Card, Pin
  };
void setup() {
  
  wdt_disable();
  
  SetupPin(SDCARD_CS);
  pinMode(ledPin, OUTPUT);
  
  SetupPin(3);
  SetupPin(5);

  SetupPin(6);
  SetupPin(9);

  SetupPin(A0);
  SetupPin(A1);

  SetupPin(A2);
  SetupPin(A3);

  SetupPin(A4);
  SetupPin(A5);
    
  Ethernet.begin(mac, ip, gateway,  subnet);
  server.begin();
  
   Serial.begin( 9600 );
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  Serial.println(Ethernet.localIP());
  Serial.print("Chat server address:");
  
  resetState();
  Serial.print("Chat server done");
}
void SetupPin(byte pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

void writeCard(unsigned long sendValue,  int WDO, int WD1) {
  const byte sendDelay = 200;
  for (short x = MESSAGE_LEN_CARD - 1; x >= 0; x--) {
    if ( bitRead(sendValue, x) == 1 ) {
      digitalWrite(WD1, LOW);
      digitalWrite(ledPin, HIGH);
      delayMicroseconds(sendDelay);
      digitalWrite(WD1, HIGH);
      digitalWrite(ledPin, LOW);
    } else {
      digitalWrite(WDO, LOW);
      digitalWrite(ledPin, HIGH);
      delayMicroseconds(sendDelay);
      digitalWrite(WDO, HIGH);
      digitalWrite(ledPin, LOW);
    }
    Serial.print(bitRead(sendValue, x));
    delayMicroseconds(2000);
  }
  Serial.println();
}

void writeData(unsigned long sendValue, byte messageLen, int WDO, int WD1) {
  const byte sendDelay = 200;
  for (short x = messageLen - 1; x >= 0; x--) {
    if ( bitRead(sendValue, x) == 1 ) {
      digitalWrite(WD1, LOW);
      digitalWrite(ledPin, HIGH);
      delayMicroseconds(sendDelay);
      digitalWrite(WD1, HIGH);
      digitalWrite(ledPin, LOW);
    } else {
      digitalWrite(WDO, LOW);
      digitalWrite(ledPin, HIGH);
      delayMicroseconds(sendDelay);
      digitalWrite(WDO, HIGH);
      digitalWrite(ledPin, LOW);
    }
    Serial.print(bitRead(sendValue, x));
    delayMicroseconds(2000);
  }
  Serial.println();
}

void resetState() {

}


unsigned long AppendByte(unsigned long data, int index, byte value)
{
  const byte byteSize = 8;
  for (  byte i = 0; i < byteSize; i++  )
    bitWrite(data, byteSize  * index + i, bitRead( value, i));
  return data;   
}


unsigned long AppendCheckSum(unsigned long data, int length)
{

  byte summ = 0;
  int middlePoint = length / 2;

  for (  short i = length - 1; i >= middlePoint; i--)
  {
    Serial.print( bitRead(data, i), DEC);
    summ += bitRead( data, i );
  }

  if (summ % 2 == 0)
    bitClear( data, length);
  else
    bitSet(data, length);
  Serial.println( summ, DEC );

  summ = 0;

  for (  short i = middlePoint - 1; i >= 0; i--)
  {
    Serial.print(bitRead( data, i), DEC);
    summ += bitRead( data, i );
  }
  data = data << 1;
  if (summ % 2 == 0)
    bitSet( data, 0);
  else
    bitClear(data, 0);
  Serial.println( summ, DEC);
  return data;
}

byte AppendInvertBits(byte data, int length)
{ 
  int middlePoint = length / 2;
  for ( int i = 0; i < middlePoint; i++ )
  {
     bitWrite( data, middlePoint + i, 1^bitRead( data, i ));
  } 
  return data; 
}


unsigned long TakeDigit( unsigned long number, int index )
{
  while( index-- )
  {
     Serial.println(number, DEC);
     number /= 10;
  }
  return number % 10;
}

void SendData( byte readerNumber, unsigned long sendValue, byte messageLen ) {
        switch (readerNumber)
      {
        case 0:  writeData(sendValue, messageLen, 3, 5); break;
        case 1:  writeData(sendValue, messageLen, 6, 9); break;
        case 2:  writeData(sendValue, messageLen, A0, A1); break;
        case 3:  writeData(sendValue, messageLen, A2, A3); break;
        case 4:  writeData(sendValue, messageLen, A4, A5); break;
      }
}

void loop() {

  EthernetClient client = server.available();
  if (client) {
    
    Serial.println("\n **** Client in ****");
    
    byte readerNumber = client.read();  
    Serial.println("Reader number:");
    Serial.println(readerNumber, DEC);
    
    byte accessType = client.read();
    Serial.println("Type identification:");
    Serial.println(accessType, DEC);
    
    unsigned long data = 0;
      for (int i = 0; i < 4; i++)
      {
        byte val =  client.read();
        Serial.println("Append value:");
        Serial.println(val, DEC);
        data = AppendByte(data, i, val );
      }

    if( accessType == Card )
    {         
      unsigned long cardValue = data;
      Serial.println("CardNumber:");
      Serial.println(cardValue, DEC);
      
      cardValue = AppendCheckSum(cardValue, CARDNUM_LEN);
      Serial.println("CardNumber after checksum:");
      Serial.println(cardValue, DEC);
  
      SendData( readerNumber, cardValue, MESSAGE_LEN_CARD );
    }
    
    if( accessType == Pin )
    {
      unsigned long pinValue = data;

      Serial.println("Pin:");
      Serial.println(pinValue, DEC);
      
      int pinCount = 0;
      unsigned long numb = pinValue;
      while( numb )
      {
        numb /= 10;
        pinCount++;
      }
      Serial.println("Count:");
      Serial.println(pinCount, DEC);
      for ( int i = pinCount - 1;  i >= 0; i-- )
      { 
        byte pinDigit = (byte)( TakeDigit(pinValue,i) );
        Serial.println("PinDigit:");
        Serial.println(pinDigit, DEC);
        pinDigit = AppendInvertBits(pinDigit, 32);
        SendData( readerNumber, pinDigit, MESSAGE_LEN_PIN );
        delay(50);
      }    
      byte enter = AppendInvertBits(ENTER_PIN, 8); 
      SendData( readerNumber, enter, MESSAGE_LEN_PIN );
    }
    resetState();
  }
  client.stop();
  delay(200);
}




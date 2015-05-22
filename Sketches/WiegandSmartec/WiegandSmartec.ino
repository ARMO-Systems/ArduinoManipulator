#include <SPI.h>
#include <EthernetV2_0.h>
#include <avr/wdt.h>
#define MESSAGE_LEN 26
#define CARDNUM_LEN 24
#define W5200_CS  10
#define SDCARD_CS 4

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xAB, 0xFE, 0xED };
IPAddress ip(192, 168, 20, 195);
IPAddress gateway(192, 168, 20, 1);
IPAddress subnet(255, 255, 255, 0);
const int serverPort = 9600;
const int ledPin=13;
EthernetServer server = EthernetServer(serverPort);

unsigned long cardValue = 0;

void setup() {
  
  wdt_disable();
  
  SetupPin(SDCARD_CS);
  
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
  
  pinMode(ledPin, OUTPUT);
  
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

void writeCard(unsigned long sendValue, int WDO, int WD1) {
  const byte sendDelay = 200;
  for (short x = MESSAGE_LEN - 1; x >= 0; x--) {
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
  cardValue = 0;
}


void AppendByteToCard(int index, byte value)
{
  const byte byteSize = 8;
  for (  byte i = 0; i < byteSize; i++  )
    bitWrite(cardValue, byteSize  * index + i, bitRead( value, i));
}

void AppendCheckSum()
{

  byte summ = 0;
  byte middlePoint = CARDNUM_LEN / 2;

  for (  short i = CARDNUM_LEN - 1; i >= middlePoint; i--)
  {
    Serial.print( bitRead(cardValue, i), DEC);
    summ += bitRead( cardValue, i );
  }

  if (summ % 2 == 0)
    bitClear( cardValue, CARDNUM_LEN);
  else
    bitSet(cardValue, CARDNUM_LEN);
  Serial.println( summ, DEC );

  summ = 0;

  for (  short i = middlePoint - 1; i >= 0; i--)
  {
    Serial.print(bitRead( cardValue, i), DEC);
    summ += bitRead( cardValue, i );
  }
  cardValue = cardValue << 1;
  if (summ % 2 == 0)
    bitSet( cardValue, 0);
  else
    bitClear(cardValue, 0);
  Serial.println( summ, DEC);
}


void loop() {

  EthernetClient client = server.available();
  if (client) {
    Serial.println("Client in");
    byte readerNumber = client.read();
    Serial.println("Reader number:");
    Serial.println(readerNumber, DEC);

    for (int i = 0; i < 3; i++)
    {
      byte val =  client.read();
      Serial.println("Append value:");
      Serial.println(val, DEC);
      AppendByteToCard(i, val );
    }
//   if (Serial.available() ) {
//     byte readerNumber = Serial.read()- '0';
//     Serial.println("Reader number:");
//    Serial.println(readerNumber, DEC);
//    while (Serial.available()) cardValue = cardValue * 10 + (Serial.read() - '0');
    Serial.println("CardNumber:");
    Serial.println(cardValue, DEC);
    AppendCheckSum();
    Serial.println("CardNumber after checksum:");
    Serial.println(cardValue, DEC);

    switch (readerNumber)
    {
      case 1:  writeCard(cardValue, 3, 5); break;
      case 2:  writeCard(cardValue, 6, 9); break;
      case 3:  writeCard(cardValue, A0, A1); break;
      case 4:  writeCard(cardValue, A2, A3); break;
      case 5:  writeCard(cardValue, A4, A5); break;
    }

    resetState();
  }
  client.stop();
  delay(200);
}


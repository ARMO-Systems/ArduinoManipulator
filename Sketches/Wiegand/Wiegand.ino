#include <SPI.h>
#include <EthernetV2_0.h>
#include <avr/wdt.h>
#define CheckSumLength 2



byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 20, 177);
IPAddress gateway(192, 168, 20, 1);
IPAddress subnet(255, 255, 255, 0);
const int serverPort = 9600;
const int ledPin = 13;
EthernetServer server = EthernetServer(serverPort);

unsigned long cardValue = 0;
byte messageLength = 0;

void setup() {

  wdt_disable();

  Serial.begin( 9600 );


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

  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  delay(2000);
  Serial.print( "Server address:" );
  Serial.println( Ethernet.localIP() );

  resetState();
  wdt_enable (WDTO_8S);
}
void SetupPin(byte pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

void writeCard(unsigned long sendValue, int WDO, int WD1) {
  const byte sendDelay = 200;
  for (short x = messageLength - 1; x >= 0; x--) {
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
  messageLength = 0;
}

void AppendCheckSum()
{

  byte summ = 0;
  byte cardNumLength = messageLength - CheckSumLength;
  byte middlePoint =  cardNumLength / 2;

  for (  short i = cardNumLength - 1; i >= middlePoint; i--)
  {
    Serial.print( bitRead(cardValue, i), DEC);
    summ += bitRead( cardValue, i );
  }
  Serial.println();
  if (summ % 2 == 0)
    bitClear( cardValue, cardNumLength);
  else
    bitSet(cardValue, cardNumLength);
  Serial.println( summ, DEC );

  summ = 0;

  for (  short i = middlePoint - 1; i >= 0; i--)
  {
    Serial.print(bitRead( cardValue, i), DEC);
    summ += bitRead( cardValue, i );
  }
  Serial.println();
  cardValue = cardValue << 1;
  if (summ % 2 == 0)
    bitSet( cardValue, 0);
  else
    bitClear(cardValue, 0);
  Serial.println( summ, DEC);
}

unsigned long ReadValue(EthernetClient client)
{
  unsigned long value = 0;
  while (client.connected()) {
    if (client.available()) {
      char symbol = client.read();
      if (symbol != '\n')
        value = value * 10 + ( symbol - '0');
      else
        return value;
    }
  }
  return value;
}

void loop() {

  EthernetClient client = server.available();
  delay(200);

  if (client) {

    byte readerNumber = ReadValue( client );
    Serial.println( "Reader number:" );
    Serial.println( readerNumber, DEC );

    messageLength =  ReadValue( client );
    Serial.println( "Message length:" );
    Serial.println( messageLength, DEC );

    cardValue = ReadValue( client );
    //       if (Serial.available() ) {
    //         byte readerNumber = Serial.read()- '0';
    //           messageLength = Serial.read()-'0';
    //         Serial.println("Reader number:");
    //        Serial.println(readerNumber, DEC);
    //        while (Serial.available()) cardValue = cardValue * 10 + (Serial.read() - '0');
    Serial.println("CardNumber:");
    Serial.println(cardValue, DEC);
    Serial.println("Message length:");
    Serial.println( messageLength, DEC );
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
    delay(200);
    resetState();
  }

  client.stop();
  wdt_reset();
}


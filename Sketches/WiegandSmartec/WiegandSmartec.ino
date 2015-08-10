#include <SPI.h>
#include <avr/wdt.h>


#include <EthernetV2_0.h>
#define UseEthernetV2
//#include <Ethernet.h>

#if  defined(UseEthernetV2)
  #define SDCARD_CS 4
#endif

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xAB, 0xFE, 0xED };
IPAddress ip(192, 168, 20, 195);

IPAddress gateway(192, 168, 20, 1);
IPAddress subnet(255, 255, 255, 0);
const int serverPort = 9600;
const int ledPin = 13;
EthernetServer server = EthernetServer(serverPort);

#define RELAY_1 A5
#define RELAY_2 A4
#define RELAY_POWER_NC 8

void setup() {

  wdt_disable();
#if defined(UseEthernetV2)
  SetupPin(SDCARD_CS);
#endif

  SetupPin(3);
  SetupPin(5);

  SetupPin(6);
  SetupPin(9);

  SetupPin(A0);
  SetupPin(A1);

  SetupPin(A2);
  SetupPin(A3);

  SetupPin(RELAY_1);
  SetupPin(RELAY_2);
  SetupPin(RELAY_POWER_NC);
  
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.begin( 9600 );

  Serial.print( "Server address:" );
  Serial.println( Ethernet.localIP() );

  wdt_enable (WDTO_8S);
}
void SetupPin(byte pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

void writeCard(String sendValue, int WDO, int WD1) {
  const byte sendDelay = 200;
  for (short x = 0; x < sendValue.length(); x++) {
    if ( sendValue[x] == '1' ) {
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
    Serial.print(sendValue[x]);
    delayMicroseconds(2000);
  }
  Serial.println();
}

void turnInput( int action, int inputNum )
{
  if( action == 0 )
  {
    Serial.println( "Close input" );  
    digitalWrite(inputNum,LOW);
  }
  if( action == 1 )
  {
    Serial.println( "Open input" ); 
    digitalWrite(inputNum,HIGH);
  }
  delay(100);  
}
 
void loop() {

  EthernetClient client = server.available();
  if (client) {
     Serial.println( "Client connected" );
    byte readerNumber =  client.read()-'0';
    Serial.println( "Reader number:" );
    Serial.println( readerNumber, DEC );
    
    String binaryString;
    
    while(client.available())
    {
      binaryString += (char)client.read();
    }
    
    Serial.println( "Value:" );
    Serial.println( binaryString );

    switch (readerNumber)
    {
      case 0:  writeCard(binaryString, 3, 5); break;
      case 1:  writeCard(binaryString, 6, 9); break;
      case 2:  writeCard(binaryString, A0, A1); break;
      case 3:  writeCard(binaryString, A2, A3); break;
      case 4:  turnInput(binaryString.toInt(), RELAY_1); break;
      case 5:  turnInput(binaryString.toInt(), RELAY_2); break;
      case 6:  turnInput(binaryString.toInt(), RELAY_POWER_NC); break;
    }
    delay(200);
  }

  client.stop();
  wdt_reset();
}





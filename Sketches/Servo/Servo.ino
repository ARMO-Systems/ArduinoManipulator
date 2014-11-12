#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>

Servo myservo;
byte arduinoMac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
const int serverPort = 9600;
EthernetServer server = EthernetServer(serverPort);

void setup(){
  Ethernet.begin(arduinoMac);
  server.begin();
  const int controlledPin = 3;
  myservo.attach(controlledPin);
}

void loop(){
  EthernetClient client = server.available();
  if (client) {
    myservo.write(client.read());
  }
}
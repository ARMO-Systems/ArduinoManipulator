#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>

Servo myservo;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,20,177);
IPAddress gateway(192,168,20,1);
IPAddress subnet(255, 255, 255, 0);
const int serverPort = 9600;
EthernetServer server = EthernetServer(serverPort);

void setup(){
  Ethernet.begin(mac, ip, gateway, subnet);
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
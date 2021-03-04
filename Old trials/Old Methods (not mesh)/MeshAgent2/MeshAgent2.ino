#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 30); // CE, CSN
//Current agent address
const uint64_t pipe[2] = {0xF0F0F0F0F3LL,0xF0F0F0F0E1LL};
unsigned int serverFound = 0;
uint64_t recievedAddress ;
bool srvrAdd_sent = false;
void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1, pipe[0]);
  radio.startListening(); // put your setup code here, to run once:
  radio.setRetries(0, 0);
  Serial.println((unsigned int)pipe[0]);
    while (! radio.available() ) //wait until radio is available
    {}
    if (radio.available()) {
      radio.read(&recievedAddress, sizeof(&recievedAddress));
      //    Serial.println((unsigned int)recievedAddress);
      if ((unsigned int)recievedAddress == (unsigned int)pipe[0]) {
        serverFound = 1;
      } else {
        serverFound = 3;
      }
      Serial.print((unsigned int)recievedAddress);
      Serial.print(" == ");
      Serial.println((unsigned int)pipe[0]);
      while (!radio.isAckPayloadAvailable()) {
        radio.writeAckPayload(1, &serverFound, sizeof(serverFound));
        Serial.print(serverFound);
      }
    }
}
void loop() {
  // put your main code here, to run repeatedly:

}

void firstMethod() {
  while (1) {
    if ( radio.available() )
    {
      radio.read(&recievedAddress, sizeof(&recievedAddress));
      Serial.println((unsigned int)recievedAddress);
      if (recievedAddress == pipe[0]) {
        Serial.println(1);
        serverFound = 1;
        radio.writeAckPayload(1, &serverFound, sizeof(serverFound));
      } else {
        while (recievedAddress != pipe[0]) {
          if (recievedAddress == pipe[0]) {
            Serial.println(2);
            serverFound = 1;
            radio.writeAckPayload(1, &serverFound, sizeof(serverFound));
            break;
          }
          else if (recievedAddress == pipe[1]) {
            Serial.println(3);
            break;
          }
          else {
            Serial.println(4);
            serverFound = 0;
            radio.writeAckPayload(1, &serverFound, sizeof(serverFound));
          }
          radio.read(&recievedAddress, sizeof(&recievedAddress));
        }
      }
    }
  }
}

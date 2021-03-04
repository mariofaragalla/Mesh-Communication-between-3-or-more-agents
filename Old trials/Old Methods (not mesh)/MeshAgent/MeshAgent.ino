#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 30); // CE, CSN
//Current agent address
const uint64_t pipe[2] = {0xF0F0F0F0F3LL, 0xF0F0F0F0E1LL};
unsigned int serverFound = 0;
uint64_t recievedAddress ;
bool isServer = false;
unsigned int counter[2] = {0, 1};
//unsigned int i = 0;
unsigned int sentData;
void setup() {
  initAgent(1);
  //wait until data is sent from the sever
  while (! radio.available() ) {}
  //if data sent
  if (radio.available()) {
    // then read the address sent
    radio.read(&recievedAddress, sizeof(&recievedAddress));
    // if the address recieved equals the address of this agent
    if ((unsigned int)recievedAddress == (unsigned int)pipe[1]) {
      serverFound = 1; //then a server is found
      isServer = true;
    } else {
      serverFound = 3; //else this is not the server (3 is a dummy number to indicate no server)
      isServer = false;
    }
    Serial.print((unsigned int)recievedAddress);
    Serial.print(" == ");
    Serial.println((unsigned int)pipe[1]);
    //while you didn't recieve a Payload ack
    while (!radio.isAckPayloadAvailable()) {
      //keep sending the status whether a server is found or not
      radio.writeAckPayload(1, &serverFound, sizeof(serverFound));
      Serial.print(serverFound);
    }
  }
}
void loop() {
  // put your main code here, to run repeatedly:

  if (isServer) {
    const uint64_t pipe[3] = {0xF0F0F0F0F3LL, 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL};
    //  int counter[1] = {1};
    unsigned int recievedData ;


    unsigned int counter[3] = {0, 1, 2};
    int i = 0; //ERROR : CHECK HOW TO USE i 
    for (byte node = 0; node < 3; node++) {
      radio.openWritingPipe(pipe[counter[i]]);
      bool tx_sent;
      tx_sent = radio.write(&sentData, sizeof(sentData));
      //    Serial.println(tx_sent);
      if (tx_sent) {
        if (radio.isAckPayloadAvailable()) {
          radio.read(&recievedData, sizeof(recievedData));
          //              ID[0] = counter[0]+1;
        }
        else
        {
          radio.flush_rx();
          radio.flush_tx();
        }
      }
      else
      {
        radio.flush_tx();
      }
    }
    //  Serial.print("ServerRecievedData");
    //  Serial.print(",");
    Serial.print(recievedData);
    Serial.print(" ");
    //  Serial.print("ServerSentData");
    //  Serial.print(",");
    Serial.println(sentData);
  } else {
    unsigned int recievedData ;
    unsigned int sentData = 400;
    if ( radio.available() )
    {
      radio.writeAckPayload(1, &sentData, sizeof(sentData));

      radio.read(&recievedData, sizeof(&recievedData));

    }
  }

}
void initAgent(unsigned int addressSelector) {
  Serial.begin(9600);
  radio.begin();
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1, pipe[addressSelector]);
  radio.startListening();
  radio.setRetries(0, 0);
}
/*
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
  } */

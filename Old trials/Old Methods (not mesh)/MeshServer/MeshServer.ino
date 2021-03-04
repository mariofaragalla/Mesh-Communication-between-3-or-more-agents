#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 10); // CE, CSN
const uint64_t pipe[2] = {0xF0F0F0F0F3LL, 0xF0F0F0F0E1LL};
const unsigned int NUM_OF_AGENTS = 2 ;
const unsigned int NOT_FOUND = 3;
unsigned int counter[2] = {0, 1};
const unsigned int ONE = 1;
//  double locationone;
int i = 0;
unsigned int sentData = 3;
unsigned int serverFlag = 0; //determines if the agent is a server or not

bool isServerFound = false;
bool serverAdd_sent = false;
//uint64_t serverAddress = pipe[0];
void setup() {
  initServer();

  //check if the targeted agent is found (assume that the server will be always found)
  //radio.isAckPayloadAvailable()

  //send server address to the first agent
  sendServerAddress(i);
  //while there is no agent is recognised as a server yet
  //and not all agents are checked yet
  while (serverFlag != 1 && i < NUM_OF_AGENTS) {
    //check if the status of the agent was sent or not
    if (radio.isAckPayloadAvailable()) {
      //read the status of the agent and assign it to the serverFlag
      radio.read(&serverFlag, sizeof(serverFlag));
      //send back any number to acknowledge that the status is arrived
      radio.writeAckPayload(1, &ONE, 1);
      //if the current agent is not the one to be the server
      //and current agent is the first one
      if (serverFlag == NOT_FOUND && i == 0) {
        sendServerAddress(++i);
      }
    }
  }
  Serial.print("serverFlag: ");
  Serial.println(serverFlag);

  /*
    // after detecting the Server a dummy address is sent to the Server
    // to make it stop detecting if it is a Server
    radio.write(&pipe[1], sizeof(pipe[1]));
  */

}

void loop() {
  /*
    const uint64_t pipe[3] = {0xF0F0F0F0F3LL, 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL};
    //  int counter[1] = {1};
    unsigned int recievedData ;


    unsigned int counter[3] = {0, 1, 2};
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
  */
}

void initServer() {
  Serial.begin(9600);
  radio.begin();
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.stopListening();
  // choose the address of the agent that will be the server
  //  radio.openWritingPipe(pipe[0]);
  radio.setRetries(0, 0);
}

//send the sever address (pipe[1]) to the selected agent by i
void sendServerAddress(unsigned int i) {
  //  Serial.println("MAAAAAAAAAAAAAAN");
  //select the agent to which the address will be sent
  radio.openWritingPipe(pipe[counter[i]]);
  Serial.print("open address: ");
  Serial.println((unsigned int)pipe[counter[i]]);
  //when an ack payload is sent from the agent
  while (!radio.isAckPayloadAvailable()) {
    //send the server address to agent
    serverAdd_sent = radio.write(&pipe[1], sizeof(pipe[1]));
    Serial.println((unsigned int)pipe[counter[i]]);
  }
}

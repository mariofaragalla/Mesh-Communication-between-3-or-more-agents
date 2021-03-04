#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 30); // CE, CSN

#define my_ID 1

//Current agent address
const uint64_t pipe[3] = {0xF0F0F0F0F3LL, 0xF0F0F0F0E1LL,0xF0F0F0F0C1LL};
unsigned int serverFound = 0;
uint64_t recievedAddress;
  //unsigned int counter[2] = {0, 1};
unsigned int i = 0;
unsigned int sentData;

struct Payload{
  uint8_t ID;
  int32_t pos;
  uint8_t isNodeActive;
};
Payload payload0to1;
Payload payload1to0;

void setup() {
  initAgent();
  radio.openReadingPipe(1,pipe[1]);
  radio.startListening();
  
  while(payload0to1.ID == 0){
    if (radio.available()) {
      // then read the address sent
      radio.read(&payload0to1, sizeof(&payload0to1));
      // if the address recieved equals the address of this agent
      Serial.print("Recieved: Channel: ");
      Serial.print(payload0to1.ID);
      Serial.print(" , pos: ");
      Serial.println(payload0to1.pos);
      if(payload0to1.ID == my_ID){
        Serial.println("I AM THE LEADER !!! ");
      } else {
            Serial.print("I am node number: ");
            Serial.println(abs(my_ID - payload0to1.ID));
      }
      //while you didn't recieve a Payload ack
    } else{
      Serial.print("Waiting for first signal!!\n");
      }
    i++;
  }

  do{
    radio.stopListening();
    radio.openWritingPipe(pipe[0]);
    
    payload1to0.ID = my_ID;
    payload1to0.pos = 218;
    payload1to0.isNodeActive = 1;
    Serial.println("Nothing is sent");
      while(1){
        radio.stopListening();
        radio.write(&payload1to0, sizeof(&payload1to0));
        Serial.print("Data Sent: ");
        Serial.print(payload1to0.pos);
        Serial.print(" Status: ");
        Serial.println(payload1to0.isNodeActive);
        radio.startListening();
      }
    }while(!radio.write(&payload0to1, sizeof(&payload0to1)));
}

void loop() {

}
void initAgent() {
  Serial.begin(115200);
  radio.begin();
//  radio.setAutoAck(true);
//  radio.enableAckPayload();
//  radio.enableDynamicPayloads();

  radio.setRetries(0, 0);
  payload0to1.ID = 0;
}

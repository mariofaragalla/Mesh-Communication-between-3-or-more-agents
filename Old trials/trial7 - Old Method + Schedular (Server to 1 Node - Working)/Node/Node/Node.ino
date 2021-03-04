#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 30); // CE, CSN

#define my_ID 1

//Current agent address
const uint64_t pipe[2] = {0xF0F0F0F0F3LL, 0xF0F0F0F0E1LL};
unsigned int serverFound = 0;
uint64_t recievedAddress;
  //unsigned int counter[2] = {0, 1};
unsigned int i = 0;
unsigned int sentData;

struct Payload{
  uint8_t ID;
  int16_t pos;
  uint8_t isNodeActive;
};
Payload payload0to1;
Payload payload1to0;

void setup() {
  initAgent();
  
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
      }
      //while you didn't recieve a Payload ack
    } else{
      Serial.print("Nothing was sent\n");
      }
    i++;
  }

  do{
    radio.stopListening();
    radio.openWritingPipe(pipe[1]);
    
    payload1to0.ID = my_ID;
    payload1to0.pos = 218;
    payload1to0.isNodeActive = 1;
    Serial.println("Nothing is sent");
      for(int i =0; i<10; i++){
        radio.write(&payload1to0, sizeof(&payload1to0));
        Serial.print("Data Sent: ");
        Serial.print(payload1to0.pos);
        Serial.print(" Status: ");
        Serial.println(payload1to0.isNodeActive);
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
  radio.openReadingPipe(1,pipe[0]);
  radio.startListening();
  radio.setRetries(0, 0);
  payload0to1.ID = 0;
}

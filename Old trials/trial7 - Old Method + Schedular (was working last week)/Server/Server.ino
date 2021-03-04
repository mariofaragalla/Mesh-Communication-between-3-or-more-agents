#include <TaskScheduler.h>
#include <TaskSchedulerDeclarations.h>
#include <TaskSchedulerSleepMethods.h>

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


#define DURATION 10000 // 
#define TASK1PERIOD 50 // 
#define TASK2PERIOD 50
#define TASK3PERIOD 50

#define LEADER_ID 1

RF24 radio(7, 10); // CE, CSN
const uint64_t pipe[3] = {0xF0F0F0F0F3LL, 0xF0F0F0F0E1LL,0xF0F0F0F0C1LL};
const unsigned int NUM_OF_AGENTS = 2 ;
//const unsigned int NOT_FOUND = 3;
//unsigned int counter[2] = {0, 1};
//const unsigned int ONE = 1;
//  double locationone;
int i = 0;
//unsigned int sentData = 3;
//unsigned int serverFlag = 0; //determines if the agent is a server or not
bool isServerFound = false;
bool serverAdd_sent = false;

Scheduler ts;

void sendTo1();
Task tSendTo1 ( TASK1PERIOD * TASK_MILLISECOND, DURATION / TASK1PERIOD, &sendTo1, &ts, true );
void recieve();
Task tRecieve( TASK1PERIOD * TASK_MILLISECOND, DURATION / TASK2PERIOD, &recieve, &ts, true );
void sendTo2();
Task tSendTo2 ( TASK3PERIOD * TASK_MILLISECOND, DURATION / TASK3PERIOD, &sendTo2, &ts, true );

struct Payload{
  uint8_t ID;
  int32_t pos;
  uint8_t isNodeActive;
};

Payload payload0to1;
Payload payload1to0;
Payload payload0to2;
Payload payload2to0;

//uint64_t serverAddress = pipe[0];
void setup() {
  initServer();
}

  void loop() {
      ts.execute();
  }

  void initServer() {
    Serial.begin(115200);
    radio.begin();
  //  radio.setAutoAck(true);
  //  radio.enableAckPayload();
  //  radio.enableDynamicPayloads();
    radio.stopListening();
    radio.setRetries(0, 0);
    payload0to1.pos = 0;
//    sendTo1();
  }

  void sendTo1(){
     radio.stopListening();
     payload0to1.ID = LEADER_ID ;
     radio.openWritingPipe(pipe[1]);
     
     if (radio.write(&payload0to1, sizeof(payload0to1))){
        Serial.print("Send_to_1: ");
        Serial.print("Channel: ");
        Serial.print(payload0to1.ID);
        Serial.print(" , pos: ");
        Serial.println((int16_t)payload0to1.pos);
     }else {
      Serial.println("Send_to_1: ");
     }
  }

  void recieve(){
     radio.openReadingPipe(1,pipe[0]);
     radio.startListening();  
     radio.read(&payload1to0, sizeof(payload1to0));
     //switch for different payloads
     if(payload1to0.ID>0) {
        payload1to0.isNodeActive = 1;
        Serial.print("Recieve: ");
        Serial.print(payload1to0.pos);
        Serial.print(" from ");
        Serial.print(payload1to0.ID); 
        Serial.print(", Active: ");
        Serial.println(payload1to0.isNodeActive);
//        radio.flush_rx();
      }else {
        Serial.println("Recieve: ");
      }
    
  }


  void sendTo2(){
     radio.stopListening();
     payload0to2.ID = LEADER_ID ;
     radio.openWritingPipe(pipe[2]);
     radio.write(&payload0to2, sizeof(payload0to2));
     if (radio.write(&payload0to2, sizeof(payload0to2))){
     Serial.print("Send_to_2: ");
     Serial.print("Channel: ");
     Serial.print(payload0to2.ID);
     Serial.print(" , pos: ");
     Serial.println(payload0to1.pos);
     }else {
      Serial.println("Send_to_2: ");
     }
  }
  

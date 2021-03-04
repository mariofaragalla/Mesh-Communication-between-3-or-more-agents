//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(7,30);  // CE, CSN

//address through which two modules communicate.
const byte serverAddress[6] = "00000";
const byte node1Address[6] = "00001";
const byte node2Address[6] = "00002";
const byte node3Address[6] = "00003";

uint8_t my_id = 2;

typedef struct {
  int16_t pos;
  uint8_t address;
}data;

data leaderData;


void setup()
{
  while (!Serial);
    Serial.begin(9600);
  
  radio.begin();
  
  //set the address
  radio.openReadingPipe(0, serverAddress);
  
  //Set module as receiver
  radio.startListening();
  
  isLeader(); // first step is know if this node is the leader
}

void loop()
{
  //Read the data if available in buffer
}

void printStruct(data d){
  Serial.print("address: ");
  Serial.print(d.address);
  Serial.print(" pos: ");
  Serial.println(d.pos);
}

void isLeader(){
  leaderData.pos = 0;
  leaderData.address = 0;
  uint8_t new_id = 0;
  while(leaderData.pos == 0){
    if (radio.available()){
      radio.read(&leaderData, sizeof(leaderData));
      printStruct(leaderData);
    }
  } 
  radio.stopListening(); 
  
  if(leaderData.address == my_id){
      Serial.println("I am the leader !!");
  } else{
      Serial.print("I am node: ");
      new_id = abs(my_id - leaderData.address);
      Serial.println(new_id);
  }
  
  radio.openWritingPipe(node2Address);
  leaderData.address = new_id + 1;
  while(!radio.write(&leaderData, sizeof(leaderData))){
    Serial.println("Send Ack");
  }
  radio.startListening(); 
  Serial.println("Confirm that leader was assigned");
}

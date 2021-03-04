//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Network_config.h>
#include <Sync.h>


//create an RF24 object
RF24 radio(7,10);  // CE, CSN

//channel through which two modules communicate.
const byte serverAddress[6] = "00000"; //channel to communicate with the server
const byte node1Address[6] = "00001";  //channel to communicate with node1
const byte node2Address[6] = "00002";  //channel to communicate with node2
const byte node3Address[6] = "00003";  //channel to communicate with node3

//we assume that Server has id of 0

typedef struct { // struct of the data sent containing the address and the position
  int16_t pos;
  uint8_t id;
}data;

data leaderData; // data for assigning the leader
data visualizationData; // data incoming from other nodes

#define LEADER_ID 1       // select which node to be the leader
#define MAX_NODES_NUM 3   // predefine how much node we want to communicate with

  bool node1 = false;             //boolean to check if the server successfully recieved ack from node1
  bool node2 = false;             //boolean to check if the server successfully recieved ack from node2
  bool node3 = false;             //boolean to check if the server successfully recieved ack from node3

void setup()
{
  Serial.begin(115200);
    
  radio.begin();
  
  assignLeader(); // first step is to set which node is the leader
  
  node1, node2 , node3 = false; //reset the recieving flags

  radio.startListening();
//  visualizationData.id = 0;
//  visualizationData.pos = 0;
  leaderData.id = 0;
  radio.powerDown();
  radio.powerUp();
  
}

void loop()
{
  visualizeData();
}

void assignLeader(){    //function to assign the selected node to a leader
  
  //set the address
  radio.openWritingPipe(serverAddress);
  
  //Set module as transmitter
  radio.stopListening();
  
  leaderData.pos = 10101;     //set dummy value for the position
  leaderData.id = LEADER_ID;  //set the id of the leader
  uint8_t sendToAgents = 0;   //set num of trials of sending
  uint8_t dataSent_count = 0; //count how many times data was sent correctly
  bool dataSent = false;          //boolean to check if data was sent correctly
  
  while(sendToAgents < 10){ //set the num of trials to 10
      sendToAgents++;       
      dataSent = radio.write(&leaderData, sizeof(leaderData));  //send data
      Serial.print("Data sent: ");
      Serial.println(dataSent);
      Serial.print("dataSent_count: ");
      Serial.println(dataSent_count);
//      Serial.println(sendToAgents);
      if(dataSent) dataSent_count++;  
      if(dataSent_count >= MAX_NODES_NUM) break;  
  }
  
  leaderData.id = 0;   //reset the leader id
//  radio.openReadingPipe(1,node1Address);  //read from the leader node
  radio.startListening();                 //start reading
  
//  while(node < MAX_NODES_NUM){    
    while(!node1 || !node2 || !node3){ 
        //read data if the connection still available
        if (radio.available()){
          radio.read(&leaderData, sizeof(leaderData));
        }
    
    //check that all nodes send acknowledgement 
        switch(leaderData.id){
            case(1): node1 = true; radio.openReadingPipe(1,node2Address); break;
            case(2): node2 = true; radio.openReadingPipe(1,node3Address); break;
            case(3): node3 = true; break;
        }
        
        Serial.print("Recieved Address: ");
        Serial.println(leaderData.id);
        if(leaderData.id == 3) break;
     } 
  
  Serial.print("node1: ");
  Serial.println(node1);
  Serial.print("node2: ");
  Serial.println(node2);
  Serial.print("node3: ");
  Serial.println(node3);

//  flush_tx();
  radio.stopListening();
  sendToAgents = 0;
  leaderData.id = 100; // arbitrary value to tell other nodes that the server recieved from them
  while(radio.write(&leaderData, sizeof(leaderData))){ //set the num of trials to 10
      sendToAgents++;       
//      dataSent = radio.write(&leaderData, sizeof(leaderData));  //send data
      Serial.println(leaderData.id);
  } 
//  Serial.println(leaderData.id);
}


void visualizeData(){

  int16_t pos1, pos2, pos3 = 0;

//        
//  radio.openReadingPipe(1,node1Address);
  byte address = *node1Address;
  uint8_t i = 0;
//  if(radio.available()){
//    while(leaderData.id == 0){
//      radio.read(&leaderData, sizeof(leaderData));
//      
//  //    if(address == *node1Address){
//  //      address = *node2Address;
//  //    }else if (address == *node2Address){
//  //      address = *node2Address;
//  //    } else {
//  //      address = *node1Address;
//  //    }
//  //  
//     }
//  }
  while(!radio.available());
  radio.read(&leaderData, sizeof(leaderData));
  switch(leaderData.id){
      case(1): node1 = true; pos1 = leaderData.pos; Serial.print(pos1); Serial.print(" "); radio.openReadingPipe(1,node2Address); break;
      case(2): node2 = true; pos2 = leaderData.pos; Serial.println(pos2); Serial.print(" "); radio.openReadingPipe(1,node3Address); break;
      case(3): node3 = true; pos3 = leaderData.pos; Serial.println(pos3); radio.openReadingPipe(1,node1Address);break;
  }

//  delay(200);

//  Serial.print("Recieved id: ");
  Serial.println(leaderData.id);
}

//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

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

#define LEADER_ID 1
#define MAX_NODES_NUM 3   // predefine how much node we want to communicate with

void setup()
{
  Serial.begin(9600);
    
  radio.begin();
  
  assignLeader(); // first step is to set which node is the leader
  
}

void loop()
{
  //Send message to receivers
//  radio.write(&leaderData, sizeof(leaderData));
  
//  delay(10);
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
  bool dataSent = 0;          //boolean to check if data was sent correctly
  bool node1 = 0;             //boolean to check if the server successfully recieved ack from node1
  bool node2 = 0;             //boolean to check if the server successfully recieved ack from node2
  bool node3 = 0;             //boolean to check if the server successfully recieved ack from node3
  
  while(sendToAgents < 10){ //set the num of trials to 100
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
  radio.openReadingPipe(1,node1Address);  //read from the leader node
  radio.startListening();                 //start reading
  
  uint8_t node = 0; // denotes which node did we recieve from
  
//  while(node < MAX_NODES_NUM){    
    while(!node1 || !node2 || !node3){ 
        //read data if the connection still available
        if (radio.available()){
          radio.read(&leaderData, sizeof(leaderData));
        }
        Serial.print("Recieved Address: ");
        Serial.println(leaderData.id);
    
    //check that all nodes send acknowledgement 
        switch(leaderData.id){
            case(1): node1 = true; node = 1; radio.openReadingPipe(1,node2Address); delay(10); break;
            case(2): node2 = true; node = 2; radio.openReadingPipe(1,node3Address); delay(10); break;
            case(3): node3 = true; node = 3; break;
        }
     } 
  
  Serial.print("node1: ");
  Serial.println(node1);
  Serial.print("node2: ");
  Serial.println(node2);
  Serial.print("node3: ");
  Serial.println(node3);
}

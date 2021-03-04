//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(7,30);  // CE, CSN 

//channel through which two modules communicate.
const byte serverAddress[6] = "00000"; //channel to communicate with the server
const byte node1Address[6] = "00001";  //channel to communicate with node1
const byte node2Address[6] = "00002";  //channel to communicate with node2
const byte node3Address[6] = "00003";  //channel to communicate with node3

uint8_t my_id = 1;   // initial id of this node
bool leader = false; //set this node not to be the leader
uint8_t trials = 0;
bool ackSent = false;

typedef struct { // struct of the data sent containing the address and the position
  int16_t pos;
  uint8_t id;
}data;

data leaderData; // data for assigning the leader
data sentData;  // data to be sent
data recievedData; // data that is recieved


void setup()
{
  while (!Serial);
    Serial.begin(115200);
  
  radio.begin();

  isLeader(); // first step is know if this node is the leader
//  delay(1000); // to wait for the server until it recieves acknowledgement from all the other nodes
  sentData.id = my_id;
  sentData.pos = random(-1000 , 1000); 
//  radio.openWritingPipe(node1Address);
  radio.powerDown();
  radio.powerUp();
  delay(5);
  radio.stopListening();
  radio.openWritingPipe(node1Address);
}

void loop()
{   
//    if(!ackSent) for(trials = 0; trials < 10 && !ackSent; trials++, ackSent = radio.write(&sentData, sizeof(sentData)));
    ackSent = radio.write(&sentData, sizeof(sentData));
//    if(!ackSent) { delay(5); radio.write(&sentData, sizeof(sentData)); } // try ten times if it didn't work then wait and do another trials
    Serial.println(ackSent);
    ackSent =false;
    Serial.println(sentData.id);
    
//    trials++;
//    radio.write(&sentData, sizeof(sentData));
//    Serial.println(sentData.id);
//  delay(10);
}

void printStruct(data d){ // a function to print the Struct
  Serial.print("id: ");
  Serial.print(d.id);
  Serial.print(" pos: ");
  Serial.println(d.pos);
}

bool isLeader(){  // this funciton is to decide if this node is the leader or not (decided by the server)
  
  // open pipe 0 with server channel
  radio.openReadingPipe(0, serverAddress); 
  
  //Set module as receiver
  radio.startListening();
  
  //set the leader data to the default value
  leaderData.pos = 0;     
  leaderData.id = 0;

  //keep reading until the data is changed (this node recieves any data)
  while(leaderData.id == 0){ 
    if (radio.available()){
      radio.read(&leaderData, sizeof(leaderData));
      printStruct(leaderData);
    }
  }
  
  // When this node recieves the data. the node should stop listening/reading 
  // then check if the incoming leader id is the id of this node
  
  radio.stopListening(); 
  if(leaderData.id == my_id){          //if they are equal then this node is the leader
      leader = true;
      Serial.println("I am the leader !!");
  } else{                                   // else the node should redefine its id 
      Serial.print("I am node: ");
      my_id = abs(my_id - leaderData.id);
      Serial.println(++my_id);
  }
  
  // after checking whether this node is leader or not, we should send back to the server
  // the id, so that it makes sure that all nodes recieved the message and one node is 
  // assigned to the leader
  
  radio.openWritingPipe(node1Address);
  leaderData.id = my_id;
  ackSent = false;
//  delay(1);
  do{
    for(trials = 0; trials < 10 && !ackSent; trials++, ackSent = radio.write(&leaderData, sizeof(leaderData))) ;
    }while(!ackSent);

    Serial.println(trials);

//  radio.flush_rx();  
  radio.startListening(); 
  while(leaderData.id != 100){ // wait for the Server to confirm
    radio.read(&leaderData, sizeof(leaderData)); 
  }
  ackSent = false;
  Serial.println(leaderData.id);
  Serial.println("Confirm that leader was assigned");
  
  return leader;
}

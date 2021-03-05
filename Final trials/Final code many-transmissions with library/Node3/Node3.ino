//Include Libraries
#include <SPI.h>
//#include <nRF24L01.h>
#include <RF24.h>
#include <SimpleMeshLib.h>

//channel through which two modules communicate.
byte address[7]={'0','1','2','3','4','5','6'};

#define COMADR 1  // choose the desired communication address 

// defining the id's of all communicating nodes
#define SERVER 0
#define LEADER 1 
#define NODE2 2
#define NODE3 3

#define BAUDRATE 115200
#define MESSAGE_NUM 5 // number of messages to be sent in one packet
#define NODES_NUM 3 // total number of nodes that are communicating


#define NODES_NUM_1 2  // no. of nodes - 1 

//create an RF24 object
RF24 radio(7,30);  // CE, CSN 
SimpleMesh  SimpleMesh(true);

bool rx_1st_time = true;  // check if this node is recieving for the first time 
uint8_t trials = 0; // used if the transmission is intended to be run for a number of trials instead of time

SimpleMesh::data_t sentData;  // data to be sent
SimpleMesh::data_t recievedData; // data that is recieved

void setup() {
  
  Serial.begin(BAUDRATE);
  
  radio.begin();  // initialize the nrf24 module 
  
  // At the very beginning the leader sends one message to the other nodes to indicate that all agents should start the data transmission scheme
  // so each node as this one should recieve this message before it starts the data transmission scheme

  radio.openReadingPipe(1,address[COMADR]); // choose the common pipeline (COMADR) to read from
  radio.startListening(); // prepare the nrf24 module to read/recieve 

//     recievedData.time_t = 0;
     
  
  Serial.println("Start recieving");
  
   while(!radio.available()){ // Test if there are data available to be read.
      while(recievedData.id == 0){  //  keep recieving until a message containing any id other than 0 is recieved
        radio.read(&recievedData, sizeof(recievedData));  // read/recieve the data
    }
  }
  Serial.println(recievedData.id);

  SimpleMesh.StartFastTimer(); // start the timer
}

void loop() {
  //Recieve Data from other Nodes

//  if(trials < 10){ // to stop after a certain number of trials uncomment this line and comment the following one

    if (SimpleMesh.FastTimer()/1000000 <= 300){  // stop after 5 mins = 18000 seconds 
      
      // Apply the data Transmission scheme:
      if(rx_1st_time){  //if the node is recieving for the 1st time
        
        // According to the transmission scheme if the node is recieving for the 1st time it should recieve its node num (node 2) - 1 times
         SimpleMesh.recievePacket(MESSAGE_NUM, NODE3-1); 
         
        rx_1st_time = false;
        
     } else{
      
        SimpleMesh.recievePacket(MESSAGE_NUM, NODES_NUM-1);  // if it is not the 1st time then recieve for # of nodes - 1 times
        
      }
      
      radio.flush_rx();  // Empty the receive buffer after recieving to avoid overload
      
      SimpleMesh.sendPacket(MESSAGE_NUM, NODE2); // send a packet using node 2 id
     
      radio.flush_tx(); // Empty the transmit buffer after sending to avoid overload

//      trials++;   // to stop after a certain number of trials uncomment this line
      
    }
    
}

//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SimpleMeshLib.h>

//channels through which two modules can communicate.
byte address[7]={'0','1','2','3','4','5','6'};

#define COMADR 1 // choose the desired communication address 


// defining the id's of all communicating nodes
#define SERVER 0
#define LEADER 1 
#define NODE2 2
#define NODE3 3

#define BAUDRATE 115200
#define MESSAGE_NUM 5 // number of messages to be sent in one packet
#define NODES_NUM 3   // total number of nodes that are communicating

//uint8_t trials = 0;

uint8_t trials = 0; // used if the transmission is intended to be run for a number of trials instead of time

RF24 radio(7,30);  // CE, CSN 

SimpleMesh  SimpleMesh(true);

SimpleMesh::data_t sentData;  // data to be sent
SimpleMesh::data_t recievedData; // data that is recieved

void setup()
{
  Serial.begin(BAUDRATE);

  while (!Serial);
    Serial.begin(BAUDRATE);
  
  radio.begin();  // initialize the nrf24 module 
  
  // At the very beginning the leader sends one message to the other nodes to indicate that all agents should start the data transmission scheme
  radio.stopListening(); // prepare the nrf24 module to send/write 
  radio.openWritingPipe(address[COMADR]); // choose the address on which the data is sent
  SimpleMesh.StartFastTimer(); // start the timer
//  sentData.time_t = FastTimer()/1000;

  // update data to be sent
  sentData.pos = 2137;   // arbitary data
  sentData.id = LEADER;
  sentData.count= 0;
  
  radio.write(&sentData, sizeof(sentData)); // send the very first message
  Serial.println("Starting timer"); 
  Serial.println("Sending data to other nodes and server");

}

void loop()
{   
//    if(trials < 10){    // to stop after certain number of trials uncomment this line and comment the following one
    
    if (SimpleMesh.FastTimer()/1000000 <= 300){  // stop after 5 mins = 300 seconds 
      //Send messages to other Nodes
    
       SimpleMesh.sendPacket(MESSAGE_NUM, LEADER); // send a packet using the leader (node 1) id
    
       radio.flush_tx();  // Empty the transmit buffer after sending to avoid overload
        
       SimpleMesh.recievePacket(MESSAGE_NUM,NODES_NUM -1);  // recieve a packet on the common pipeline
      
       radio.flush_rx();  // Empty the receive buffer after recieving to avoid overload
  
//       trials++;   // to stop after certain number of trials uncomment this line

    }
     
//     timerDelay(DEL);

}

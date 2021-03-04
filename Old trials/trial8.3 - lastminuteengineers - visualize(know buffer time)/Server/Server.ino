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
byte address[7]={'0','1','2','3','4','5','6'};
#define SERVER 0
#define LEADER 1
#define NODE2 2
#define NODE3 3
#define NODE4 4
#define NODE5 5
#define NODE6 6

#define BAUDRATE 115200

//we assume that Server has id of 0

typedef struct { // struct of the data sent containing the address and the position
  int16_t pos;
  uint8_t id;
}data;

typedef struct { // struct of the data sent containing the address and the position
  uint32_t time_t;
  int16_t pos;
  uint8_t id;
}data_t;


data_t leaderData; // data_t for assigning the leader
data_t visualizationData; // data incoming from other nodes

#define MAX_NODES_NUM 3   // predefine how much node we want to communicate with

uint16_t milli_sec;
uint16_t my_time;



void setup()
{
  Serial.begin(BAUDRATE);
    
  radio.begin();
  
  assignLeader(); // first step is to set which node is the leader
  
  visualizationData.time_t = 0;
  visualizationData.pos = 0;
  visualizationData.id = 0;
  timer();
  radio.startListening();
  while(visualizationData.id != LEADER){
      radio.read(&visualizationData, sizeof(visualizationData));
    }
  milli_sec = visualizationData.time_t;
}

void loop()
{
  if(milli_sec % 50 == 0){
     radio.read(&visualizationData, sizeof(visualizationData));
     printStruct(visualizationData);
  }
}

void assignLeader(){    //function to assign the selected node to a leader
  //set the address
  radio.openWritingPipe(address[SERVER]);
  
  //Set module as transmitter
  radio.stopListening();
  
  leaderData.pos = 10101;     //set dummy value for the position
  leaderData.id = LEADER;  //set the id of the leader
  uint8_t trials = 0;   //set num of trials of sending
  uint8_t dataSent_count = 0; //count how many times data was sent correctly
  bool dataSent = false;          //boolean to check if data was sent correctly
  
  while(!radio.write(&leaderData, sizeof(leaderData)));
  Serial.println("Sent id");
  
  leaderData.id = 0;   //reset the leader id
  radio.openReadingPipe(1,address[LEADER]);  //read from the leader node
  radio.startListening();                 //start reading
  
//  while(node < MAX_NODES_NUM){    
    while(leaderData.id == 0){ 
        //read data if the connection still available
//          Serial.println(leaderData.id);
        if (radio.available()){
          radio.read(&leaderData, sizeof(leaderData));
          printStruct(leaderData);
        }
    }

      Serial.print("Recieved Address: ");
      Serial.println(leaderData.id);
    /*
    //check that all nodes send acknowledgement 
        switch(leaderData.id){
            case(1): node1 = true; radio.openReadingPipe(1,node2Address); break;
            case(2): node2 = true; radio.openReadingPipe(1,node3Address); break;
            case(3): node3 = true; break;
        }
     */


  radio.stopListening();
  trials = 0;
  leaderData.id = 100; // arbitrary value to tell other nodes that the server recieved from them
  while(radio.write(&leaderData, sizeof(leaderData)));
  Serial.println("Ack sent");
  
//      trials++;       
//      dataSent = radio.write(&leaderData, sizeof(leaderData));  //send data 
//  Serial.println(leaderData.id);
//  Serial.println(milli_sec * 10);
}
void printStruct(data_t d){ // a function to print the Struct
  Serial.print("time: ");
  Serial.print(d.time_t);
  Serial.print(" id: ");
  Serial.print(d.id);
  Serial.print(" pos: ");
  Serial.println(d.pos);
}


// timer1 implementation
void timer(){
  cli();
  TCCR1A = 0;     // set entire TCCR5A register to 0
  TCCR1B = 0;     // same for TCCR5B
  TCNT1 = 0;
  OCR1A = 624;            // compare match register 16MHz/256/100Hz(10 ms) // https://timer-interrupt-calculator.simsso.de/

  TCCR1B |= (1 << WGM12);   // CTC mode

  TCCR1B |= (1 << CS12);    // 256 prescaler 

  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

  sei();             // enable all interrupts
  milli_sec = 0;
}

 ISR(TIMER1_COMPA_vect){          // timer compare interrupt service routine
   
   milli_sec++;  // one increment = 10ms // 100 = 1 sec
    }

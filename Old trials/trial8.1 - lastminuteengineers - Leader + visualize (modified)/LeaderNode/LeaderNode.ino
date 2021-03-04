//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(7,30);  // CE, CSN 

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

uint8_t my_id = LEADER;   // initial id of this node
bool leader = false; //set this node not to be the leader
uint8_t trials = 0;
bool ackSent = false;
uint16_t milli_sec = 0;

typedef struct { // struct of the data sent containing the address and the position
  int16_t pos;
  uint8_t id;
}data;

typedef struct { // struct of the data sent containing the address and the position
  uint32_t time_t;
  int16_t pos;
  uint8_t id;
}data_t;

data_t leaderData; // data for assigning the leader
data_t sentData;  // data to be sent
data_t recievedData; // data that is recieved

uint16_t my_time = 0;
uint16_t time1 = 0;
uint16_t time2 = 0;
void setup()
{
  Serial.begin(BAUDRATE);

  while (!Serial);
    Serial.begin(BAUDRATE);
  
  radio.begin();

//  isLeader(); // first step is know if this node is the leader
  
  radio.stopListening();
  radio.openWritingPipe(address[LEADER]);
  sentData.time_t = milli_sec;
  sentData.pos = 2137;
  sentData.id = LEADER;
//  radio.write(&sentData, sizeof(sentData));
  Serial.println("Starting timer");
  Serial.println("Sending data to other nodes and server");
  timer();
}

void loop()
{  
   if(milli_sec % 50 == 0){   // LEADER sends @ 50
      radio.stopListening();
      radio.openWritingPipe(address[LEADER]);
      sentData.time_t = milli_sec;
      sentData.pos = 2312;
      sentData.id = 1;
      radio.write(&sentData, sizeof(sentData));
    }
//      printStruct(sentData);

   if(milli_sec % 70 == 0){ // NODE2 sends @ 70
      radio.startListening();
      radio.openReadingPipe(1,address[NODE2]);
      radio.read(&recievedData, sizeof(recievedData));
   }
   if(recievedData.id == NODE2) printStruct(recievedData);

   if(milli_sec % 90 == 0){  // NODE3 sends @ 90
      radio.openReadingPipe(2,address[NODE3]);  //read from the leader node
      radio.startListening();
      radio.read(&recievedData, sizeof(recievedData));
   }
    if(recievedData.id == NODE3) printStruct(recievedData);
   
   
  /*
//    if(!ackSent) for(trials = 0; trials < 10 && !ackSent; trials++, ackSent = radio.write(&sentData, sizeof(sentData)));
    ackSent = radio.write(&sentData, sizeof(sentData));
//    if(!ackSent) { delay(5); radio.write(&sentData, sizeof(sentData)); } // try ten times if it didn't work then wait and do another trials
    Serial.println(ackSent);
    ackSent =false;
    Serial.println(sentData.id);
    */
}

void printStruct(data_t d){ // a function to print the Struct
  Serial.print("time: ");
  Serial.print(d.time_t);
  Serial.print(" id: ");
  Serial.print(d.id);
  Serial.print(" pos: ");
  Serial.println(d.pos);
}

void setData(data_t d ,uint32_t time_t,int16_t pos,uint8_t id){
  d.time_t = time_t;
  d.pos = pos;
  d.id =id;
}

bool isLeader(){  // this funciton is to decide if this node is the leader or not (decided by the server)
  
  // open pipe 0 with server channel
  radio.openReadingPipe(0, address[SERVER]); 
  
  //Set module as receiver
  radio.startListening();
  
  //set the leader data to the default value
  leaderData.time_t = 0;
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
  
  radio.openWritingPipe(address[LEADER]);
  leaderData.id = my_id;
  ackSent = false;
  leaderData.time_t = milli_sec;
//  delay(1);
  do{
    for(trials = 0; trials < 100 && !ackSent; trials++, ackSent = radio.write(&leaderData, sizeof(leaderData))) ;
    Serial.println("Send");
    }while(!ackSent);

    Serial.println(trials);
 
  radio.startListening(); 
  while(leaderData.id != 100){ // wait for the Server to confirm
    radio.read(&leaderData, sizeof(leaderData)); 
  }
  ackSent = false;
  Serial.println(leaderData.id);
  Serial.println("Confirm that leader was assigned");
  
  return leader;
}

void timer(){
  cli();
  TCCR5A = 0;     // set entire TCCR5A register to 0
  TCCR5B = 0;     // same for TCCR5B
  TCNT5 = 0;
  OCR5A = 624;            // compare match register 16MHz/256/100Hz(10 ms) // https://timer-interrupt-calculator.simsso.de/

  TCCR5B |= (1 << WGM52);   // CTC mode

  TCCR5B |= (1 << CS52);    // 256 prescaler 

  TIMSK5 |= (1 << OCIE5A);  // enable timer compare interrupt

  sei();             // enable all interrupts
  milli_sec = 0;
}

 ISR(TIMER5_COMPA_vect){          // timer compare interrupt service routine
   
   milli_sec++;  // one increment = 10ms
  }

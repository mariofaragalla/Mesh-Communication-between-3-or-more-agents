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
uint32_t micro_sec = 0;

bool isChanged = false;
uint8_t curr_id = 0;

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
uint32_t time1 = 0;
uint16_t time2 = 0;

bool interruptflag_irq = false;
bool readFirst = false; 
//bool tx_ok = 1;
//bool tx_fail = 0;
//bool rx_ready = 1;

void setup()
{
  Serial.begin(BAUDRATE);

  while (!Serial);
    Serial.begin(BAUDRATE);
  
  radio.begin();
//  radio.maskIRQ(1,1,0);
//  attachInterrupt(digitalPinToInterrupt(3), interruptIRQFunction, FALLING);

  
  radio.stopListening();
  radio.openWritingPipe(address[LEADER]);
  sentData.time_t = micro_sec;
  sentData.pos = 2137;
  sentData.id = LEADER;
  
//  recievedData.id = 500; //default value rather than 0
  
//  radio.write(&sentData, sizeof(sentData));
  Serial.println("Starting timer");
  Serial.println("Sending data to other nodes and server");
  timer();
}

void loop()
{ 
          
          radio.stopListening();
          radio.openWritingPipe(address[LEADER]);
//          sentData.time_t = micro_sec;
          sentData.pos = random(10,500);
          sentData.id = 1;
          micro_sec=0;
          radio.write(&sentData, sizeof(sentData));
          sentData.time_t = micro_sec;
          time1 = micro_sec*4;
          Serial.println(time1);
//          if(time1 < 5800){
//            Serial.println(1);
//          }else{ 
//            Serial.println(0);
//          }
//          radio.flush_tx();

/*
       if(micro_sec % 11 == 0){ // NODE2 sends @ 70
          radio.openReadingPipe(1,address[NODE2]);
          radio.startListening();
          radio.read(&recievedData, sizeof(recievedData));
       }
      
       if(micro_sec % 21 == 0){  // NODE3 sends @ 90
          radio.openReadingPipe(2,address[NODE3]);
          radio.startListening();
          radio.read(&recievedData, sizeof(recievedData));
       }
       if(interruptflag_irq){
          radio.read(&recievedData, sizeof(recievedData));
          printStruct(recievedData);
          interruptflag_irq = false;
//          radio.whatHappened(tx_ok,tx_fail,rx_ready);
//          Serial.print(tx_ok);
//          Serial.print(tx_fail);
//          Serial.println(rx_ready);
          radio.flush_rx();
       }
       
//       radio.flush_rx();
*/
}

void interruptIRQFunction(){
//  Serial.print("flag: ");
//  Serial.println(interruptflag_irq);
    interruptflag_irq = true;
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
void timer(){
  cli();
  TCCR5A = 0;     // set entire TCCR5A register to 0
  TCCR5B = 0;     // same for TCCR5B
  TCNT5 = 0;

  // using 8 prescaling (some errors: works with 4 microseconds instead of 1 microsecond)
  OCR5A = 1;            // compare match register (16MHz/(8 prescaler * 1000000(interrupt frequency))- 1) // 1 microsec = 1000000 Hz // https://timer-interrupt-calculator.simsso.de/

  TCCR5B |= (1 << WGM52);   // CTC mode

  TCCR5B &= ~(1 << CS50);
  TCCR5B |= (1 << CS51);    // clkI/O /8 prescaler 
  TCCR5B &= ~(1 << CS52);
  
  TIMSK5 |= (1 << OCIE5A);  // enable timer compare interrupt

  sei();             // enable all interrupts
  micro_sec = 0;
}

 ISR(TIMER5_COMPA_vect){          // timer compare interrupt service routine
   micro_sec++;  // one increment = 1 microsec
}
   

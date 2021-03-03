//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define BAUDRATE 115200 
#define MESSAGE_NUM 5  // number of messages to be sent in one packet
#define NODES_NUM 3 // total number of nodes that are communicating Leader + node2 & node3
#define DELAY 150 // 150 milliseconds

typedef struct { // struct of the data sent containing the address and the position
//  unsigned long time_t;
  int16_t pos;  // the position (or actual data that should be sent)
  uint16_t count; // counter to track each message
  int16_t checkSum; // checksum to validate the correctness of sent/recieved data
  uint8_t id; // the address from which node/to which node data is recieved or sent 
}data_t;

// 3 variables to capture the position of each node (data logging)
int16_t pos1 = 0; // for recieving leader (node 1) data
int16_t pos2 = 0; // for recieving node 2 data
int16_t pos3 = 0; // for recieving node 3 data

bool flag = false;  // flag to stop recieving after a certain number of trials
bool send_f = true; // flag to enable/disable sending
bool recieve_f = true;  // flag to enable/disable recieving

uint8_t recieve_count =0; // count the total number of messages recieved

unsigned long prev_time_t = 0;  // time of the last message sent/recieved
unsigned long current_time_t = 0; // the current time 

RF24 radio(7, 10); // CE, CSN

//channel through which two modules communicate.
byte address[7]={'0','1','2','3','4','5','6'};

#define COMADR 1  // choose the desired communication address 

// defining the id's of all communicating nodes
#define SERVER 0
#define LEADER 1 
#define NODE2 2
#define NODE3 3


data_t recievedData; // data that is recieved


void setup() {
   Serial.begin(BAUDRATE);
  
  radio.begin();
  
  radio.openReadingPipe(1,address[COMADR]);  // read from the common pipeline
  radio.startListening(); // prepare the nrf24 module to read/recieve 

// declare the variables in which data is recieved
//     recievedData.time_t = 0;
     recievedData.pos = 0;
     recievedData.count = 0;
     recievedData.id = 0;
  
  

  
  while(!radio.available()){  // Test if there are data available to be read.
      while(recievedData.id == 0){   //  keep recieving until a message containing any id other than 0 is recieved
        radio.read(&recievedData, sizeof(recievedData));  // read/recieve the data
    }
  }
  Serial.println(recievedData.id);
// delay(1000);
  StartFastTimer(); // start the timer
}

void loop() {
 if(recieve_f){
      radio.openReadingPipe(1,address[LEADER]);  // read from the leader node
      radio.startListening(); // prepare the nrf24 module to read/recieve 
      timerDelay(DELAY);  // delay for 150 ms
      radio.read(&recievedData, sizeof(recievedData));  // read/recieve the data/message
       
       // calculate the checkSum of the recieved data and compare it with the recieved checkSum to check if data is correct 
       // Also check if the id is not zero (in case the checkSum is zero by any chance)
      if (recievedData.checkSum == checkSum(recievedData.pos,recievedData.count,recievedData.id) && recievedData.id > 0){
//          printStruct(recievedData);
          // if the data is correct check from which node is it being sent
          switch(recievedData.id){
            case(LEADER):pos1=recievedData.pos; break;
            case(NODE2):pos2=recievedData.pos; break;
            case(NODE3):pos3=recievedData.pos;  break;
          } 
          // Open Serial plotter to see a graphical representation of all the data being sent/recieved between the 3 nodes
          Serial.print(pos1);
          Serial.print(",");
          Serial.print(pos2);
          Serial.print(",");
          Serial.print(pos3);
          Serial.println("");   
      }
/* For testing with a certain number of trials
    if (recieve_count >= MESSAGE_NUM * NODES_NUM * 9){
      flag= true;
      recieve_f = false;
    }
      recieve_count++;
      
 }
 */
//   if (FastTimer()/1000000 >= 300){ // stop after 5 mins = 300 seconds
//      recieve_f = false;
//   }
 }
}


void printStruct(data_t d){ // a function to print the Struct

  Serial.print(" id: ");
  Serial.print(d.id);
  Serial.print(" pos: ");
  Serial.print(d.pos);
  Serial.print(" count: ");
  Serial.print(d.count);
  Serial.print(" checkSum: ");
  Serial.println(d.checkSum);
  
}

int16_t checkSum(int16_t id, int16_t pos , int16_t count){
      int16_t sum = id + pos + count;
      return sum/2;
}

void timerDelay(uint16_t delay_t){
  
   prev_time_t = current_time_t;
   current_time_t = FastTimer()/1000;
   while(current_time_t - prev_time_t < delay_t ){
         current_time_t = FastTimer()/1000;
   }
  
}
void StartFastTimer() {
  noInterrupts ();  // protected code
  // reset Timer 1
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  TIMSK1 = 0;

  TIFR1 = _BV(TOV1); // clear Overflow Flag so we don't get a bogus interrupt
  TIMSK1 = _BV(TOIE1); // Enable Timer 1 Overflow Interrupt
  TCCR1B = _BV(CS10); // start Timer 1, no prescaler

  interrupts ();
}

volatile uint16_t Overflows = 0;

ISR(TIMER1_OVF_vect){ // interrupts each 0.0625 us seconds
  Overflows++;
}

unsigned long FastTimer() {
  unsigned long currentTime;
  uint16_t overflows;
  
  noInterrupts();
  overflows = Overflows;  // Make a local copy


  // If an overflow happened but has not been handled yet
  // and the timer count was close to zero, count the
  // overflow as part of this time.
  if ((TIFR1 & _BV(TOV1)) && (ICR1 < 1024))
    overflows++;

  currentTime = overflows; // Upper 16 bits
  currentTime = (currentTime << 16) | ICR1;
  interrupts();
  
  return currentTime;
}

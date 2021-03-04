//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define BAUDRATE 115200

typedef struct { // struct of the data sent containing the address and the position
  uint32_t time_t;
  int16_t pos;
  uint8_t id;
}data_t;

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

data_t sentData;  // data to be sent
data_t recievedData; // data that is recieved

uint16_t milli_sec = 0;
uint16_t time1 =0;
uint16_t time2 =0;

unsigned long my_time = 0;

void setup() {
  
  Serial.begin(BAUDRATE);
  
  radio.begin();
  
  radio.openReadingPipe(1,address[LEADER]);  //read from the leader node
  radio.startListening();

     recievedData.time_t = 0;
     recievedData.pos = 0;
     recievedData.id = 0;
  
  Serial.println("Start recieving");
  
  while(!radio.available()){
//    while(recievedData.id == 0 || recievedData.id == 100) { with server
      while(recievedData.id == 0){
        radio.read(&recievedData, sizeof(recievedData));
        Serial.println(recievedData.id);
    }
  }
  timer();
  milli_sec = recievedData.time_t;    //set local time for this node with the global time from the Leader node
  Serial.println(recievedData.time_t);
}

void loop() {
 if(milli_sec % 6 == 0){   // LEADER sends @ 50
//      if(recievedData.id == 0 || recievedData.id == NODE2){
        radio.openReadingPipe(1,address[LEADER]);  //read from the leader node
        radio.startListening();
        radio.read(&recievedData, sizeof(recievedData));
      }
      
//    printStruct(recievedData);
    
  if(milli_sec % 11 == 0){  // NODE2 sends @ 70
      radio.openReadingPipe(1,address[NODE2]);  //read from the leader node
      radio.startListening();
      radio.read(&recievedData, sizeof(recievedData));
    }
   

  if(milli_sec % 20 == 0){   // NODE3 sends @ 70
      radio.stopListening();
      radio.openWritingPipe(address[NODE3]);
      sentData.time_t = milli_sec;
      sentData.pos = random(10,500);
      sentData.id = NODE3;
      radio.write(&sentData, sizeof(sentData));
    }

     printStruct(recievedData);
//    printStruct(sentData);
    
}

void printStruct(data_t d){ // a function to print the Struct
  Serial.print("time: ");
  Serial.print(d.time_t);
  Serial.print(" id: ");
  Serial.print(d.id);
  Serial.print(" pos: ");
  Serial.println(d.pos);
}

void setData(data_t d ,uint32_t t,int16_t p,uint8_t i){
  d.time_t = t;
  d.pos = p;
  d.id =i;
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

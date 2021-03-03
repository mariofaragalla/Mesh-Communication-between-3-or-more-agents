//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define BAUDRATE 115200

typedef struct { // struct of the data sent containing the address and the position
//  unsigned long time_t;
  int16_t pos;
  uint16_t count;
  uint8_t id;
}data_t;

uint8_t byte_data = 0;
uint8_t data_array[255] ;

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

//  radio.flush_rx();
  
  radio.openReadingPipe(1,address[LEADER]);  //read from the leader node
  radio.startListening();

//     recievedData.time_t = 0;
     recievedData.pos = 0;
     recievedData.count = 0;
     recievedData.id = 0;
  
  Serial.println("Start recieving");
  
  while(!radio.available()){
//    while(recievedData.id == 0 || recievedData.id == 100) { with server
      while(recievedData.id == 0){
        radio.read(&recievedData, sizeof(recievedData));
        Serial.println(recievedData.id);
    }
  }
//  timer();
//  milli_sec = recievedData.time_t;    //set local time for this node with the global time from the Leader node
//  Serial.println(recievedData.time_t);
}

void loop() {
 
// if(milli_sec % 10 == 0 ){   // LEADER sends @ 10
//  if(recievedData.count < 200){
    if(byte_data < 254){
      radio.openReadingPipe(1,address[LEADER]);  //read from the leader node
      radio.startListening();
//      radio.read(&recievedData, sizeof(recievedData));
//      syncTime(recievedData.time_t);
//      printStruct(recievedData);
      delay(8);
      if(radio.available()){
        radio.read(&byte_data, sizeof(byte_data));
      } else {
          radio.read(&byte_data, sizeof(byte_data));
        }
//      Serial.println(byte_data);
      if(byte_data != 0) {
          data_array[byte_data] = byte_data;
        } else{
            delay(1);
        }
  }

//    radio.flush_rx();
//   }
//    printStruct(recievedData);

     if(byte_data == 254){
          Serial.println("data: ");
          for(int i =1; i <= 254; i++){
            Serial.println(data_array[i]);
    //          Serial.print(" ");
          }
          byte_data++;
        }

 /*
  if(milli_sec % 20 == 0){  // NODE2 sends @ 70
      radio.stopListening();
      radio.openWritingPipe(address[NODE2]);
      sentData.time_t = milli_sec;
      sentData.pos = 3243;
      sentData.id = NODE2;
      radio.write(&sentData, sizeof(sentData));
    }
//    printStruct(sentData);
    
  if(milli_sec % 30 == 0){  // NODE3 sends @ 90
      radio.openReadingPipe(2,address[NODE3]);  //read from the leader node
      radio.startListening();
      radio.read(&recievedData, sizeof(recievedData));
   }
    printStruct(recievedData);
 */
}

void printStruct(data_t d){ // a function to print the Struct
//  Serial.print("time: ");
//  Serial.print(d.time_t);
  Serial.print(" id: ");
  Serial.print(d.id);
  Serial.print(" pos: ");
  Serial.print(d.pos);
  Serial.print(" count: ");
  Serial.println(d.count);
}

void syncTime(uint32_t globalTime){
  if(globalTime != 0){
    milli_sec = globalTime;
  }
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

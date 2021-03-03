//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define BAUDRATE 115200 
#define MESSAGE_NUM 5
#define NODES_NUM 3
#define DELAY 150

typedef struct { // struct of the data sent containing the address and the position
//  unsigned long time_t;
  int16_t pos;
  uint16_t count;
  int16_t checkSum;
  uint8_t id;
}data_t;

int16_t pos_arr[250];
uint16_t count_arr[250];
uint8_t id_arr[250];

bool flag = false;
bool send_f = true;
bool recieve_f = true;

uint8_t recieve_count =0;

unsigned long prev_time_t = 0;
unsigned long current_time_t = 0;

RF24 radio(7, 10); // CE, CSN

//channel through which two modules communicate.
byte address[7]={'0','1','2','3','4','5','6'};
#define SERVER 0
#define LEADER 1
#define NODE2 2
#define NODE3 3
#define NODE4 4
#define NODE5 5
#define NODE6 6


data_t recievedData; // data that is recieved



void setup() {
   Serial.begin(BAUDRATE);
  
  radio.begin();
  
  radio.openReadingPipe(1,address[LEADER]);  //read from the leader node
  radio.startListening();

//     recievedData.time_t = 0;
     recievedData.pos = 0;
     recievedData.count = 0;
     recievedData.id = 0;
  
  Serial.println("Start recieving");
  
  while(!radio.available()){
      while(recievedData.id == 0){
        radio.read(&recievedData, sizeof(recievedData));
    }
  }
  Serial.println(recievedData.id);
// delay(1000);
  StartFastTimer();
}

void loop() {
 if(recieve_f){
      radio.openReadingPipe(1,address[LEADER]);  //read from the leader node
      radio.startListening();
      timerDelay(DELAY);
      radio.read(&recievedData, sizeof(recievedData));
      if (recievedData.checkSum == checkSum(recievedData.pos,recievedData.count,recievedData.id) && recievedData.id > 0){
         pos_arr[recieve_count] = recievedData.pos;
         count_arr[recieve_count] = recievedData.count;
         id_arr[recieve_count] = recievedData.id;
      }

  if (recieve_count >= MESSAGE_NUM * NODES_NUM * 9){
    flag= true;
    recieve_f = false;
  }
  recieve_count++;
  if(recieve_count % 15 == 0){
    timerDelay(2);
  }
 }

    if(flag){
      Serial.println("data : ");
      for(int i = 0; i < MESSAGE_NUM * NODES_NUM * 9; i++){
          Serial.print(" id: ");
          Serial.print(id_arr[i]);
          Serial.print(" pos: ");
          Serial.print(pos_arr[i]);
          Serial.print(" count: ");
          Serial.print(count_arr[i]);
          Serial.print(" checkSum: ");
          Serial.println(checkSum(id_arr[i],pos_arr[i],count_arr[i]));
      }
      flag = false;
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

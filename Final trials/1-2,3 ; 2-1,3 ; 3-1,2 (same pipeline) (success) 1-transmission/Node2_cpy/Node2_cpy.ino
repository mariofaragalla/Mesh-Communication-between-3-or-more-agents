//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define BAUDRATE 115200
#define DELAY 150
#define DEL 10
#define MESSAGE_NUM 5
#define NODES_NUM 3
#define NODES_NUM_1 2  // no. of nodes - 1 

typedef struct { // struct of the data sent containing the address and the position
//  unsigned long time_t;
  int16_t pos;
  uint16_t count;
  int16_t checkSum;
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

unsigned long prev_time_t = 0;
unsigned long current_time_t = 0;

int16_t pos_arr[250];
uint16_t count_arr[250];
int16_t checkSum_arr[250];
uint8_t id_arr[250];
uint8_t nodes = 0;
uint16_t counter_glb = 0;

bool flag = false;
bool send_f = true;
bool recieve_f = true;

bool rx_1st_time = true;
uint8_t rx_count = 0;
uint8_t trials = 0;
uint8_t mult = 0;

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
        Serial.println(recievedData.id);
    }
  }

  StartFastTimer();
}

void loop() {
  //Recieve Data from other Nodes

  if(recieve_f){
    if(rx_1st_time){
       for(int i = 0; i< NODE3 - 1; i++){
        for(uint8_t recieve_count = 0; recieve_count< MESSAGE_NUM + 2; recieve_count++){
          radio.openReadingPipe(1,address[LEADER]);  //read from the leader node
            radio.startListening();
            prev_time_t = current_time_t;
            current_time_t = FastTimer()/1000;
            while(current_time_t - prev_time_t < DELAY ){
                   current_time_t = FastTimer()/1000;
            }
            radio.read(&recievedData, sizeof(recievedData));
            if (recievedData.checkSum == checkSum(recievedData.pos,recievedData.count,recievedData.id) && recievedData.id > 0){
                pos_arr[counter_glb] = recievedData.pos;
                count_arr[counter_glb] = recievedData.count;
                id_arr[counter_glb] = recievedData.id;
                counter_glb++;
            } else {
              recievedData.count = 0;
            }
         }
//         delay(DEL);
      }
//      counter_glb++;
      rx_1st_time = false;
   } else{
      for(int i = 0; i< NODES_NUM - 1; i++){
        for(uint8_t recieve_count = 0; recieve_count< MESSAGE_NUM + 2; recieve_count++){
          radio.openReadingPipe(1,address[LEADER]);  //read from the leader node
            radio.startListening();
            prev_time_t = current_time_t;
            current_time_t = FastTimer()/1000;
            while(current_time_t - prev_time_t < DELAY ){
                   current_time_t = FastTimer()/1000;
            }
            radio.read(&recievedData, sizeof(recievedData));
            if (recievedData.checkSum == checkSum(recievedData.pos,recievedData.count,recievedData.id) && recievedData.id > 0){
                pos_arr[counter_glb] = recievedData.pos;
                count_arr[counter_glb] = recievedData.count;
                id_arr[counter_glb] = recievedData.id;
                counter_glb++;
            } else {
              recievedData.count = 0;
            }
         }
//         delay(DEL);
      }
//      counter_glb++;
      recieve_f = false;
      send_f = false;
      flag= true;
    }
 }  
   
      
 if(send_f){
    for(uint8_t send_count = 0; send_count< MESSAGE_NUM; send_count++){
        radio.stopListening();
        radio.openWritingPipe(address[LEADER]);
        prev_time_t = current_time_t;
        current_time_t = FastTimer()/1000;
        sentData.pos = random(-500,500);
        sentData.count = send_count+1;
        sentData.id = NODE3;
        sentData.checkSum = checkSum(sentData.pos,sentData.count,sentData.id);
        while(current_time_t - prev_time_t < DELAY ){
              current_time_t = FastTimer()/1000;
        }
        pos_arr[counter_glb] = sentData.pos;
        count_arr[counter_glb] = sentData.count;
        id_arr[counter_glb] = sentData.id;
  //        printStruct(sentData);
        counter_glb++;
        radio.write(&sentData, sizeof(sentData));
    }
//    delay(DEL);
//      counter_glb++;
 }
    
  if(flag){
      Serial.println("data : ");
      for(int i = 0; i < MESSAGE_NUM * NODES_NUM; i++){
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
//  Serial.print("time: ");
//  Serial.print(d.time_t);
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
        return sum/2 ;
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

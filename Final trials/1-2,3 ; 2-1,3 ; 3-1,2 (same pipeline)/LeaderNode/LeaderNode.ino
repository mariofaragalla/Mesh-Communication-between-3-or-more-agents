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
#define DELAY 150
#define MESSAGE_NUM 10
#define NODES_NUM 3

uint8_t trials = 0;
unsigned long prev_time_t = 0;
unsigned long current_time_t = 0;
uint16_t error[1001];

int16_t pos_arr[250];
uint16_t count_arr[250];
int16_t checkSum_arr[250];
uint8_t id_arr[250];
uint8_t nodes = 0;

bool flag = true;

typedef struct { // struct of the data sent containing the address and the position
//  unsigned long time_t;
  int16_t pos;
  uint16_t count;
  int16_t checkSum;
  uint8_t id ;
}data_t;


data_t sentData;  // data to be sent
data_t recievedData; // data that is recieved


void setup()
{
  Serial.begin(BAUDRATE);

  while (!Serial);
    Serial.begin(BAUDRATE);
  
  radio.begin();
  
  radio.stopListening();
  radio.openWritingPipe(address[LEADER]);
  StartFastTimer();
//  sentData.time_t = FastTimer()/1000;
  sentData.pos = 2137;
  sentData.id = LEADER;
  sentData.count= 0;
  
  radio.write(&sentData, sizeof(sentData));
  Serial.println("Starting timer");
  Serial.println("Sending data to other nodes and server");

}

void loop()
{  
  //Send messages to other Nodes
    while(sentData.count < MESSAGE_NUM){
        radio.stopListening();
        radio.openWritingPipe(address[LEADER]);
        prev_time_t = current_time_t;
        current_time_t = FastTimer()/1000;
        sentData.pos = random(0,500);
        sentData.count++;
        sentData.id = 1;
        sentData.checkSum = checkSum(sentData.pos,sentData.count,sentData.id);
        while(current_time_t - prev_time_t < DELAY ){
              current_time_t = FastTimer()/1000;
        }
        pos_arr[sentData.count] = sentData.pos;
        count_arr[sentData.count] = sentData.count;
        id_arr[sentData.count] = sentData.id;
//        printStruct(sentData);
        radio.write(&sentData, sizeof(sentData));
        error[sentData.count] = current_time_t - prev_time_t;
      }

      while(recievedData.count < MESSAGE_NUM *(NODES_NUM-1) && nodes < NODES_NUM-1){
          radio.openReadingPipe(1,address[LEADER]);  //read from the leader node
          radio.startListening();
          prev_time_t = current_time_t;
          current_time_t = FastTimer()/1000;
          while(current_time_t - prev_time_t < DELAY ){
                 current_time_t = FastTimer()/1000;
           }
          radio.read(&recievedData, sizeof(recievedData));
          if (recievedData.checkSum == checkSum(recievedData.pos,recievedData.count,recievedData.id) && recievedData.id > 0){
              pos_arr[recievedData.count+10] = recievedData.pos;
              count_arr[recievedData.count+10] = recievedData.count;
              id_arr[recievedData.count+10] = recievedData.id;
          } else {
            recievedData.count = 0;
          }
          nodes++;
    }
      
      if(flag){
          Serial.println("data: ");
          for(int i = 1; i <= MESSAGE_NUM*NODE3; i++){
              Serial.print(" id: ");
              Serial.print(id_arr[i]);
              Serial.print(" pos: ");
              Serial.print(pos_arr[i]);
              Serial.print(" count: ");
              Serial.print(count_arr[i]);
              Serial.print(" checkSum: ");
              Serial.println(checkSum(id_arr[i],pos_arr[i],count_arr[i]));
          }
          /*
          Serial.println("error: ");
          for(int i = 1; i <= MESSAGE_NUM; i++){
            Serial.println(error[i]);
            
          } */
          
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
      return sum/2;
}



// Timer methods (may be I could add them to a library later)
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

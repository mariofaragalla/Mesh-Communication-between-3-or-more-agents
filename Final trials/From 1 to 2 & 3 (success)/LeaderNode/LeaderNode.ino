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
#define DELAY 100

uint8_t my_id = LEADER;   // initial id of this node
bool leader = false; //set this node not to be the leader
uint8_t trials = 0;
bool ackSent = false;
uint16_t milli_sec = 0;
unsigned long prev_time_t = 0;
unsigned long current_time_t = 0;
uint16_t error[1001];
uint16_t error_e;
uint8_t byte_data = 0;
uint8_t byte_data_arr[2];

bool isChanged = false;
uint8_t curr_id = 0;

typedef struct { // struct of the data sent containing the address and the position
//  unsigned long time_t;
  int16_t pos;
  uint16_t count;
  int16_t checkSum;
  uint8_t id ;
}data_t;

int16_t pos_arr[250];
uint16_t count_arr[250];
int16_t checkSum_arr[250];
uint8_t id_arr[250];

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
    if(sentData.count < 200){
        radio.stopListening();
        radio.openWritingPipe(address[LEADER]);
        prev_time_t = current_time_t;
        current_time_t = FastTimer()/1000;
//        sentData.time_t = current_time_t;
        sentData.pos = random(0,500);
        sentData.count++;
        sentData.id = 1;
        sentData.checkSum = checkSum(sentData.pos,sentData.count,sentData.id);
        while(current_time_t - prev_time_t < DELAY ){
              current_time_t = FastTimer()/1000;
        }
        radio.write(&sentData, sizeof(sentData));
//        printStruct(sentData);
        pos_arr[sentData.count] = sentData.pos;
        count_arr[sentData.count] = sentData.count;
        id_arr[sentData.count] = sentData.id;
        error[sentData.count] = current_time_t - prev_time_t;
        
      }
      
      if(sentData.count == 200){
          Serial.println("data: ");
          for(int i = 1; i <= 200; i++){
              Serial.print(" id: ");
              Serial.print(id_arr[i]);
              Serial.print(" pos: ");
              Serial.print(pos_arr[i]);
              Serial.print(" count: ");
              Serial.print(count_arr[i]);
              Serial.print(" checkSum: ");
              Serial.println(checkSum(id_arr[i],pos_arr[i],count_arr[i]));
          }
          Serial.println("error: ");
          for(int i = 1; i <= 200; i++){
            Serial.println(error[i]);
            
          }
          sentData.count++;
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

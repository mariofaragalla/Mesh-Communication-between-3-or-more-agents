//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//channels through which two modules can communicate.
byte address[7]={'0','1','2','3','4','5','6'};

#define COMADR 1 // choose the desired communication address 


// defining the id's of all communicating nodes
#define SERVER 0
#define LEADER 1 
#define NODE2 2
#define NODE3 3

#define BAUDRATE 115200
#define DELAY 150 // 150 milliseconds
#define DEL 2     // 2 milliseconds
#define MESSAGE_NUM 5 // number of messages to be sent in one packet
#define NODES_NUM 3   // total number of nodes that are communicating

//uint8_t trials = 0;
unsigned long prev_time_t = 0;   // time of the last message sent/recieved
unsigned long current_time_t = 0; // the current time  

uint8_t trials = 0; // used if the transmission is intended to be run for a number of trials instead of time

typedef struct { // struct of the data sent containing (1 message)
//  unsigned long time_t;
  int16_t pos;  // the position (or actual data that should be sent) 
  uint16_t count; // counter to track each message
  int16_t checkSum; // checksum to validate the correctness of sent/recieved data 
  uint8_t id ; // the address from which node/to which node data is recieved or sent 
}data_t;

RF24 radio(7,30);  // CE, CSN 

data_t sentData;  // data to be sent
data_t recievedData; // data that is recieved


void setup()
{
  Serial.begin(BAUDRATE);

  while (!Serial);
    Serial.begin(BAUDRATE);
  
  radio.begin();  // initialize the nrf24 module 
  
  // At the very beginning the leader sends one message to the other nodes to indicate that all agents should start the data transmission scheme
  radio.stopListening(); // prepare the nrf24 module to send/write 
  radio.openWritingPipe(address[COMADR]); // choose the address on which the data is sent
  StartFastTimer(); // start the timer
//  sentData.time_t = FastTimer()/1000;

  // update data to be sent
  sentData.pos = 2137;   // arbitary data
  sentData.id = LEADER;
  sentData.count= 0;
  
  radio.write(&sentData, sizeof(sentData)); // send the very first message
  Serial.println("Starting timer"); 
  Serial.println("Sending data to other nodes and server");

}

void loop()
{   
//    if(trials < 10){    // to stop after certain number of trials uncomment this line and comment the following one
    
    if (FastTimer()/1000000 <= 300){  // stop after 5 mins = 300 seconds 
      //Send messages to other Nodes
    
       sendPacket(MESSAGE_NUM, LEADER); // send a packet using the leader (node 1) id
    
       radio.flush_tx();  // Empty the transmit buffer after sending to avoid overload
        
       recievePacket(MESSAGE_NUM);  // recieve a packet on the common pipeline
      
       radio.flush_rx();  // Empty the receive buffer after recieving to avoid overload
  
//       trials++;   // to stop after certain number of trials uncomment this line

    }
     
//     timerDelay(DEL);

}

// function to calculate & determine the checkSum
// the checkSum is made to validate whether a message is correct or not 
int16_t checkSum(int16_t id, int16_t pos , int16_t count){  
      int16_t sum = id + pos + count;
      return sum/2;
}

// Send according to the Data Transmission scheme
// this method takes the num of messages that should be sent per package and the id of the node sending the data
void sendPacket(uint8_t numOfMessages, uint8_t id){  
  
      timerDelay(DEL); //small delay (adjust to handle communication error)
      radio.stopListening();  // prepare the nrf24 module to send/write 
      radio.openWritingPipe(address[COMADR]); // choose the common pipeline (COMADR) for sending
      for(uint8_t send_count = 0; send_count< numOfMessages; send_count++){ // send the required no. of messages
          sentData.pos = random(-500,500);  // send random position to simulate actual position change
          sentData.count = send_count+1;  // update message tracking counter 
          sentData.id = id;   // update the message id 
          sentData.checkSum = checkSum(sentData.pos,sentData.count,sentData.id);  // calculate & update the sent checkSum 
          timerDelay(DELAY);  // delay for 150 ms
          radio.write(&sentData, sizeof(sentData)); // send one message
      }
}

// Recieve according to the Data Transmission scheme
void recievePacket(uint8_t numOfMessages){  
      for(uint8_t i = 0; i< NODES_NUM -1; i++){
            timerDelay(DEL*2);  // small delay (adjust to handle communication error)
            radio.openReadingPipe(1,address[COMADR]);  // choose the common pipeline (COMADR) for reading
            radio.startListening();   // prepare the nrf24 module to read/recieve 
          for(uint8_t recieve_count = 0; recieve_count< numOfMessages; recieve_count++){  // recieve the specified number of messages 
                timerDelay(DELAY);  // delay between recieving each message
                radio.read(&recievedData, sizeof(recievedData));  // read/recieve the data/message
            }
      }
}

 //use the timer to delay sending/recieving in milliseconds
void timerDelay(uint16_t delay_t){ 
  
   prev_time_t = current_time_t;
   current_time_t = FastTimer()/1000; //convert timer time from micros to millis
   // fetch time until the difference between the current time the time of 
   // the previous message becomes equal or more than the delay time
   while(current_time_t - prev_time_t < delay_t ){  
         current_time_t = FastTimer()/1000;
   }
  
}

// Timer methods (may be I could add them to a library later)

// Timer Code Courtesy : https://forum.arduino.cc/index.php?topic=712108.0

void StartFastTimer() {    

  noInterrupts ();  // protected code (turn off all interrupts so this code snippet won't be interrupted while it is being executed)
  // https://ww1.microchip.com/downloads/en/devicedoc/atmel-2549-8-bit-avr-microcontroller-atmega640-1280-1281-2560-2561_datasheet.pdf
  
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
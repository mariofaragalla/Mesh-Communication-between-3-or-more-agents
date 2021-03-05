//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//channel through which two modules communicate.
byte address[7]={'0','1','2','3','4','5','6'};

#define COMADR 1  // choose the desired communication address 

// defining the id's of all communicating nodes
#define SERVER 0
#define LEADER 1 
#define NODE2 2
#define NODE3 3

#define BAUDRATE 115200
#define DELAY 150 // 150 milliseconds
#define DEL 2 // 2 milliseconds
#define MESSAGE_NUM 5 // number of messages to be sent in one packet
#define NODES_NUM 3 // total number of nodes that are communicating


#define NODES_NUM_1 2  // no. of nodes - 1 

typedef struct { // struct of the data sent containing the address and the position

//  unsigned long time_t;
  int16_t pos; // the position (or actual data that should be sent) 
  uint16_t count; // counter to track each message
  int16_t checkSum; // checksum to validate the correctness of sent/recieved data 
  uint8_t id; // the address from which node/to which node data is recieved or sent 
}data_t;

//create an RF24 object
RF24 radio(7,30);  // CE, CSN 


data_t sentData;  // data to be sent
data_t recievedData; // data that is recieved

unsigned long prev_time_t = 0;  // time of the last message sent/recieved
unsigned long current_time_t = 0;  // the current time  

bool rx_1st_time = true;  // check if this node is recieving for the first time 
uint8_t trials = 0; // used if the transmission is intended to be run for a number of trials instead of time

void setup() {
  
  Serial.begin(BAUDRATE);
  
  radio.begin();  // initialize the nrf24 module 
  
  // At the very beginning the leader sends one message to the other nodes to indicate that all agents should start the data transmission scheme
  // so each node as this one should recieve this message before it starts the data transmission scheme

  radio.openReadingPipe(1,address[COMADR]); // choose the common pipeline (COMADR) to read from
  radio.startListening(); // prepare the nrf24 module to read/recieve 

//     recievedData.time_t = 0;
     recievedData.pos = 0;
     recievedData.count = 0;
     recievedData.id = 0;
  
  Serial.println("Start recieving");
  
   while(!radio.available()){ // Test if there are data available to be read.
      while(recievedData.id == 0){  //  keep recieving until a message containing any id other than 0 is recieved
        radio.read(&recievedData, sizeof(recievedData));  // read/recieve the data
    }
  }
  Serial.println(recievedData.id);

  StartFastTimer(); // start the timer
}

void loop() {
  //Recieve Data from other Nodes

//  if(trials < 10){ // to stop after a certain number of trials uncomment this line and comment the following one

    if (FastTimer()/1000000 <= 300){  // stop after 5 mins = 18000 seconds 
      
      // Apply the data Transmission scheme:
      if(rx_1st_time){  //if the node is recieving for the 1st time
        
        // According to the transmission scheme if the node is recieving for the 1st time it should recieve its node num (node 2) - 1 times
         recievePacket(MESSAGE_NUM, NODE2-1); 
         
        rx_1st_time = false;
        
     } else{
      
        recievePacket(MESSAGE_NUM, NODES_NUM-1);  // if it is not the 1st time then recieve for # of nodes - 1 times
        
      }
      
      radio.flush_rx();  // Empty the receive buffer after recieving to avoid overload
      
      sendPacket(MESSAGE_NUM, NODE2); // send a packet using node 2 id
     
      radio.flush_tx(); // Empty the transmit buffer after sending to avoid overload

//      trials++;   // to stop after a certain number of trials uncomment this line
      
    }
    
}

void printStruct(data_t d){ // a function to print the Struct for data checking
  Serial.print(" id: ");
  Serial.print(d.id);
  Serial.print(" pos: ");
  Serial.print(d.pos);
  Serial.print(" count: ");
  Serial.print(d.count);
  Serial.print(" checkSum: ");
  Serial.println(d.checkSum);
}

int16_t checkSum(int16_t id, int16_t pos , int16_t count){  //  function to calculate & determine the checkSum
        int16_t sum = id + pos + count;
        return sum/2 ;
}

// Send according to the Data Transmission scheme
// this method takes the num of messages that should be sent per package and the id of the node sending the data
void sendPacket(uint8_t numOfMessages, uint8_t id){
  
      timerDelay(DEL);  // small delay (adjust to handle communication error)
      radio.stopListening();  // prepare the nrf24 module to send/write 
      radio.openWritingPipe(address[COMADR]);  // choose the common pipeline (COMADR) for sending
      for(uint8_t send_count = 0; send_count< numOfMessages; send_count++){ // send the required no. of messages
          sentData.pos = random(-500,500);  // send random position to simulate actual position change 
          sentData.count = send_count+1;  // update message tracking counter 
          sentData.id = id;  // update the message id 
          sentData.checkSum = checkSum(sentData.pos,sentData.count,sentData.id);  // calculate & update the sent checkSum 
          timerDelay(DELAY);  // delay for 150 ms
          radio.write(&sentData, sizeof(sentData)); // send one message   
        }
        
}

// Recieve and change the number of iterations of recieving according to the Data Transmission scheme
void recievePacket(uint8_t numOfMessages, uint8_t iterations){
      
      for(uint8_t i = 0; i< iterations; i++){
            timerDelay(DEL*2);  // small delay (adjust to handle communication error)
            radio.openReadingPipe(1,address[COMADR]);  //read from the common pipeline
            radio.startListening(); // prepare the nrf24 module to read/recieve 
            
        for(uint8_t recieve_count = 0; recieve_count< numOfMessages; recieve_count++){   // recieve the specified number of messages 
              timerDelay(DELAY);  // delay between recieving each message
              radio.read(&recievedData, sizeof(recievedData));  // read/recieve the data/message
              
              // calculate the checkSum of the recieved data and compare it with the recieved checkSum to check if data is correct 
              // Also check if the id is not zero (in case the checkSum is zero by any chance)
              if (recievedData.checkSum == checkSum(recievedData.pos,recievedData.count,recievedData.id) && recievedData.id > 0){ 
                 
                 // here you consider the data by saving it into a data structure or an external file for example since 
                 // data here is the correct data

              } else {  // if the data is not correct then reset the counter
                recievedData.count = 0;
              }

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

#include "SimpleMeshLib.h"


unsigned long prev_time_t = 0;  // time of the last message sent/recieved
unsigned long current_time_t = 0;  // the current time 


SimpleMesh::SimpleMesh(bool enable = true){
     recievedData.pos = 0;
     recievedData.count = 0;
     recievedData.id = 0;
     recievedData.checkSum = 0;
} 
#if(defined(__AVR_ATmega2560__)) // if Arduino Mega is used
  RF24 radio_1 (7,30);
#endif
#if defined(__AVR_ATmega328P__) // if Arduino Uno is used
  RF24 radio_1 (7,10);
#endif

void SimpleMesh::printStruct(data_t d){ // a function to print the Struct for data checking
  Serial.print(" id: ");
  Serial.print(d.id);
  Serial.print(" pos: ");
  Serial.print(d.pos);
  Serial.print(" count: ");
  Serial.print(d.count);
  Serial.print(" checkSum: ");
  Serial.println(d.checkSum);
}

int16_t SimpleMesh::checkSum(int16_t id, int16_t pos , int16_t count){  //  function to calculate & determine the checkSum
        int16_t sum = id + pos + count;
        return sum/2 ;
}

// Send according to the Data Transmission scheme
// this method takes the num of messages that should be sent per package and the id of the node sending the data
void SimpleMesh::sendPacket(uint8_t numOfMessages, uint8_t id){
  
      timerDelay(DEL);  // small delay (adjust to handle communication error)
      radio_1.stopListening();  // prepare the nrf24 module to send/write 
      radio_1.openWritingPipe(COMADRR);  // choose the common pipeline (COMADRR) for sending
      for(uint8_t send_count = 0; send_count< numOfMessages; send_count++){ // send the required no. of messages
          sentData.pos = random(-500,500);  // send random position to simulate actual position change 
          sentData.count = send_count+1;  // update message tracking counter 
          sentData.id = id;  // update the message id 
          sentData.checkSum = checkSum(sentData.pos,sentData.count,sentData.id);  // calculate & update the sent checkSum 
          timerDelay(DELAY);  // delay for 150 ms
          radio_1.write(&sentData, sizeof(sentData)); // send one message   
        }
        
}

// Recieve and change the number of iterations of recieving according to the Data Transmission scheme
void SimpleMesh::recievePacket(uint8_t numOfMessages, uint8_t iterations){
      
      for(uint8_t i = 0; i< iterations; i++){
            timerDelay(DEL*2);  // small delay (adjust to handle communication error)
            radio_1.openReadingPipe(1,COMADRR);  //read from the common pipeline
            radio_1.startListening(); // prepare the nrf24 module to read/recieve 
            
        for(uint8_t recieve_count = 0; recieve_count< numOfMessages; recieve_count++){   // recieve the specified number of messages 
              timerDelay(DELAY);  // delay between recieving each message
              radio_1.read(&recievedData, sizeof(recievedData));  // read/recieve the data/message
              
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


volatile uint16_t Overflows = 0;

unsigned long SimpleMesh::FastTimer() {
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

void SimpleMesh::timerDelay(uint16_t delay_t){  
  
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

void SimpleMesh::StartFastTimer() {

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

ISR(TIMER1_OVF_vect){ // interrupts each 0.0625 us seconds
  Overflows++;
}

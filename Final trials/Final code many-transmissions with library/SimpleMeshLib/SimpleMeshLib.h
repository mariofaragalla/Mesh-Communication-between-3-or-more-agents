// helpful source: https://youtu.be/fE3Dw0slhIc
#ifndef SmplML
#define SmplML

#if (ARDUINO >= 100)
  #include "Arduino.h"
#endif

#define DELAY 150 // 150 milliseconds
#define DEL 2 // 2 milliseconds

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>



#define COMADRR '1'  // choose the desired communication address 

// defining the id's of all communicating nodes
#define SERVER 0
#define LEADER 1 
#define NODE2 2
#define NODE3 3

class SimpleMesh {
  public:
  
//  Attributes
  
  typedef struct { // struct of the data sent containing the address and the position
    int16_t pos; // the position (or actual data that should be sent) 
    uint16_t count; // counter to track each message
    int16_t checkSum; // checksum to validate the correctness of sent/recieved data 
    uint8_t id; // the address from which node/to which node data is recieved or sent 
  }data_t;

  
  data_t sentData;  // data to be sent
  data_t recievedData; // data that is recieved
  
  uint8_t CE; 
  uint8_t CSN;
  
  // Constructor
  
  SimpleMesh(bool enable);

  //Methods

  void printStruct(data_t d);
  int16_t checkSum(int16_t id, int16_t pos , int16_t count);
  void sendPacket(uint8_t numOfMessages, uint8_t id);
  void recievePacket(uint8_t numOfMessages, uint8_t iterations);
  unsigned long FastTimer();
  void timerDelay(uint16_t delay_t);
  void StartFastTimer();
  uint8_t getCE(){
    return CE;
  }
   uint8_t getCSN(){
    return CSN;
  }
};
#endif

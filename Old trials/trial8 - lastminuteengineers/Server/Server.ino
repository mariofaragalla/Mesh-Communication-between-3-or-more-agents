//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(7,10);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00000";

typedef struct {
  int16_t pos;
  uint8_t address;
}data;

data leaderData;


void setup()
{
  radio.begin();
  
  //set the address
  radio.openWritingPipe(address);
  
  //Set module as transmitter
  radio.stopListening();

  leaderData.pos = 10101;
  leaderData.address = 1;
}

void loop()
{
  //Send message to receiver
//  const char text[] = "Hello World";
  
//  radio.write(&text, sizeof(text));
  radio.write(&leaderData, sizeof(leaderData));
  
//  delay(10);
}

//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(7,30);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00000";

typedef struct {
  int16_t pos;
  uint8_t address;
}data;

data leaderData;


void setup()
{
  while (!Serial);
    Serial.begin(9600);
  
  radio.begin();
  
  //set the address
  radio.openReadingPipe(0, address);
  
  //Set module as receiver
  radio.startListening();
}

void loop()
{
  //Read the data if available in buffer
  if (radio.available())
  {
//    char text[32] = {0};
//    radio.read(&text, sizeof(text));

    radio.read(&leaderData, sizeof(leaderData));

    printStruct(leaderData);
  }
}

void printStruct(data d){
  Serial.print("address: ");
  Serial.print(d.address);
  Serial.print(" pos: ");
  Serial.println(d.pos);
}

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 30); // CE, CSN
const uint64_t pipe[1] = {0xF0F0F0F0F3LL};
int c = 0;
int i = 0 ;

//List of addresses: 0xF0F0F0F0F3LL, 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL
void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1, pipe[0]);
  radio.startListening();
  radio.setRetries(0, 0);

}

void loop() {
  i += 5;
  unsigned int recievedData ;
  unsigned int sentData = 400 + i;
  if ( radio.available() )
  {
    radio.writeAckPayload(1, &sentData, sizeof(sentData));

    radio.read(&recievedData, sizeof(&recievedData));

  }
  //    delay(2);

      Serial.print("ClientRecievedData");
      Serial.print(",");
      Serial.println(recievedData);
      Serial.print(" ");
      Serial.print("ClientSentData");
      Serial.print(",");
      Serial.println(sentData);
//  Serial.print(sentData);
//  Serial.print(" ");
//  Serial.println(recievedData);

}

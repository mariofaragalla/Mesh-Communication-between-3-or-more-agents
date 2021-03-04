#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 30); // CE, CSN
const uint64_t pipe[1] = {0xF0F0F0F0E1LL};
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
  
  if (radio.available()) {
    unsigned int recievedData=0;
    delay(600);
    char text[32] = "";
    delay(100);
    radio.read(&recievedData, sizeof(recievedData));
    delay(100);
//    radio.read(&text, sizeof(text));
    Serial.println(recievedData);
    Serial.print(text);
   
  }
  
  /* Simulink s-function code:
    unsigned int recievedData ;
    unsigned int sentData = 1;
    if ( radio.available() )
    {
      radio.writeAckPayload(1, &sentData, sizeof(sentData));

        radio.read(&recievedData, sizeof(&recievedData));

    }
    delay(2);

    Serial.print("Client recieved Data : ");
    //  Serial.println((recievedData/14746371.0)/1.0);
    Serial.println(recievedData);
    Serial.print("Client sent Data : ");
    Serial.println(sentData);
    */
  
  /* Testing code 2:
    if (radio.available() && i < 6) {
      delay(1000);
      radio.startListening();
      char text[32] = "";
      radio.read(&text, sizeof(text));
      radio.read(&i, sizeof(i));
      Serial.print(text);
      Serial.println(i);
    } else {
      radio.stopListening();
      radio.openWritingPipe(pipe[0]);
      const char text[] = "From client to server";
      radio.write(&text, sizeof(text));
      ++c;
      radio.write(&c, sizeof(c));
      Serial.print("Client has sent message no :");
      Serial.println(c);
      delay(1000);
    } */
}

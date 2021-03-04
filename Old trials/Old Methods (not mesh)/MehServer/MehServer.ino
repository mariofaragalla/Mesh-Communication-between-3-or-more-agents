#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 10); // CE, CSN
const uint64_t pipe[1] = {0xF0F0F0F0E1LL};
//List of addresses: 0xF0F0F0F0F3LL, 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL
//  double locationone;
int i = 0;
void setup() {

  Serial.begin(9600);
  radio.begin();
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.stopListening();


  //  radio.openWritingPipe(1,pipe[0]);
  radio.setRetries(0, 0);

}

void loop() {
  bool tx_sent;
  radio.openWritingPipe(pipe[0]);
  unsigned int sentData = 3000;
  const char text[] = " from Server to client";
  tx_sent = radio.write(&sentData, sizeof(sentData));
  ++i;
  radio.write(&text, sizeof(text));
  Serial.print(" Server has sent message no : ");
  Serial.println(i);
  Serial.println(tx_sent);
  delay(800);
  
  /* Simulink s-function code:
    const uint64_t pipe[3] = {0xF0F0F0F0F3LL,0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL};
    //  int counter[1] = {1};
    unsigned int recievedData ;
    unsigned int sentData = 3;
    for (byte node = 0; node < 3; node++) {
       radio.openWritingPipe(pipe[1]);
       bool tx_sent;

       tx_sent = radio.write(&sentData, sizeof(sentData));
       Serial.println(tx_sent);
       if (tx_sent) {
           if (radio.isAckPayloadAvailable()) {
               radio.read(&recievedData, sizeof(recievedData));
    //              ID[0] = counter[0]+1;
           }
           else
           {
               radio.flush_rx();
               radio.flush_tx();
           }
       }
       else
       {
           radio.flush_tx();
       }
    }
    Serial.print("Server recieved Data : ");
    //  Serial.println((recievedData/14746371.0)/1.0);
    Serial.println(recievedData);
    Serial.print("Server sent Data : ");
    Serial.println(sentData);
  */

/* Testing code 2:
    if (i < 6) {
    radio.stopListening();
    radio.openWritingPipe(pipe[0]);
    const char text[] = "From Server to client";
    radio.write(&text, sizeof(text));
    ++i;
    radio.write(&i, sizeof(i));
    Serial.print("Server has sent message no : ");
    Serial.println(i);
    delay(1000);
    }
    else{
    radio.startListening();
    radio.openReadingPipe(1, pipe[0]);
    delay(1000);
    char text[32] = "";
    radio.read(&text, sizeof(text));
    radio.read(&i, sizeof(i));
    Serial.print(text);
    Serial.println(i);
    } */

}

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 10); // CE, CSN
const uint64_t pipe[1] = {0xF0F0F0F0E1LL};
//  double locationone;
int i = 0;
unsigned int sentData = 3;

void setup() {

  Serial.begin(9600);
  radio.begin();
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.stopListening();
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  radio.setRetries(0, 0);

}

void loop() {

  const uint64_t pipe[3] = {0xF0F0F0F0F3LL, 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL};
  //  int counter[1] = {1};
  unsigned int recievedData ;


  unsigned int counter[3] = {0, 1, 2};
  for (byte node = 0; node < 3; node++) {
    radio.openWritingPipe(pipe[counter[i]]);
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
  Serial.print("ServerRecievedData");
  Serial.print(",");
  Serial.print(recievedData);
  Serial.print(" ");
  Serial.print("ServerSentData");
  Serial.print(",");
  Serial.println(sentData);
  sentData++;
  if (i < 3)
    i++;
  else i = 0;
}

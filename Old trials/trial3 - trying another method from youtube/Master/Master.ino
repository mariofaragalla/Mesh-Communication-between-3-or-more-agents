#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <nRF24L01.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/

#define CE  7 //This pin is used to set the nRF24 to standby (0) or active mode (1)
#define CS  10 //This pin is used for SPI comm chip select (DON'T FORGET TO CHANGE CS (10 for uno, 30 for mega))
RF24 radio(CE, CS); // Declare object from nRF24 library (Create your wireless SPI) 
const uint64_t rAddress = 0x01;  //Create pipe address for the network and notice
//const uint8_t rFChan = 89; //Set channel frequency default (chan 84 is 2.484GHz to 2.489GHz)

//Create a structure to hold data and channel data
struct PayLoad {
  uint8_t chan;
  uint8_t data;
} payload;

void setup() {
  radio.begin();  //Start the nRF24 module
  //radio.setChannel(rFChan); //set communication frequency channel
  radio.openReadingPipe(1,rAddress);  //This is receiver or master so we need to be ready to read data from transmitters
  radio.startListening();    // Start listening for messages
  Serial.begin(115200);  //serial port to display received data
  Serial.println("Network master is online...");
}

void loop() {
  if(radio.available()){ //Check if recieved data
     radio.read(&payload, sizeof(payload)); //read packet of data and store it in struct object
     Serial.print("Received data packet from node: ");
     Serial.println(payload.chan); //print node number or channel
     Serial.print("Node sensor value is: ");
     Serial.println(payload.sensor); //print node's sensor value
     Serial.println(); 
  }
}

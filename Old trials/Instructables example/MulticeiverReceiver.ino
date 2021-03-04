// An example demonstrating the multiceiver capability of the NRF24L01+
// in a star network with one PRX hub and up to six PTX nodes

//This sketch is a modification from a video on the ForceTronics YouTube Channel,
//which code was leveraged from http://maniacbug.github.io/RF24/starping_8pde-example.html
//This sketch is free to the public to use and modify at your own risk

#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <nRF24L01.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/

const int pinCE = 7; //This pin is used to set the nRF24 to standby (0) or active mode (1)
const int pinCSN = 8; //This pin is used to tell the nRF24 whether the SPI communication is a command or message to send out
RF24 radio(pinCE, pinCSN); // Declare object from nRF24 library (Create your wireless SPI)

//Create up to 6 pipe addresses P0 - P5;  the "LL" is for LongLong type
const uint64_t rAddress[] = {0x7878787878LL, 0xB3B4B5B6F1LL, 0xB3B4B5B6CDLL, 0xB3B4B5B6A3LL, 0xB3B4B5B60FLL, 0xB3B4B5B605LL };

byte daNumber = 0; //The number that the transmitters are trying to guess

void setup()   
{
  randomSeed(analogRead(0)); //create unique seed value for random number generation
  daNumber = (byte)random(11); //Create random number that transmitters have to guess
  Serial.begin(115200);  //start serial to communication
  Serial.print("The number they are trying to guess is: "); 
  Serial.println(daNumber); //print the number that they have to guess
  Serial.println();
  radio.begin();  //Start the nRF24 module

  radio.setPALevel(RF24_PA_LOW);  // "short range setting" - increase if you want more range AND have a good power supply
  radio.setChannel(108);          // the higher channels tend to be more "open"

  // Open up to six pipes for PRX to receive data
  radio.openReadingPipe(0,rAddress[0]);
  radio.openReadingPipe(1,rAddress[1]);
  radio.openReadingPipe(2,rAddress[2]);
  radio.openReadingPipe(3,rAddress[3]);
  radio.openReadingPipe(4,rAddress[4]);
  radio.openReadingPipe(5,rAddress[5]);
  
  radio.startListening();                 // Start listening for messages
}

void loop()  
{   
    byte pipeNum = 0; //variable to hold which reading pipe sent data
    byte gotByte = 0; //used to store payload from transmit module
    
    while(radio.available(&pipeNum)){ //Check if received data
     radio.read( &gotByte, 1 ); //read one byte of data and store it in gotByte variable
     Serial.print("Received guess from transmitter: "); 
     Serial.println(pipeNum + 1); //print which pipe or transmitter this is from
     Serial.print("They guess number: ");
     Serial.println(gotByte); //print payload or the number the transmitter guessed
     if(gotByte != daNumber) { //if true they guessed wrong
      Serial.println("Fail!! Try again."); 
     }
     else { //if this is true they guessed right
      if(sendCorrectNumber(pipeNum)) Serial.println("Correct! You're done."); //if true we successfully responded
      else Serial.println("Write failed"); //if true we failed responding
     }
     Serial.println();
    }

   delay(200);    
}

 //This function turns the receiver into a transmitter briefly to tell one of the nRF24s
//in the network that it guessed the right number. Returns true if write to module was
//successful
bool sendCorrectNumber(byte xMitter) {
    bool worked; //variable to track if write was successful
    radio.stopListening(); //Stop listening, start receiving data.
    radio.openWritingPipe(rAddress[xMitter]); //Open writing pipe to the nRF24 that guessed the right number
      // note that this is the same pipe address that was just used for receiving
    if(!radio.write(&daNumber, 1))  worked = false; //write the correct number to the nRF24 module, and check that it was received
    else worked = true; //it was received
    radio.startListening(); //Switch back to a receiver
    return worked;  //return whether write was successful
}

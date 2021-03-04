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


RF24 radio(pinCE, pinCSN); // Create your nRF24 object or wireless SPI connection

#define WHICH_NODE 2     // must be a number from 1 - 6 identifying the PTX node

const uint64_t wAddress[] = {0x7878787878LL, 0xB3B4B5B6F1LL, 0xB3B4B5B6CDLL, 0xB3B4B5B6A3LL, 0xB3B4B5B60FLL, 0xB3B4B5B605LL};

const uint64_t PTXpipe = wAddress[ WHICH_NODE - 1 ];   // Pulls the address from the above array for this node's pipe

byte counter = 1; //used to count the packets sent

bool done = false; //used to know when to stop sending packets


void setup()  

{

  Serial.begin(115200);   //start serial to communicate process

  randomSeed(analogRead(0)); //create unique seed value for random number generation

  radio.begin();            //Start the nRF24 module

  radio.setPALevel(RF24_PA_LOW);  // "short range setting" - increase if you want more range AND have a good power supply
  radio.setChannel(108);          // the higher channels tend to be more "open"

  radio.openReadingPipe(0,PTXpipe);  //open reading or receive pipe

  radio.stopListening(); //go into transmit mode

}

void loop()  

{

   if(!done) { //true once you guess the right number

     byte randNumber = (byte)random(11); //generate random guess between 0 and 10

     radio.openWritingPipe(PTXpipe);        //open writing or transmit pipe

     if (!radio.write( &randNumber, 1 )){  //if the write fails let the user know over serial monitor

         Serial.println("Guess delivery failed");      

     }

     else { //if the write was successful 

          Serial.print("Success sending guess: ");

          Serial.println(randNumber);

       

        radio.startListening(); //switch to receive mode to see if the guess was right

        unsigned long startTimer = millis(); //start timer, we will wait 200ms 

        bool timeout = false; 

        while ( !radio.available() && !timeout ) { //run while no receive data and not timed out

          if (millis() - startTimer > 200 ) timeout = true; //timed out

        }

    

        if (timeout) Serial.println("Last guess was wrong, try again"); //no data to receive guess must have been wrong

        else  { //we received something so guess must have been right

          byte daNumber; //variable to store received value

          radio.read( &daNumber,1); //read value

          if(daNumber == randNumber) { //make sure it equals value we just sent, if so we are done

            Serial.println("You guessed right so you are done");

            done = true; //signal to loop that we are done guessing

          }

          else Serial.println("Something went wrong, keep guessing"); //this should never be true, but just in case

        }

        radio.stopListening(); //go back to transmit mode

     }

   }

    delay(1000);

}

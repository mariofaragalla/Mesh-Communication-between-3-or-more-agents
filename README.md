# Overview
This Project involves the communication between four nrf24 modules, where three are installed on three Arduinos of type Mega (**agents**) while one is 
connected to Arduino Uno (**server**).

## Connection
The Hardware connection in this project is established according to this table:

|   Nrf24 pin   | Arduino Mega  |  Arduino Uno  |
|:-------------:|:-------------:|:-------------:|
|     VCC       |     3.3V      |     3.3V      |
|     GND       |     GND       |     GND       |
|     CSN       |     32        |     10        |
|     CE        |     7         |     7         |
|     SCK       |     52        |     13        |
|     MOSI      |     51        |     11        |
|     MISO      |     50        |     12        |

## About the Communication
### Agents:
The agents send and recieve data to each other according to the following **data transmission scheme**: <br>  <br> 

<img width="758" alt="Data Transmission scheme between 3 agents" src="https://user-images.githubusercontent.com/55330224/109875453-1cbd7f00-7c79-11eb-909d-91df4b8362bb.PNG">

_Check [the data transmission scheme file](Final%20trials/Transmission%20scheme%20schedule.docx) for more details_. <br> <br>
The type of communication used here is similar to mesh flooding (here is a [link](https://hackaday.io/project/20388-home-smart-mesh/log/59566-nrf24l01-flood-mesh-protocol) 
for some insight). Each agent sends a packet of 5 messages where each message contains the following: 
1. The **data** being sent.
2. A **counter** to track the number of message sent
3. A **checkSum** to validate the correctness of the data.
4. An **id** which defines from which agent is the message sent.

_Note that: the id is included inside the message packet which is not normal case._ <br>

All agents are using one pipeline/address (can handle up to six other modules) for both sending and recieving. That's why the transmission scheme is used to ensure that there is no data collision. 
<br> The agents are classified into a leader and other agents(or nodes). This concept can be used in communication in multi-agent systems such as swarm robots. 
Currently the leader and the nodes are statically allocated (we must specify from the beginning which agent is the leader and which ones are the agents).

### Server:

For now the server has only one task which is [data logging](https://user-images.githubusercontent.com/55330224/110165399-8d8ca480-7dfb-11eb-9a1a-05a92ba9c824.mp4), so basically its only duty is to monitor all the messages being sent or recieved to/from other agents 
by listening to the same address/pipeline that all other agents use for communication. 
Data logging is impotant for online data visualization and in order to keep track of messages to detect if any data is lost during the whole transmission process. 

## Organization

The Final code can be found here: [/Final trials/Final code many-transmissions/Documented code](/Final%20trials/Final%20code%20many-transmissions/Documented%20code) as follows:
1. **Leadernode.ino**: which is the file containing the code for the leader node (node 1)
2. **Node2.ino**: which is the file containing the code for node 2
3. **Node3.ino**: which is the file containing the code for node 3 
4. **Server.ino**:  which is the file containing the code for the Server (node 0)

N.B: The library used for this project is the [RF24.h](https://maniacbug.github.io/RF24/classRF24.html).

The final code can be tested using these [files](Final%20trials/Final%20code%20many-transmissions%20(code%20for%20testing)). Also you can find some results for the testing code [here](Final%20trials/Results%20many-transmission%20(final).xlsx). Other files in the [Final trials](Final%20trials) folder include previous trials (before reaching the final result) , results of these prevouis trials and testing other components.

## Sequence of Operation

First connect all arduinos to the computer, then open the files and adjust the com ports for each arduino. After that, in order to run the whole program,  [Node2.ino](Final%20trials/Final%20code%20many-transmissions/Documented%20code/Node2) , [Node3.ino](Final%20trials/Final%20code%20many-transmissions/Documented%20code/Node3) & [Server.ino](Final%20trials/Final%20code%20many-transmissions/Documented%20code/Server) should run first so that nodes 2 & 3 and the Server prepare themselves for listening before the leader broadcast any message. After that run the [Leadernode.ino](Final%20trials/Final%20code%20many-transmissions/Documented%20code/Leadernode) file to start the whole data transmission scheme disscussed above. In order to, monitor the messages being sent or recieved during transmission, the Serial plotter in the Server.ino file is used.

Check this [video](https://user-images.githubusercontent.com/55330224/110161169-abefa180-7df5-11eb-8672-aaebe4a6d775.mp4).




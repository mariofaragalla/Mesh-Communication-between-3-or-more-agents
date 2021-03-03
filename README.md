# Overview
This Project involves the communication between four nrf24 modules, where three are installed on three Arduinos of type Mega (**agents**) while one is 
connected to Arduino Uno (**server**).  

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
For now the leader and the nodes are statically allocated (we must specify from the beginning which agent is the leader and which ones are the agents).

### Server:

For now the server has only one task which is data logging, so basically its only duty is to monitor all the messages being sent or recieved to/from other agents 
by listening to the same address/pipeline that all other agents use for communication. 
Data logging is impotant for online data visualization and in order to keep track of messages to detect if any data is lost during the whole transmission process. 

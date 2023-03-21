
# IPK Project 1: IPK Calculator Protocol


Implementation of a client for the IPK Calculator Protocol (described later). 

## Authors

- [@xpejch08](https://www.github.com/xpejch08)


## Theory of TCP/UDP communication
### TCP (Transmission Control Protocol)
TCP (Transmission Control Protocol) is a defined way of communication through a network between 2 seperete candidates. TCP unlike UDP is a connection-oriented protocol which means the connection has to be established and maintained untill the final message exchange ends. That makes TCP more reliable. TCP is one of the most used protocols, because it ensures you will recieve your packets correctly. In TCP communication there are 4 main parts to implement on the client side and those are:
- Creating a socket and connecting to a server
- Sending a message(request) to the server
- Read the reply from the server
- Closing the connection with the server

TCP communication can look like something below where the red rectangle shows which part of the communication can be done repeatedly and which part happens only once.
![Picture 1](/img/TCP.png "TCP Communication")

### UDP (User Datagram Protocol)
UDP (User Datagram Protocol) is also a defined way of communication similar to TCP. The main difference between TCP and UDP is that UDP is a connectionless and unreliable protocol. There is no need to establish a connection prior to data transfer, that can result in some loss of data and that is why UDP is used mainly for low-latency, losst-tolerating connections established over a network. The UDP communication is used for example in voice or video connection mainly because it is more efficient. One of the reasons is that UDP doesn't check for errors. In UDP communication there are also 4 main parts to implement on the client side:
- Creating a socket(in UDP u don't have to conncect before sending packets)
- Creating a packet(datagram) with the serverIP and port and sending it
- Reading a packet(datagram) from the server
- closing th connection

UDP communication can look like something below where the red rectangle shows which part of the communication can be done repeatedly and which part happens only once.
![Picture 2](/img/UDP.png "UDP Communication")
## Protocol description
The Protocol is described in 2 variants, Binary and Textual.
### Binary
The binary variant is stateless and uses UDP communication. The first byte of the request message contains the operation code(0 for request 1 for response) in ASCII format, the second byte contains the payload length. The rest is the payload itself. The response message consists of 4 main parts. The first byte is the operation code, the second is the status code(0 for succes 1 for error), the third byte is the payload length and the rest is the payload itself.
### Textual
The textual variant is statefull and operates over TCP. The communication begins with a "HELLO" message from the client, the server then responds with "HELLO" itself. The client can then send a "SOLVE" request and expression to count(formatted bellow). The server then sends a response with the answer. Communication is ended with a "BYE" message from the client after recieving a "BYE" message from the server
TCP protocol format:
- hello = "HELLO" LF
- solve = "SOLVE" SP query LF
- result = "RESULT" SP 1*DIGIT LF
- bye = "BYE" LF

## My implementation
I implemented my code in C++. The code is implemented in 1 sourcefile called main.cpp. It can be divded into 4 main parts **helpFunctions class**, **clientTCP class**, **clientUDP class** and the **main body**
### helpFunctions
help functions class contains one method called **checkArgs**, it's only purpose is to check if the command line arguments (argv) are correct, if they are incorrect, or there is too many or little of them the program ends with an error message on stderr.  
### clientTCP class
#### Variables
int cSocket, portNumber, byteStx, byteSrx;\
int first = 0;\
socklen_t serverLen;\
const char* serverHost;\
struct hostent *server;\
struct sockaddr_in serverAdress;\
char buffer[BUFFER_SIZE];

#### Methods
 setArgs()        |
| printArgs()      |
| getServerAdress()|
| findIp()         |
| printInfo()      |
| createSocket()   |
| readMessageFromUser()|
| sendMessage()    |
| recieveMessage() |
| cleanBuff()

- **void setArgs()** is a method used for setting the command line arguments
- **void printArgs()** is a method used for debugging to check if all the arguments are correct
- **void getServerAdress()** is a method that obtains the server address. I use the gethostbyname() function to obtain the server. If the server isn't found it results with an error message printed on stderr and the program ends. 
- **void findIp()** is a method that finds the IP adress of the server. 
- **void printInfo()** id a method used for debugging, it prints the info about the server depending on the IP adress.
- **void createSocket()** is a method that creates a new socket using the socket() function from the socket.h library. The socket is of domain AF_INET and the type SOCK_STREAM If the socket creation fails an error message is printed on the stderr and the program is ended.
- **void readMessageFromUser()** is a method that reads the message the user writes on the stdin into the buffer, then it connects to the server, using the connect function from the socket.h library. It connects using the created socket and the server address. The connection occurs only once before disconnecting. If the connection fails an error message is printed on the stderr and the program ends
- **void sendMessage()** is a method that sends the message copied into the buffer from the stdin to the server using the send() function defined in the socket.h library. If the send fails the program writes an error message on the stderr and ends itself.
- **void recieveMessage()** method recieves the message from the server using the read()function defined int the socket.h library. If the read fails the program prints an error message on the stderr and ends itself. It then prints the message out on the stdin. If the message is "BYE\n" the connection between the client and the server ends and the program is ended.
- **void cleanBuff()** method cleans the buffer
### clientUDP class
#### Variables
int cSocket, portNumber, byteStx, byteSrx, bLen;\
int first = 0;\
socklen_t serverLen;\
const char* serverHost;\
struct hostent *server;\
struct sockaddr_in serverAdress;\
char buffer[1 + 2 + BUFFER_SIZE];\
char message[BUFFER_SIZE];

#### Methods
 setArgs()        |
| printArgs()      |
| getServerAdress()|
| findIp()         |
| printInfo()      |
| createSocket()   |
| readMessageFromUser()|
| sendMessage()    |
| recieveMessage() |
| cleanBuff()

- **void setArgs()** is a method used for setting the command line arguments
- **void printArgs()** is a method used for debugging to check if all the arguments are correct
- **void getServerAdress()** is a method that obtains the server agres. I use the gethostbyname() function to obtain the server. If the server isn't found it results with an error message printed on stderr and the program ends. 
- **void findIp()** is a method that finds the IP address of the server. 
- **void printInfo()** is a method used for debugging, it prints the info about the server depending on the IP adress.
- **void createSocket()** is a method that creates a new socket using the socket() function from the socket.h library. The socket is of domain AF_INET and type SOCK_DGRAM. If the socket creation fails an error message is printed on the stderr and the program is ended.
- **void readMessageFromUser()** is a method that reads the message the user writes on the stdin into the buffer, the message must then be formatted as the protocol defines. I concat 3 bytes to the beginning of the buffer. I then set the first one to a zero, because the opcode for request is 0 and then set the second one to the length of the message. 
- **void sendMessage()** is a method that sends the message copied into the buffer from the stdin after formatting it,  to the server using the sendto() function defined in the socket.h library. If the send fails the program writes an error message on the stderr and ends itself.
- **void recieveMessage()** method recieves the message from the server using the recvfrom() function defined int the socket.h library. If the read fails the program prints an error message on the stderr and ends itself. Otherwise it prints the message out on the stdin.
- **void cleanBuff()** method cleans the buffer
### main body
In the main body I first declare all the objects I use (client, clientUDP, help). It then checks the command line arguments using the checkArgs(method) and sets the arguments for both objects. After that the main body consists of an if statement checking whether the tcp or the udp mode is turned on. The branches of the if statement are the same, the only difference is that hey are operating on different objects, either the client -> clientTCP object, or the clientUDP -> clientUDP object. The branches consist of two parts, what happens before a infinite loop, and what happens after it. Before the loop i call getServerAdress(), findIp() and createSocket(). In the infinite loop I call readMessageFromUser(), sendMessage(), cleanBuff(), recieveMessage(), and then cleanBuff() again, all of which are described above. Also I check for SIGINT (C-c) by using the signal function and then the signal handler. If the program is in udp mode I send a "BYE\n" message to the server and close then connection before exiting the program, if in udp mode I just close the connection and exit the program.
## Usage/Examples
- testing make:
![Picture 3](/img/testingmake.png "make test")
- testing parameters:
![Picture 4](/img/testingparametrs.png "parameters test")
- testing tcp:
![Picture 5](/img/testingtcp.png "tcp test")
- testing udp:
![Picture 6](/img/testingUdp.png "udp test")
## sources
- IPK lectures
- stubs fit git IPK
- [link] https://www.tutorialspoint.com/data_communication_computer_network/user_datagram_protocol.htm
- [link] https://www.geeksforgeeks.org/user-datagram-protocol-udp/
- [link] https://en.wikipedia.org/wiki/Transmission_Control_Protocol
- [link] https://www.geeksforgeeks.org/what-is-transmission-control-protocol-tcp/

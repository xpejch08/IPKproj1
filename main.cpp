//**********************************************//
//              IPK project 1                   //
// PROJECT: IPK client side for TCP/UDP server  //
// AUTHOR: xpejch08, Štěpán Pejchar             //
// DATE: 21.03.2023                             //
// DATE: 21.03.2023                             //
//**********************************************//

#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <regex>

#define BUFFER_SIZE 1024

/**
 * @class helpFunctions
 * @brief this class contains the checkArgs method that checks the command line parameters
 */
class helpFunctions{
public:
    /**
     * @brief checks parameters
     * @param argc
     * @param argv
     * @return zero if run correctly else the program ends
     */
    int checkArgs(int argc, char *argv[]){
        if(argc != 7){
            std::cerr << "Wrong number of arguments bye!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        //going through all the command line arguments
        for(int i = 0; i < argc; ++i) {
            if(i == 0){

            }
            else if(i == 1){
                if(strcmp(argv[i], "-h") != 0){
                    fprintf(stderr, "parameter %d is incorrect\n", i);
                    std::exit(EXIT_FAILURE);
                }
            }
            else if(i == 2){
                //using regex to checks if ipv4 adress is correct
                if(!std::regex_match(argv[i], std::regex("^((25[0-5]|(2[0-4]|1\\d|[1-9]|)\\d)\\.?\\b){4}$"))){
                    fprintf(stderr, "parameter %d is incorrect\n", i);
                    std::exit(EXIT_FAILURE);
                }
            }
            else if(i == 3){
                if(strcmp(argv[i], "-p") != 0){
                    fprintf(stderr, "parameter %d is incorrect\n", i);
                    std::exit(EXIT_FAILURE);
                }
            }
            else if(i == 4){
                //using regex to check if the param is a 4 digit number
                if(!std::regex_match(argv[i], std::regex("^\\d{4}$"))){
                    fprintf(stderr, "parameter %d is incorrect\n", i);
                    std::exit(EXIT_FAILURE);
                }
            }
            else if(i == 5){
                if(strcmp(argv[i], "-m") != 0){
                    fprintf(stderr, "parameter %d is incorrect\n", i);
                    std::exit(EXIT_FAILURE);
                }
            }
            else if(i == 6){
                if(strcmp(argv[i], "tcp") != 0 && strcmp(argv[i], "udp") != 0){
                    fprintf(stderr, "parameter %d is incorrect\n", i);
                    std::exit(EXIT_FAILURE);
                }
            }
        }
        return 0;
    }
private:
};
/**
 * @class clientTCP
 * @brief this class contains the methods used on the tcp client
 */
class clientTCP{
public:
    /**
     * @brief sets the command line arguments to the correct variables
     * @param argv
     * @param mode -> tcp
     */
    void setArgs(char *argv[], std::string *mode){
        serverHost = argv[2];
        *(mode) = argv[6];
        portNumber = atoi(argv[4]);

    }
    /**
     * @brief method used for debugging
     * @param mode -> tcp
     */
    void printArgs(std::string *mode){
        std::cout << serverHost << std::endl << portNumber << std::endl << *mode << std::endl;
    }

    /**
     * @brief is a method that obtains the server address.
     * I use the gethostbyname() function to obtain the server.
     * If the server isn't found it results with an error message printed on stderr and the program ends.
     */
    void getServerAdress(){
        if((server = gethostbyname(serverHost)) == NULL){
            std::cerr << "ERROR: no host with the name:" << serverHost << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    /**
     * @brief is a method that finds the IP adress of the server.
     */
    void findIp(){
        bzero((char *) &serverAdress, sizeof(serverAdress));
        serverAdress.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serverAdress.sin_addr.s_addr, server->h_length);
        serverAdress.sin_port = htons(portNumber);
    }

    /**
     * @brief method used for debugging
     */
    void printInfo(){
        std::cout << inet_ntoa(serverAdress.sin_addr) << ntohs(serverAdress.sin_port) << std::endl;
    }

    /**
     * @brief is a method that creates a new socket using the socket() function from the socket.h library.
     * The socket is of domain AF_INET and the type SOCK_STREAM If the socket creation fails,
     * an error message is printed on the stderr
     */
    void createSocket(){
        if ((cSocket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
        {
            std::cerr << "ERROR: problem with creating socket" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    /**
     * @brief is a method that reads the message the user writes on the stdin into the buffer,
     * then it connects to the server, using the connect function from the socket.h library.
     * It connects using the created socket and the server address. The connection occurs only once before disconnecting.
     * If the connection fails an error message is printed on the stderr and the program ends.
     */
    void readMessageFromUser(){
        explicit_bzero(buffer, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);

        //checking if we are connected or once, then increasing first -> connection happens only once
        if(first == 0) {
            first++;
            if (connect(cSocket, (const struct sockaddr *) &serverAdress, sizeof(serverAdress)) != 0) {
                fprintf( stderr, "ERROR: connect");
                exit(EXIT_FAILURE);
            }
        }
    }

    /**
     * @brief is a method that sends the message copied into the buffer from the stdin to the server using
     * the send() function defined in the socket.h library.
     * If the send fails the program writes an error message on the stderr and ends itself.
     */
    void sendMessage(){
        byteStx = send(cSocket, buffer, strlen(buffer), 0);
        if (byteStx < 0) {
            std::cerr << "ERROR: Sent message gone wrong";
        }
    }

    /**
     * @brief method recieves the message from the server using the read()function defined int the socket.h library.
     * If the read fails the program prints an error message on the stderr and ends itself.
     * It then prints the message out on the stdin.
     * If the message is "BYE\n" the connection between the client and the server ends and the program is ended.
     * The method also contains an infinite loop that checks whether or not the whole message was recieved.
     * If the message is as long as BUFFER_SIZE or it finds an endline the loop ends.
     */
    void recieveMessage(){
        explicit_bzero(buffer, BUFFER_SIZE);
        byteSrx = 0;
        while (true) {
            //variable that counts how much bytes of memory the client read
            int bytesRead = read(cSocket, buffer + byteSrx, BUFFER_SIZE - byteSrx);
            if (bytesRead < 0) {
                std::cerr << "ERROR: Recieved wrong message";
                exit(EXIT_FAILURE);
            }
            byteSrx += bytesRead;
            if (byteSrx >= BUFFER_SIZE || buffer[byteSrx - 1] == '\n') {
                break;
            }
        }
        printf("%s", buffer);
        //if the message from the server says bye the socket closes and the program ends
        if(strcmp(buffer, "BYE\n") == 0){
            close(cSocket);
            exit(EXIT_SUCCESS);
        }
    }

    /**
     * @brief method that cleans the buffer
     */
    void cleanBuff(){
        bzero(buffer, BUFFER_SIZE);
    }
private:
    /**
     * cSocket -> number of the socket, byteStx -> amount of bytes sent to the server,
     * byteSrx -> number of bytes recieved from the server, first -> flag for checking if I have to connect,
     */
    int cSocket, portNumber, byteStx, byteSrx;
    int first = 0;
    const char* serverHost;
    struct hostent *server;
    struct sockaddr_in serverAdress;
    char buffer[BUFFER_SIZE];
};

class clientUDP{
public:

    /**
     * @brief is a method used for setting the command line arguments
     * @param argv
     * @param mode -> udp
     */
    void setArgs(char *argv[], std::string *mode){
        serverHost = argv[2];
        *(mode) = argv[6];
        portNumber = atoi(argv[4]);

    }

    /**
     * @brief is a method used for debugging to check if all the arguments are correct
     * @param mode -> udp
     */
    void printArgs(std::string *mode){
        std::cout << serverHost << std::endl << portNumber << std::endl << *mode << std::endl;
    }

    /**
     * @brief  is a method that obtains the server agres. I use the gethostbyname() function to obtain the server.
     * If the server isn't found it results with an error message printed on stderr and the program ends.
     */
    void getServerAdress(){
        if((server = gethostbyname(serverHost)) == NULL){
            std::cerr << "ERROR: no host with the name:" << serverHost << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    /**
     * @brief is a method that finds the IP address of the server.
     */
    void findIp(){
        bzero((char *) &serverAdress, sizeof(serverAdress));
        serverAdress.sin_family = AF_INET;
        bcopy((char *)server->h_addr, (char *)&serverAdress.sin_addr.s_addr, server->h_length);
        serverAdress.sin_port = htons(portNumber);
    }

    /**
     * @brief is a method used for debugging, it prints the info about the server depending on the IP adress.
     */
    void printInfo(){
        std::cout << inet_ntoa(serverAdress.sin_addr) << ntohs(serverAdress.sin_port) << std::endl;
    }

    /**
     * @brief  is a method that creates a new socket using the socket() function from the socket.h library.
     * The socket is of domain AF_INET and type SOCK_DGRAM.
     * If the socket creation fails an error message is printed on the stderr and the program is ended.
     */
    void createSocket(){
        if ((cSocket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
        {
            std::cerr << "ERROR: problem with creating socket" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    /**
     * @brief is a method that reads the message the user writes on the stdin into the buffer,
     * the message must then be formatted as the protocol defines. I concat 3 bytes to the beginning of the buffer.
     * I then set the first one to a zero, because the opcode for request is 0 and then set the second one to the length of the message.
     */
    void readMessageFromUser(){
        bzero(buffer, BUFFER_SIZE);
        bzero(message, BUFFER_SIZE);
        while(!strcmp(message, "\0") || !strcmp(message, "\n")){
            fgets(message, BUFFER_SIZE, stdin);
        }
        char bufferByte1 = 48;
        char length = (char)(strlen(message)-1);
        //concatenating 3 bytes for the opcode, payload length and statuscode
        strncat(buffer, &bufferByte1, 1);
        strncat(buffer, &length, 2);
        strcat(buffer, message);
        bLen = strlen(buffer);
        //setting the first byte to ascii 0
        buffer[0] = '\0';
        buffer[bLen-1] = '\0';
        bLen--;

    }

    /**
     * @brief is a method that sends the message copied into the buffer from the stdin after formatting it,
     * to the server using the sendto() function defined in the socket.h library.
     * If the send fails the program writes an error message on the stderr and ends itself.
     */
    void sendMessage(){
        serverLen = sizeof(serverAdress);
        byteStx = sendto(cSocket, buffer, bLen, 0, (struct sockaddr *) &serverAdress, serverLen);
        if (byteStx < 0) {
            std::cerr << "ERROR: Sent message gone wrong";
        }
    }

    /**
     * @brief method recieves the message from the server using the recvfrom() function defined int the socket.h library.
     * If the read fails the program prints an error message on the stderr and ends itself.
     * Otherwise it prints the message out on the stdin.
     */
    void recieveMessage(){
        byteSrx = recvfrom(cSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &serverAdress, &serverLen);
        if (byteSrx < 0) {
            std::cerr << "ERROR: Recieved wrong message";
            exit(EXIT_FAILURE);
        }
        //if first byte is ascii 1 we recieved an error
        if(buffer[1] == '\1'){
            printf("ERR: %s\n", buffer+3);
        }
        else{
            printf("OK: %s\n", buffer+2);
        }
    }

    /**
     * @brief method used for cleaning the buffer
     */
    void cleanBuff(){
        bzero(buffer, BUFFER_SIZE);
    }



private:
    /**
     * cSocket -> number of the socket, byteStx -> amount of bytes sent to the server,
     * byteSrx -> number of bytes recieved from the server, first -> flag for checking if I have to connect,
     * serverLen -> used for containing the length of the serverAdress, blen -> used for storing the buffer length
     */
    int cSocket, portNumber, byteStx, byteSrx, bLen;
    int first = 0;
    socklen_t serverLen;
    const char* serverHost;
    struct hostent *server;
    struct sockaddr_in serverAdress;
    char buffer[1 + 2 + BUFFER_SIZE];
    char message[BUFFER_SIZE];
};

/**
 * @brief main body of program
 */
int main(int argc, char *argv[]) {
    //setting all the objects needed
    clientTCP client;
    clientUDP clientUDP;
    helpFunctions help;
    std::string mode;
    std::string tcp = ("tcp");
    //checking and setting the command line arguments
    help.checkArgs(argc, argv);
    client.setArgs(argv, &mode);
    clientUDP.setArgs(argv, &mode);


    //if mode is tcp we go into this branch, else we know the mode is udp because the args have been checked
    if(mode.compare(tcp) == 0) {
        //part that needs to happen only once
        client.getServerAdress();
        client.findIp();
        client.createSocket();
        //infinite loop waiting for the server to send a BYE message and then terminating the connection and program
        while (1) {
            client.readMessageFromUser();
            client.sendMessage();
            client.recieveMessage();
            //client.cleanBuff();
        }
    }
    else{
        //part that needs to happen only once
        clientUDP.getServerAdress();
        clientUDP.findIp();
        clientUDP.createSocket();
        while(1){
            //infinite loop that ends only if user interrupts the program
            clientUDP.readMessageFromUser();
            clientUDP.sendMessage();
            clientUDP.cleanBuff();
            clientUDP.recieveMessage();
            clientUDP.cleanBuff();
        }

    }



    return 0;
}

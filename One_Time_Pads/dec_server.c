/*
Author: Nag Balijepalli
Date: 11/25/2020
Name: Server decryption 
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#define SIZE 70001

void error(const char *msg);
int charToInt(char c);
char IntToChar(int i);
void decryption(char message[],char key[], int sLen);


void setupAddressStruct(struct sockaddr_in* address, int portNumber){
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 
  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}


/*
 * Description: bruh why are u reading decription for the main()
 */
int main(int argc, char *argv[]){
    int socketFD;                   //Same variables from dec_client.c
    int portNumber; 
    int status;                
    int connectionSocket;                
    char buffer[SIZE];
    struct sockaddr_in clientAdd;   //client add
    struct sockaddr_in serverAdd;   // server add
    int val = 1;
    socklen_t clientSize;
    pid_t pid;

    //this is straight from the dec_client.c
    if (argc != 2) {
       fprintf(stderr,"Server: Incorrect number of arguments.\n", argv[0]);
       exit(0);
    }
    socketFD = socket(AF_INET,SOCK_STREAM,0);
    if(socketFD < 0){
        perror("SERVER: ERROR opening socket");
        exit(1);
    }
    setupAddressStruct(&serverAdd, atoi(argv[1]));
    if (bind(socketFD, (struct sockaddr *)&serverAdd, sizeof(serverAdd)) < 0)
        error("SERVER: ERROR on binding");
    // Start listening for connetions. Allow up to 5 connections to queue up

    while(1){
        listen(socketFD,5);
        clientSize = sizeof(clientAdd);
        connectionSocket = accept(socketFD,(struct sockaddr*)&clientAdd, &clientSize);
        if (connectionSocket < 0)
            error("DEC_SERVER: ERROR on Accept()");
        pid = fork();
        if(pid < 0){
            error("SERVER: Forking has FAILED. Use a spoon");
            exit(1);
        }
        if(pid == 0){
            memset(buffer,'\0',sizeof(buffer));
            int buffSize = sizeof(buffer);
            int fRead = 0;
            char *keyS;
            int newLine = 0;
            int i = 0;
            read(connectionSocket,buffer,sizeof(buffer)-1);
            if(strcmp(buffer,"dec") != 0){
                char output[] = "invalid";
                send(connectionSocket,output,sizeof(output),0);
                exit(2);
            } else {
                char output[] = "dec_server";
                send(connectionSocket,output, sizeof(output),0);
            }
            memset(buffer,'\0',sizeof(buffer));
            char *buffer_2 = buffer;
            while(1){  
                fRead = read(connectionSocket,buffer_2,buffSize);
                if(fRead == 0)
                    break;
                if(fRead < 0)
                    error("SERVER: Error reading from socket.");
                while(i < fRead){
                    if (buffer_2[i] == '\n'){
                        newLine++; // the first line will go into the conditional
                        if(newLine == 1)
                            keyS = buffer_2+i+1;
                    }
                    i++;
                }
                if(newLine == 2) //the next line #2 is a signal for termination
                    break;
                buffer_2 += fRead;
                buffSize -= fRead;
            }
            char message[SIZE];
            memset(message,'\0',sizeof(message));
            strncpy(message,buffer,keyS - buffer);
            decryption(message,keyS,strlen(message));
            send(connectionSocket,message,sizeof(message),0);
        }
        close(connectionSocket);
        while(pid > 0){ // to make sure the children finish
            pid = waitpid(-1,&status,WNOHANG); // from previous assignments
        }
    }
    close(socketFD);
    return 0;
}
/*
 * Description: Error message
 */
void error(const char *msg) {
  perror(msg);
  exit(1);
} 
/*
 * Decription: Don't judge, this way to change from char to int was easier.
 */
int charToInt(char c){
    char* alphabets = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";     //From keygen alphabets
    int i = 0;
    while(i < 27){ //extra space so its 27
        if(c == alphabets[i])
            return i;
        i++;
    }
    return -1; // if it fails
}
/*
 * Description: does the opposite of charToInt
 */
char IntToChar(int i){
    char* alphabets = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";     //From keygen alphabets
	if (i < 0 || 27 < i)	    //If number is invalid then just return a lowercase	
		return 'a';
	return alphabets[i]; // the valid charector
}
/*
 * Decription: decrpts the server side of things
 */
void decryption(char message[],char key[], int sLen){
    int i = 0;
    int fNum,keyNum, decNum;
    sLen = strlen(message)-1;
    while(i < sLen){
        fNum = charToInt(message[i]);
        keyNum = charToInt(key[i]);
        decNum = (fNum - keyNum);
        if(decNum < 0){
            decNum += 27;
        }
        decNum = decNum%27;
        message[i] =  IntToChar(decNum);
        i++;
    }
    message[i] = '\0';
    return;
}


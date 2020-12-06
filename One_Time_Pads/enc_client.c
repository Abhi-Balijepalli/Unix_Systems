/*
Author: Nag Balijepalli
Date: 11/25/2020
Name: Client Encryption
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <ctype.h>
#include <netinet/in.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#define SIZE 70001

void sendFile(char* filename, int socketFD, int fLen);

void setupAddressStruct(struct sockaddr_in* address,int portNumber, char* hostname){
  memset((char*) address, '\0', sizeof(*address)); 
  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Get the DNS entry for this host name
  struct hostent* hostInfo = gethostbyname(hostname); 
  if (hostInfo == NULL) { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  memcpy((char*) &address->sin_addr.s_addr, hostInfo->h_addr_list[0],hostInfo->h_length);
}

int main(int argc, char *argv[]){
    int socketFD,optValue,charsRead;
    struct sockaddr_in serverAdd;
    struct hostent* serverInfo;
    char buffer[SIZE];

    if (argc != 4) {
       fprintf(stderr,"Requires 4 arguments.\n", argv[0]);
       exit(0);
    }
    socketFD = socket(AF_INET,SOCK_STREAM,0); //setting up the stocke
    if(socketFD < 0){
        perror("Client: ERROR opening socket");
        exit(1);
    }   
    // copy in the serverAddressStruct function, from lecture.
    int portNumber = atoi(argv[3]);
    // optValue = 1;
    setupAddressStruct(&serverAdd,portNumber,"localhost");
    if(connect(socketFD,(struct sockaddr *) &serverAdd, sizeof(serverAdd)) < 0){ 
        perror("Error connecting."); // if you can't connect to the server
        exit(1);
    }
    char auth[] = "enc";
    write(socketFD,auth,sizeof(auth));
    read(socketFD,buffer,sizeof(buffer));
    // buffer[strcspn(buffer, "\n")] = '\0'; 
    if(strcmp(buffer,"enc_server") != 0){
        perror("CLIENT: Error connecting to the server on the given port. \n");
        exit(2);
    }
    // int fileSize = get_flen(argv[1]);
    // int keySize = get_flen(argv[2]);
    long key = open(argv[2], O_RDONLY);                  //Get text, key, and the sizes of both
    long keySize = lseek(key, 0, SEEK_END);
    
    long text = open(argv[1], O_RDONLY);
    long fileSize = lseek(text, 0 , SEEK_END);
    if(fileSize > keySize){
        fprintf(stderr,"Error: You need a longer key.");
        exit(1);
    }
    int c = open(argv[1],'r');
    while(read(c,buffer,1) != 0){
        // https://www.programiz.com/c-programming/examples/alphabet
        if(buffer[0] != ' ' && (buffer[0] < 'A' || buffer[0] > 'Z')){
            if(buffer[0] != '\n'){
                fprintf(stderr,"%s  - Contains invalid charectors \n", argv[1]);
                exit(EXIT_FAILURE);
            }
        }
    }
    memset(buffer,'\0',sizeof(buffer));
    sendFile(argv[1],socketFD,fileSize);
    sendFile(argv[2],socketFD,keySize);
    charsRead = recv(socketFD,buffer,sizeof(buffer)-1,0);
    if(charsRead < 0){
        perror("Error reading the socket");
        exit(1);
    }
    printf("%s\n",buffer); // print the buffer, and close the socket opened by the client
    close(socketFD);
    return 0;
}

void sendFile(char* filename, int socketFD, int f_len){
    FILE* f = fopen(filename, "r");         //Set up a file to read
    char buffer[SIZE];
    memset(buffer,'\0',sizeof(buffer));
    int bytes_read;
    while((fread(buffer, sizeof(char), f_len, f)) > 0){
        if((bytes_read = send(socketFD, buffer, f_len, 0)) < 0){ 
            break;
        }
        memset(buffer,'\0',sizeof(buffer));  
    }
    if(bytes_read == SIZE){ 
        send(socketFD, "0", 1, 0); //send 0 if the bytes being read are equal to the size (7001)
    }
    fclose(f);   
    return;
}
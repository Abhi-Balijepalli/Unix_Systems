/*
Author: Nag Balijepalli
Date: 11/25/2020
Name: Client Decryption
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define SIZE 70001

void sendFile(char* filename, int clientSocket, int fLen);
void setupAddressStruct(struct sockaddr_in* address,int portNumber, char* hostname);
int get_flen(char *f);

int get_flen(char *f){
    int chars;
	int count = 0;
	FILE* file = fopen(f, "r");
    while (1) {
        chars = fgetc(file);
        if (chars == '\0' || chars == '\n')
            break;
        //https://www.programiz.com/c-programming/library-function/ctype.h/isupper
		if(!isupper(chars) && chars != ' '){
			error("File contains bad characters. \n");
		}
        ++count;
    }
	fclose(file);
	return count;
}

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
    int clientSocket,portNumber,optValue,charsRead;
    struct sockaddr_in serverAdd;
    char buffer[SIZE];
    optValue = 1;
    char auth[] = "dec";
    memset(buffer,'\0',sizeof(buffer));
    if (argc != 4) {
       fprintf(stderr,"Requires 4 arguments.\n", argv[0]);
       exit(0);
    }
    portNumber = atoi(argv[3]); //we know last argument is the port number
    clientSocket = socket(AF_INET,SOCK_STREAM,0); //setting up the stocket
    if(clientSocket < 0){
        perror("Dec_client: ERROR opening socket");
        exit(1);
    }
    // setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR, &optValue, sizeof(int));
    setupAddressStruct(&serverAdd,portNumber,"localhost");

    if(connect(clientSocket,(struct sockaddr *) &serverAdd, sizeof(serverAdd)) < 0){ 
        perror("Dec_client: Error connecting."); // if you can't connect to the server
        exit(1);
    }
    write(clientSocket,auth,sizeof(auth));
    read(clientSocket,buffer,sizeof(buffer));
    if(strcmp(buffer,"dec_server") != 0){
        perror("Dec_Client: Error connecting to the server on the given port. \n");
        exit(2);
    }
    // https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
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
    memset(buffer,'\0',sizeof(buffer));
    sendFile(argv[1],clientSocket,fileSize);
    sendFile(argv[2],clientSocket,keySize);
    charsRead = recv(clientSocket,buffer,sizeof(buffer)-1,0);
    if(charsRead < 0){
        perror("Dec_Client: Error reading the socket");
        exit(1);
    }
    printf("%s\n",buffer); // print the buffer, and close the socket opened by the client
    close(clientSocket);
    return 0;
}

void sendFile(char* filename, int clientSocket, int f_len){
    FILE* f = fopen(filename, "r");         //Set up a file to read
    char buffer[SIZE];
    memset(buffer,'\0',sizeof(buffer));
    int bytes_read;
    while((f_len = fread(buffer, sizeof(char), SIZE, f)) > 0){
        if((bytes_read = send(clientSocket, buffer, f_len, 0)) < 0){ 
            break;
        }
        memset(buffer,'\0',sizeof(buffer));  
    }
    if(bytes_read == SIZE){ 
        send(clientSocket, "0", 1, 0); //send 0 if the bytes being read are equal to the size (7001)
    }
    fclose(f);   
    return;
}
/*
Author: Nag Balijepalli
Date: 11/25/2020
Name: One-Time Pads ~ Encryption 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void Generate_Encryption(int, char const*);

int main(int argc, char const *argv[]) {
  Generate_Encryption(argc, argv[1]); //I have Generate_Encryption becuase it sounds cool. 
  return 0;
}

void Generate_Encryption(int argc, char const* argv) {
  if(argc == 2) {
    int cLen,buffer,i,idx;
    char* alphabets;
    srand(time(NULL)); // fixes the issue where i don't have repeating random gen
    cLen = atoi(argv);                                              
    buffer = cLen + 1;                                              
    alphabets = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "; //need empty space                             
    char keyGen[buffer];
    while(i < cLen){
        idx = rand() % 27;                                               
        keyGen[i] = alphabets[idx];  //keygen logic
        i+=1;
    }
    keyGen[cLen] = '\0'; // Last charector                                           
    printf("%s\n", keyGen);                                                   
  }
  else {
    fprintf(stderr, "Add more cmd line args\n");                  
    exit(0);
  }
}

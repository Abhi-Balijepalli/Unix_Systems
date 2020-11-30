// Author: Nag Balijepalli
// Name: Multithreaded processing (Producer - Consumer)
// Date: 11/11/2020
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

#define SIZE 50
#define CHAR_SIZE 1000

int length_of_buffer = 0; //esentially each line in the buffer
int count_1 = 0;
int count_2 = 0;
int count_3 = 0;
//to store each line of the inputfile
char buffer_1[SIZE][CHAR_SIZE]; // store input
int prod_idx_1 = 0; //producer index
int con_idx_1 = 0; //consumer index

char buffer_2[SIZE][CHAR_SIZE]; // stores removed line seperator
int prod_idx_2 = 0;
int con_idx_2 = 0;

char buffer_3[SIZE][CHAR_SIZE]; // stored new array with "++" -> "^"
int prod_idx_3 = 0;
int con_idx_3 = 0;

// buffer 1:
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;
// buffer 2:
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;
// buffer 3:
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

//Buffers:
void buffer_1_put(char line_input[CHAR_SIZE]);
char* get_buff_1();

void buffer_2_put(char line_input[CHAR_SIZE]);
char* get_buff_2();

void buffer_3_put(char line_input[CHAR_SIZE]);
char* get_buff_3();

// 4 Threads:
void* store_input();
void* line_seperator();
void* plus_remover();
void* output();

// 1st thread, takes in stdin, and puts it into the 1st buffer (stops after "STOP\n")
void* store_input(){
    char line_1[CHAR_SIZE];
    while(fgets(line_1,CHAR_SIZE,stdin)){
        if(strcmp(line_1,"STOP\n") == 0){
            buffer_1_put(line_1); // this is my trigger to stop all other threads, from going into a blackhole.
            break;
        } else {
            buffer_1_put(line_1);
        }
    }
    return NULL;
}

// this function will remove the line-seperators with a space, trigger to end is "STOP\n".
void* line_seperator(){
    while(1) {
        char *each_line = get_buff_1();
        // printf(each_line);
        // fflush(stdout);
        if(strcmp(each_line,"STOP\n") == 0){
            buffer_2_put(each_line);
            break;
        }
        each_line[strlen(each_line)-1] = ' '; // '\n' gets replaced by ' '
        buffer_2_put(each_line);
    }
    return NULL;
}

// this function removes the ++ from the buffer
void* plus_remover(){ // Think happy thoughts before you take a look at this function...
    int count = 0;
    while(1){
        int i = 0;
        char *each_line = get_buff_2();
        if(strcmp("STOP\n",each_line) == 0){ // trigger to end
            buffer_3_put(each_line);
            break;
        }
        while(i < strlen(each_line)) {
            if(each_line[i] == '+' && each_line[i+1] == '+'){
                each_line[i] ='^'; 
                each_line[i+1] = '&'; //marking every 2nd '+' as visited node '&' 
                //+++++
            }
            i++;
        }
        int x = 0;
        int y = 0; 
        // iterate through the list and remove the visited node '&'
        for(x = 0, y = 0; x < strlen(each_line); x++){
            if(each_line[x] != '&'){
                each_line[y] = each_line[x]; // removal algo
                y++;
            }
        }
        if(y == 0){
            each_line[strlen(each_line)-1] = '\0';
        } else {
            each_line[y] = '\0';
        }
        buffer_3_put(each_line);
    }
    return NULL;
}

// Output 80 charectors per line, until it meets the "STOP"
void* output(){
    char *each_line;
    while(1) {
        char output_str[CHAR_SIZE];
        each_line = get_buff_3();
        // write(1,each_line,strlen(each_line));
        if(strcmp("STOP\n",each_line) == 0) //trigger
            break;
        if(output_str != NULL)
            strcat(output_str,each_line);
        else
            strcpy(output_str,each_line);

        while(strlen(output_str) >= 80){
            for(int x = 0; x < 80; x++){
                printf("%c", output_str[x]);
                fflush(stdout);
            }
            memmove(output_str,output_str+80,strlen(output_str)-80+1); // cut the 80 charectors
            printf("\n");
            fflush(stdout);
        }
    }
    return NULL;
}

// BUFFER 1 ------------------
void buffer_1_put(char line_input[CHAR_SIZE]){
    pthread_mutex_lock(&mutex_1);
    strcpy(buffer_1[prod_idx_1],line_input);
    // printf("%s",buffer_1[prod_idx_1]);
    prod_idx_1 = prod_idx_1 + 1;
    count_1+=1;
    pthread_cond_signal(&full_1);
    pthread_mutex_unlock(&mutex_1);
}
char* get_buff_1() {
    pthread_mutex_lock(&mutex_1);
    while(count_1 == 0)
        pthread_cond_wait(&full_1, &mutex_1);
    char* line = buffer_1[con_idx_1];
    // printf("%s END OF \n",line);
    con_idx_1 = con_idx_1 + 1;
    count_1--;
    pthread_mutex_unlock(&mutex_1);
    return line;
}

// BUFFER 2 ------------------
void buffer_2_put(char line_input[CHAR_SIZE]){
    pthread_mutex_lock(&mutex_2);
    strcpy(buffer_2[prod_idx_2],line_input);
    prod_idx_2 = prod_idx_2 + 1;
    count_2++;
    pthread_cond_signal(&full_2);
    pthread_mutex_unlock(&mutex_2);
}
char* get_buff_2(){
    pthread_mutex_lock(&mutex_2);
    while (count_2 == 0)
        pthread_cond_wait(&full_2, &mutex_2);
    char* line = buffer_2[con_idx_2];
    con_idx_2 = con_idx_2 + 1;
    count_2--;
    pthread_mutex_unlock(&mutex_2);
    return line;
}

// BUFFER 3 ------------------
void buffer_3_put(char line_input[CHAR_SIZE]){
    pthread_mutex_lock(&mutex_3);
    strcpy(buffer_3[prod_idx_3],line_input);
    prod_idx_3 = prod_idx_3 + 1;
    count_3++;
    pthread_cond_signal(&full_3);
    pthread_mutex_unlock(&mutex_3);
}
char* get_buff_3(){
    pthread_mutex_lock(&mutex_3);
    while (count_3 == 0)
        pthread_cond_wait(&full_3, &mutex_3);
    char* line = buffer_2[con_idx_3];
    con_idx_3 = con_idx_3 + 1;
    count_2--;
    pthread_mutex_unlock(&mutex_3);
    return line;
}


int main(int argc, char*argv[]) {
    pthread_t input_t,line_seperator_t,plus_remover_t,output_t;
    
    pthread_create(&input_t,NULL,store_input,NULL);
    pthread_create(&line_seperator_t,NULL,line_seperator,NULL);
    pthread_create(&plus_remover_t,NULL,plus_remover,NULL);
    pthread_create(&output_t,NULL,output,NULL);

    pthread_join(input_t,NULL);
    pthread_join(line_seperator_t,NULL);
    pthread_join(plus_remover_t,NULL);
    pthread_join(output_t,NULL);
    
    return EXIT_SUCCESS;

}
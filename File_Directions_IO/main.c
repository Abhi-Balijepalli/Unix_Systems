/*
Author: Abhi Balijepalli
Name: CS344 Assignment 2
Description: Files processing and creation.
*/
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#define PREFIX "movies_"

struct movies {
    char *title;
    char *year;
    char *languages;
    char *rating;
    struct movies *next;
};

// declarations only for ones, that conflict function types
void file_selection();
void year_writer(struct movies *yearMovies, char* year_given, char* dir_name);

void* concatination(char *uno, char *dos) {
    char *result = malloc(strlen(uno) + strlen(dos) + 1);
    strcpy(result, uno);
    strcat(result, dos);
    return result;
}

struct movies *createMovie(char *currLine) {
    struct movies *currMovie = malloc(sizeof(struct movies));

    // For use with strtok_r
    char *saveptr;

    // The first token is the ONID
    char *token = strtok_r(currLine, ",", &saveptr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->year, token);

    // The next token is the lastName
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->languages = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->languages, token);

    // The next token is the firstName
    token = strtok_r(NULL, "\n", &saveptr);
    currMovie->rating = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->rating, token);

    // The last token is the major

    // Set the next node to NULL in the newly created student entry
    currMovie->next = NULL;

    return currMovie;
}

struct movies *process_file(char* file) {
    FILE *movieFile;
    // printf(file);
    if ((movieFile = fopen(file,"r"))!=NULL){
        printf("Now processing chosen file %s \n", file);
        char *currLine = 0;
        size_t len = 0;
        ssize_t nread;
        char *token;
        int count = 0;
        struct movies *head = NULL;
    // The tail of the linked list
        struct movies *tail = NULL;
        // printf("segfault check #2");
        getline(&currLine,&len,movieFile);
        // printf("segfault check #3");
        while ((nread = getline(&currLine, &len, movieFile)) != -1) {
            count +=1;
            // Get a new student node corresponding to the current line
            struct movies *newNode = createMovie(currLine);
            // Is this the first node in the linked list?
            if (head == NULL){
                head = newNode;
                tail = newNode;
            }
            else {
                tail->next = newNode;
                tail = newNode;
            }
        }
        // printf("Segfault check #4");
        free(currLine);
        fclose(movieFile);
        return head;
    } else {
        printf("This file doesn't exist, try again. \n");
        file_selection();
    }
}

// makes the name of the directories, only gets called every you choose file to process
char* dir_name_func(){
    int choice;
    int max_bound = 100000;
    char *pathname = "balijepn.movies.";
    srand(time(NULL)); //Had to use srand() becuase rand() was generating the same number everytime
    int r = rand() % max_bound;
    char rand_num[7];
    sprintf(rand_num,"%d",r);
    char* fin = concatination(pathname,rand_num);
    // printf("%s \n",fin);
    return fin;
    // free(fin);
}

// The most annoying function i have ever written, it makes dirs, processes files, and calls my year_writer()
void dir_writer(char *fileName){
    // printf(fileName);
    struct movies *linkedlist = process_file(fileName); //file gets processes only after choosing
    char* dir_name = dir_name_func();
    char *fin_dir = concatination("./",dir_name);
    // printf("Making the directory");
    mkdir(fin_dir,0750);
    // free(dir_maker);
    struct movies *curr = linkedlist;
    // printf("after creating curr");
    while(curr != NULL){
        year_writer(linkedlist, curr->year, fin_dir); // every year is being passed for comparision
        // printf("Iterating");
        curr = curr->next;
    }

}

//creates *.txt inside corresponding directories
void year_writer(struct movies *yearMovies, char* year_given, char* dir_name){
    // DIR* currDir; 
    // currDir = opendir(dir_name);
    struct movies *curr = yearMovies;
    int fd;
    char *dirName = concatination(dir_name,"/");
    char *file_type = concatination(year_given,".txt");
    char *newFileName =concatination(dirName,file_type); 
    // char *newFileName = year_given;
    if (access(newFileName, F_OK ) == -1) { //only creates a file, if it doesn't exist in dir
        fd = open(newFileName, O_RDWR | O_CREAT | O_TRUNC, 0640);
        while(curr != NULL){
            if(!strcmp(year_given,curr->year)){
                char *message = concatination(curr->title, "\n");
                write(fd, message, strlen(message) + 1);
                // write(fd,'\n',2);
            }
            curr = curr->next;
        }
    }
    // closedir(currDir); 
}

// finds the largest file in source dir, and processes that.
void larger_file() {
    DIR* currDir = opendir(".");
    struct dirent *L_aDir;
    struct stat L_dirStat;
    int i = 0;
    char *L_entryName;

    while((L_aDir = readdir(currDir)) != NULL){
        if(strncmp(PREFIX,L_aDir->d_name, strlen(PREFIX)) == 0){
            stat(L_aDir->d_name,&L_dirStat);
            if (L_dirStat.st_size >= i){
                i = L_dirStat.st_size;
                // printf("The size: %d", i);
                L_entryName = L_aDir->d_name;
            }
        }
    }
    // printf("Now processing chosen file %s \n", L_entryName);
    dir_writer(L_entryName);
    closedir(currDir);
    
}

// finds the smallest file in source dir, and processes that.
void smaller_file() {
    DIR* currDir = opendir(".");
    struct dirent *S_aDir;
    struct stat S_dirStat;
    int i = 0;
    char *S_entryName;

    while((S_aDir = readdir(currDir)) != NULL){
        if(strncmp(PREFIX,S_aDir->d_name, strlen(PREFIX)) == 0){
            stat(S_aDir->d_name,&S_dirStat);
            i = S_dirStat.st_size;
            S_entryName = S_aDir->d_name;
            if (S_dirStat.st_size < i){
                i = S_dirStat.st_size;
                // printf("The size: %d", i);
                S_entryName = S_aDir->d_name;
                // printf("The small file name: %s", S_entryName);
            }
        }
    }
    // printf("Now processing chosen file %s \n", S_entryName);
    dir_writer(S_entryName);
    closedir(currDir);
    
}

//this is the function that deals with a custom file, not smallest or largest
void custom_file() {
    char fileName[100];
    printf("Enter the name of the file: ");
    scanf("%s", fileName);
    // printf(fileName);
    dir_writer(fileName);
}


void file_selection(){
    int x;
    printf("Which file do you want to process ? \n");
    printf("1. to pick the largest file \n");
    printf("2. to pick the smallest file \n");
    printf("3. to specify the name of a file \n");
    printf("Enter a choice between 1 to 3: ");
    scanf("%d",&x);

    if(x == 1){
        larger_file();
    } else if (x == 2) {
        smaller_file();
    } else if (x == 3) {
        custom_file();
    } else {
        printf("You have entered a wrong value, try again. \n");
        file_selection();
    }
}   


int main(int argc, char*argv[]){
    int choice;
    while(1){
        printf("\n");
        printf("1. Select file to process \n");
        printf("2. Exit the program \n");
        printf("Enter a choice 1 or 2: ");
        scanf("%d", &choice);

        if(choice == 1){
            file_selection();
        } else if (choice == 2){
            printf("Bye bye! \n");
            break;
        } else {
            printf("That choice does not exist, try again. \n");
        }
    }
    return 0;
}

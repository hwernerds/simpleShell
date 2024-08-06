#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define NORMAL 0
#define NEWLINE 1
#define NULLBYTE 2
#define REDIR_IN 3
#define REDIR_OUT 4

char* get_token(char** arg1, int* arg2) {
  // Increment pointer past initial white spaces
  while (isspace(**arg1) && **arg1 != '\n') {
    (*arg1)++;
  }

  //If it is the symbol, skip it and proceed to the next token after the white spaces
  if(**arg1 == '<'){
    (*arg1)++;
    while (isspace(**arg1) && **arg1 != '\n') {
      (*arg1)++;
    }
    *arg2 = REDIR_IN;
  }

  //If it is the symbol, skip it and proceed to the next token after the white spaces
  if(**arg1 == '>'){
    (*arg1)++;
    while (isspace(**arg1) && **arg1 != '\n') {
      (*arg1)++;
    }
    *arg2 = REDIR_OUT;
  }

  // If *arg1 is a sequence of non-white space characters
  if (**arg1 != '\n' && **arg1 != '\0') {
    //find the lenght of the array
    int len = 0; 
    char *p = *arg1;
    while(!isspace(*p) && *p != '\0'){
      len++;
      p++;
    }

    //dynamically allocate the array and assing arg1 to tok
    char *tok = (char*) malloc(len+1);
    int i = 0;
    while (i < len) {
      tok[i] = **arg1;
      i++;
      (*arg1)++;
    }
    tok[i++] = '\0';

    // Assign correct value to *arg2 before returning tok
    if(*arg2 == REDIR_IN){
      return tok;
    }
    if(*arg2 == REDIR_OUT){
      return tok;
    }

    if(**arg1 == '\n') {
      *arg2 = NEWLINE;
    } else if (**arg1 == '\0') {
      *arg2 = NULLBYTE;
    } else {
      *arg2 = NORMAL;
    }

    return tok;
  } else {
    // Assign correct value to *arg2 before returning null in case it is not normal
    if (**arg1 == '\n') {
      *arg2 = NEWLINE;
    } else if (**arg1 == '\0') {
      *arg2 = NULLBYTE;
    }

    return NULL;
  }
}

int main(int argc, char **argv, char **envp) {
  char *line = (char*) malloc(1000);
  char *line2 = line; // Need to keep the original location for free
  size_t size = 1000;
  int ret; //int to check if system calls are returning correctly
  char** arr = (char**) malloc(sizeof(char*)*5); //array to hold the input of the program
  char **redir = (char**) malloc(sizeof(char*)*3);
  char *token; //pointer that holds each character of the input at a time
  int status; //status to tell the function when to stop adding or whehter we have to perform I/O redirect
  int ct; //count the number of tokens input
  int cpid; //pid of child
  int reset; //int used to reset arr so that the program can run until user performs EOF

  //while the user has not manually input EOF or EOF has not been reached in a file
  while((getline(&line2, &size, stdin)) != EOF){
    //reset variables

    ct = 0;
    reset = 0;
    redir[0] = NULL;
    redir[1] = NULL;
    //while the return value is not NULL
    while((token = get_token(&line2, &status))!=NULL){
      if(status == 3){
        redir[0] = token;
        continue;
      }
      if(status == 4){
        redir[1] = token;
        continue;
      }
      //leave space for the NULL at the end of the array
      if(ct == 4){
        printf("Too many inputs! Maximum of 4 inputs allowed! Running program with first 4 inputs.\n");
        break;
      }
      //set arr so that each index is one word input
      arr[ct] = token;
      ct++;
      if(status==1 || status==2){
        break;
      }
    }
    //make last index of arr null
    arr[ct] = NULL;
    redir[3] = NULL;

    //perform fork
    if((ret = fork())<0){
      perror("fork");
      return -1;
    }

    //if it is the parent, wait for the child to complete
    if(ret != 0){
      if((cpid = wait(&status))<0){
        perror("wait");
        return -1;
      }
    }

    //if it is the child, perform execve and return
    if(ret == 0){
      //if redir[0] is not null, redirect input
      if(redir[0]){
        close(0);
        if((ret = open(redir[0], O_RDONLY))<0){
          perror("open");
          return -1;
        } 
      }
      //if redir[1] is not null, redirect output
      if(redir[1]){
        close(1);
        if((ret = open(redir[1], O_WRONLY))<0){
          perror("open");
          return -1;
        }
      }
      if((ret = execve(arr[0], arr, envp)) < 0){
        perror("execve");
        return -1;
      }
    }

    //reset arr before going through the outer loop again
    while(arr[reset]){
      free(arr[reset]);
      reset++;
    }
    //reset redir if needed
    for(int i = 0; i < 2; i++){
      if(redir[i]){
        free(redir[i]);
      }
    }
    //reset line buffer
    free(line);
    char *line = (char*) malloc(1000);
    line2 = line;
  }

  //free dynamically allocated memory before returning from the program
  free(redir);
  free(line);
  free(arr);
  return 0;
}




/**
 * This file is a temporary c file used for testing functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

#include "LinkedList.h"

int main(int argc, char const *argv[]){
    char* str1 = "testCmd";
    init();

    char c;
    int i = 1000;
    int j = 1000;
    while(1){
        scanf("%d", &j);
        while(getchar()!='\n');
        if(j == 0){
            i++;
            listAppend(i,str1,1);
        } else {
            listRemove(j);
        }
        listPrint();
    }

    // char input[40];
    // pid_t pid = fork();
    // if (pid < 0){
    //     perror("Fork failed\n");
    //     return -1;
    // } else if (pid == 0) {
    //     // child process
    //     sleep(3);
    //     exit(0);
    //     return -1;
    // }

    // int status;
    // int options;
    // struct rusage usg;
    
    // printf("%d",pid);
    // for(int i = 0; i < 10; i++){
    //     sleep(1);
    //     int id = wait3(&status, 0, &usg);
    //     // int id = waitpid(-1, &status, WNOHANG);
    //     printf("%d\n", id);
    // }
    
    // // printf("wait the process\n");
    // return 0;
}

/**
 * CS3013 - C19
 * Project 1 - Midday Commander
 * Author: Rui Huang
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_INPUT 128
#define MAX_ARGS 32

char* commands[] = {"whoami", "last", "ls", NULL};
int argCounter = 0;

/**
 * Print the menu of Midday Commander program.
 */
void print_menu() {
    printf("\nG’day, Commander! What command would you like to run? \n");
    printf("   0. whoami  : Prints out the result of the whoami command \n");
    printf("   1. last    : Prints out the result of the last command \n");
    printf("   2. ls      : Prints out the result of a listing on a user-specified path \n");
}

/**
 * Print tthe statistics after executing a process
 */
void print_stats(double time, long majflt, long minflt) {
    printf("\n-- Statistics --\n");
    printf("Elapsed time: %.3f milliseconds\n", time);
    printf("Page Faults: %ld \n", majflt);
    printf("Page Faults (reclaimed): %ld \n", minflt);
}

/**
 * Read and check input from stdin, return 1 if exceeds the max length
 */
int readInput(char* str){
    int c;
    fgets(str, MAX_INPUT+3, stdin);
    if (strlen(str) > MAX_INPUT+1){
        printf("\nERROR: input exceeds the limit (128).\n");
        if(str[MAX_INPUT+1]!='\n') while((c=getchar()) != '\n' && c!= EOF); // clear the stdin
        return 1;
    }
    if (str[strlen(str)-1] == '\n')
        str[strlen(str)-1] = '\0';
    return 0;
}

/** TODO
 * Read and check input as arguments from stdin, return 1 if exceeds the max arg/input length
 */
int readInputArgs(char** pstr, char* str){
    if (strlen(str) != 0){
        char* delim = " ";
        pstr[argCounter++] = strtok(str, delim);
        while ((pstr[argCounter] = strtok(NULL, delim))) {
            if (argCounter>MAX_ARGS) {
                printf("ERROR: input arguments exceeds the limit (32).\n");
                return 1;
            }
            argCounter++;
        }
    }
    pstr[argCounter] = NULL;
    return 0;
}


int main(int argc, char const *argv[]){
    printf("===== Mid-Day Commander, v0 =====");

    while(!feof(stdin)){
        char* args[MAX_ARGS+2];
        char input[MAX_INPUT+3];
        argCounter = 0;

        print_menu();
        printf("Option?: ");
        // --- INPUT CHECK ---
        if(readInput(input)) continue;

        // --- PROCESS INPUTS ---
        int option = -1;
        // if(strlen(option)) while((c=getchar()) != '\n' && c!= EOF);
        sscanf(input, "%d", &option);

        if (option == 0){
            printf("\n-- Who Am I? --\n");
            args[0] = "whoami";
            args[1] = NULL;
        } else if (option == 1){
            printf("\n-- Last Logins --\n");
            args[0] = "last";
            args[1] = NULL;
        } else if (option == 2){
            char arginput[MAX_INPUT+3];
            char pathinput[MAX_INPUT+3];
            printf("\n-- Directory Listing --\n");
            args[argCounter++] = "ls";
            
            // get arguments
            printf("Arguments?: ");
            if(readInput(arginput)) continue;
            if(readInputArgs(args, arginput)) continue;
            printf("Path?: ");
            if(readInput(pathinput)) continue;
            if(readInputArgs(args, pathinput)) continue;

            printf("\n");
        } else {
            printf("\nInvalid command input, please use the options above.\n");
            continue;
        }

        // --- COLLECT INFO ---
        struct timeval start, end;
        struct rusage before, after;
        gettimeofday(&start, NULL);
        getrusage(RUSAGE_CHILDREN, &before);
        
        printf("command: \n");
        for(int i = 0; i <= argCounter; i++){
            printf("\t[%s]\n", args[i]);
        }
        printf("\n");

        // --- FORK PROCESS ---
        pid_t pid = fork();
        if (pid < 0){
            perror("Fork failed\n");
            return -1;
        } else if (pid == 0) {
            // child process
            execvp(commands[option], args);
            printf("Failed to execute command\n"); // shouldn't be here
            return -1;
        }
        // parent process
        wait(NULL);
        gettimeofday(&end, NULL);
        getrusage(RUSAGE_CHILDREN, &after);

        // --- PRINT STATS ---
        double time = ((double)end.tv_sec - (double)start.tv_sec)*1000 + ((double)end.tv_usec - (double)start.tv_usec)/1000;
        print_stats(time, after.ru_majflt - before.ru_majflt , after.ru_minflt - before.ru_minflt);
    }
    return 0;
}
/**
 * CS3013 - C19
 * Project 1 - Midday Commander
 * Author: Rui Huang
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 128
#define MAX_ARGS 32
#define MAX_CMDS 128

char** cmdList;
char* input;
char** args;
int argCounter = 0;
int cmdlen = 0;

void print_added_cmd(){
    for(int i = 0; i < cmdlen; i++){
        printf("   %d. %s\t: User added command\n", i+3, cmdList[i]);
    }
}

/**
 * Print the menu of Midday Commander program.
 */
void print_menu() {
    printf("\n=====================================================================");
    printf("\nG’day, Commander! What command would you like to run? \n");
    printf("   0. whoami\t: Prints out the result of the whoami command \n");
    printf("   1. last\t: Prints out the result of the last command \n");
    printf("   2. ls\t: Prints out the result of a listing on a user-specified path \n");
    print_added_cmd();
    printf("   a. add command\t: Adds a new command to the menu \n");
    printf("   c. change directory\t: Changes process working directory \n");
    printf("   e. exit\t: Leave Mid-Day Commander \n");
    printf("   p. pwd\t: Prints working directory \n");
    printf("Option?: ");
}

/**
 * Print the statistics after executing a process
 */
void print_stats(double time, long majflt, long minflt) {
    printf("\n-- Statistics --\n");
    printf("Elapsed time: %.3f milliseconds\n", time);
    printf("Page Faults: %ld \n", majflt);
    printf("Page Faults (reclaimed): %ld \n", minflt);
}

/**
 * Free the memory and exit the program
 */
void exitmc(){
    for(int i = 0; i < MAX_CMDS; i++){
        free(cmdList[i]);
    }
    free(cmdList);
    free(input);
    free(args);
    printf("Logging you out, Commander.\n");
    exit(0);
}

/**
 * Read and check input from stdin, return 1 if exceeds the max length
 */
int readInput(char* str){
    int c;
    // if input interrupted or EOF
    if (fgets(str, MAX_INPUT+3, stdin) == NULL){
        printf("\n -- No Input Detected. -- \n");
        exitmc();
    }
    // if input character > MAX_INPUT
    if (strlen(str) > MAX_INPUT+1){
        printf("\nERROR: input exceeds the limit (128).\n");
        if(str[MAX_INPUT+1]!='\n') while((c=getchar()) != '\n' && c!= EOF); // clear the stdin
        return 1;
    }
    // else valid input, replace the ending \n by \0
    if (str[strlen(str)-1] == '\n')
        str[strlen(str)-1] = '\0';
    return 0;
}

/** 
 * Read and check input as arguments from stdin, split the str to args and append
 * the args to the end of pstr. return 1 if exceeds the max arg/input length.
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

/**
 * Return if a string is a number
 */
int isnumber(char* str){
    int i;
    for (i = 0; i < strlen(str); i++){
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

/**
 * Read the command from user and add to command list
 */
int addCommand(){
    printf("\n-- Add a command --\n");
    printf("Command to add?: ");
    if(readInput(input)){
        printf("Error when reading the command.\n");
        return -1;
    }
    strcpy(cmdList[cmdlen++], input);
    printf("Okay, added with ID [%d]\n", cmdlen+2);
    return 0;
}

/**
 * Try to find the given option in preset commands or command list
 * return corresponding id if command is in command list
 * return -1 if command not found
 * return -2 if command is a/c/p
 */
int findCommand(char* str){
    int option;
    char optchar;
    int len = strlen(str);
    if (len == 0)
        return -1;

    // try to parse as int first
    if (isnumber(str)){
        sscanf(str, "%d", &option);
        return option;
    }

    // then try to parse as char
    if (len == 1) {
        sscanf(str, "%c", &optchar);
        if (optchar == 'a') {
            // 'a': add command
            addCommand();
        } else if (optchar == 'c') {
            // 'c': change working direcotry
            char wd[MAX_INPUT+3];
            printf("\n-- Change Directory --\n");
            printf("New Direcotry?: ");
            if(readInput(wd))
                return -2;
            if(chdir(wd))
                printf("Failed to change directory");
        } else if (optchar == 'e'){
            // 'e': exit program
            exitmc();
        } else if (optchar == 'p'){
            // 'p': print working direcotry
            char cwd[128];
            if (getcwd(cwd, 128) == NULL){
                printf("\nCurrent path length is too long to be printed.\n");
            } else {
                printf("\n-- Current Directory --\n");
                printf("Directory: %s\n", cwd);
            }
        } else {
            // not recognized character
            return -1;
        }
        // return -2 indicates performed a/c/p action
        return -2;
    }
    // otherwise this is an invalid input
    return -1;
}

/**
 * Run Midday Commander program.
 */
int main(int argc, char const *argv[]){
    printf("===== Mid-Day Commander, v1 =====");

    // allocate memories
    input = (char*) malloc(sizeof(char*) * (MAX_INPUT + 3));
    args = (char**) malloc(sizeof(char**) * (MAX_ARGS + 2));
    cmdList = (char**) malloc(sizeof(char*) * MAX_CMDS);
    for (int i = 0; i < MAX_CMDS; i++){
        cmdList[i] = (char*) malloc(sizeof(char) * (MAX_INPUT + 3));
    }

    while(1){
        // --- INITIALIZE ---
        argCounter = 0;
        print_menu();

        // --- INPUT CHECK ---
        if(readInput(input)) continue;
        int option = findCommand(input);

        // --- SELECT COMMAND ---
        if (option == 0){
            // 0: whoami
            printf("\n-- Who Am I? --\n");
            args[0] = "whoami";
            args[1] = NULL;
        } else if (option == 1){
            // 1: last
            printf("\n-- Last Logins --\n");
            args[0] = "last";
            args[1] = NULL;
        } else if (option == 2){
            // 2: ls
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
        } else if (option >= 3 && option < cmdlen+3){
            // user defined commands
            char cmdstr[MAX_INPUT+3];
            strcpy(cmdstr, cmdList[option-3]);
            printf("\n-- Command: %s --\n", cmdstr);
            readInputArgs(args, cmdstr);
        } else {
            // invalid option or a/c/p action
            if (option != -2){
                printf("\nInvalid command input, please use the options above.\n");
            }
            continue;
        }

        // --- COLLECT INFO ---
        struct timeval start, end;
        struct rusage before, after;
        gettimeofday(&start, NULL);
        getrusage(RUSAGE_CHILDREN, &before);
        
        printf("command: \n\t");
        for(int i = 0; i <= argCounter; i++){
            printf("[%s]", args[i]);
        }
        printf("\n");

        // --- FORK PROCESS ---
        pid_t pid = fork();
        if (pid < 0){
            perror("Fork failed\n");
            return -1;
        } else if (pid == 0) {
            // child process
            execvp(args[0], args);
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
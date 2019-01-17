#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

char* commands[] = {"whoami", "last", "ls", NULL};

void print_menu() {
    printf("\n===== Mid-Day Commander, v0 ===== \n");
    printf("G’day, Commander! What command would you like to run? \n");
    printf("   0. whoami  : Prints out the result of the whoami command \n");
    printf("   1. last    : Prints out the result of the last command \n");
    printf("   2. ls      : Prints out the result of a listing on a user-specified path \n");
}

int main(int argc, char const *argv[]){
    while(1){
        print_menu();

        int cmd;
        char* args[4] = {NULL, NULL, NULL, NULL};
        printf("Option?: ");
        scanf("%c", &cmd);
        printf("\n");

        // --- PROCESS INPUTS ---
        if (cmd == 0){
            printf("-- Who Am I? --\n");
            args[0] = "whoami";
        } else if (cmd == 1){
            printf("-- Last Logins --\n");
            args[0] = "last";
        } else if (cmd == 2){
            char arg[20] = "";
            char dir[20] = "";
            printf("-- Directory Listing --\n");
            printf("Arguments?: ");
            scanf("%s", arg);
            printf("Path?: ");
            scanf("%s", dir);
            args[0] = "ls";
            args[1] = arg;
            args[2] = dir;
        } else {
            printf("Invalid command input, exiting...\n");
            return -1;
        }

        struct timeval start, end;
        struct rusage before, after;
        gettimeofday(&start, NULL);
        getrusage(RUSAGE_SELF, &before);

        // --- FORK PROCESS ---
        pid_t pid = fork();
        if (pid < 0){
            perror("Fork failed");
            return -1;
        } else if (pid == 0) {
            // 0 means child process, execute command
            execvp(commands[cmd], args);
            printf("Failed to execute command\n"); // shouldn't be here
            return -1;
        }
        // else it is parent process
        wait(NULL);
        gettimeofday(&end, NULL);
        getrusage(RUSAGE_SELF, &after);

        // --- PRINT STATS ---
        printf("\n-- Statistics --\n");
        int time = ((int)end.tv_sec - (int)start.tv_sec)*1000 + ((int)end.tv_usec - (int)start.tv_usec)/1000;
        printf("Elapsed time: %d milliseconds\n", time);
        printf("Page Faults: %ld \n", after.ru_majflt - before.ru_majflt);
        printf("Page Faults (reclaimed): %ld \n", after.ru_minflt - before.ru_minflt);
    }
    return 0;
}
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

char* commands[] = {"whoami", "last", "ls", NULL};

void print_menu() {
    printf("\nG’day, Commander! What command would you like to run? \n");
    printf("   0. whoami  : Prints out the result of the whoami command \n");
    printf("   1. last    : Prints out the result of the last command \n");
    printf("   2. ls      : Prints out the result of a listing on a user-specified path \n");
}

int main(int argc, char const *argv[]){
    int c;
    printf("===== Mid-Day Commander, v0 =====");
    while(!feof(stdin)){
        print_menu();

        char* args[4] = {NULL, NULL, NULL, NULL};
        printf("Option?: ");
        char cmd = fgetc(stdin);
        
        // printf("======\n");
        // while((c=getchar()) != '\n' && c!= EOF);

        // --- PROCESS INPUTS ---

        if (cmd != '\n') while((c=getchar()) != '\n' && c!= EOF);
        if (cmd == '0'){
            printf("-- Who Am I? --\n");
            args[0] = "whoami";
        } else if (cmd == '1'){
            printf("-- Last Logins --\n");
            args[0] = "last";
        } else if (cmd == '2'){
            char arg[20];
            // char* argp[5] = {NULL, NULL, NULL, NULL, NULL};
            char dir[20];
            printf("-- Directory Listing --\n");
            
            // prevent skip fgets
            printf("Arguments?: ");
            fgets(arg, 20, stdin);
            arg[strlen(arg)-1] = '\0';

            printf("Path?: ");
            fgets(dir, 20, stdin);
            dir[strlen(dir)-1] = '\0';

            args[0] = "ls";
            if (strlen(arg) != 0 && strlen(dir) != 0){
                args[1] = arg;
                args[2] = dir;
            } else if (strlen(arg) != 0){
                args[1] = arg;
            } else if (strlen(dir) != 0){
                args[1] = dir;
            }
            printf("\n");

        } 
        else {
            printf("Invalid command input, please use the numbers above.\n");
            continue;
        }

        struct timeval start, end;
        struct rusage before, after;
        gettimeofday(&start, NULL);
        getrusage(RUSAGE_CHILDREN, &before);

        // --- FORK PROCESS ---
        pid_t pid = fork();
        if (pid < 0){
            perror("Fork failed");
            return -1;
        } else if (pid == 0) {
            // 0 means child process, execute command
            execvp(commands[cmd-48], args);
            printf("Failed to execute command\n"); // shouldn't be here
            return -1;
        }
        // else it is parent process
        wait(NULL);
        gettimeofday(&end, NULL);
        getrusage(RUSAGE_CHILDREN, &after);

        // --- PRINT STATS ---
        printf("\n-- Statistics --\n");
        double time = ((double)end.tv_sec - (double)start.tv_sec)*1000 + ((double)end.tv_usec - (double)start.tv_usec)/1000;
        printf("Elapsed time: %.3f milliseconds\n", time);
        printf("Page Faults: %ld \n", after.ru_majflt - before.ru_majflt);
        printf("Page Faults (reclaimed): %ld \n", after.ru_minflt - before.ru_minflt);
    }
    return 0;
}
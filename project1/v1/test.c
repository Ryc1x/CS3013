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
#include <ctype.h>

int main(int argc, char const *argv[]){
    char input[40];
    char* args[20];
    char* delim = " ";
    char* p;
    int number = -1;

    while(1){
        fgets(input, 10, stdin); // max+3
        
        // if(strlen(input) > 7){ //max+1
        //     printf("input too long\n");
        //     if(input[7]!='\n')while(getchar() != '\n');
        // }
        // for (int i = 0; i < 10; i++){
        //     printf("%d: %d | ", i, input[i]);
        // }
        
        char c;
        int d;
        sscanf(input, "%d", &d);
        printf("%d\n",d);
        

    }

    return 0;
}

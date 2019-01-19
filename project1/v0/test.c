#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <ctype.h>

int main(int argc, char const *argv[]){
    char s[40];
    char* args[20];
    char* delim = " ";
    char* p;
    int number = -1;

    while(1){
        // fgets(input, 5, stdin); // max+3
        // // if()
        // if(strlen(input) > 3){ //max+1
        //     printf("input too long\n");
        //     if(input[3]!='\n')while(getchar() != '\n');
        // }
        // for (int i = 0; i < 5; i++){
        //     printf("%d: %d | ", i, input[i]);
        // }
        // printf("\n");


        fgets(s, 40, stdin);
        
        char* token = strtok(s, delim);
        printf("%s\n",token);
        int i = 0;
        while((args[i] = strtok(NULL, delim))){
            printf("%d:%s %p\n", i, args[i], args[i]);
            i++;
        }

    }

    return 0;
}

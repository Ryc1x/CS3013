#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>

int main(int argc, char const *argv[]){
    int num = 0;
    scanf("%d", &num);
    printf("%d", num);
    return 0;
}

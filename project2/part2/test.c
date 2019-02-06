#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// These values MUST match the unistd_32.h modifications:
#define __NR_cs3013_syscall2 378

/*asestry info stored here*/
struct ancestry {
  pid_t ancestors[10]; //pid of ancestors
  pid_t siblings[100]; //pid of siblings
  pid_t children[100]; //pid of children
};//end struct

long testCall2 (unsigned short* tar,struct ancestry* anc) {

  if(*tar==(unsigned short) -1) {
    //me
    pid_t pid = fork();
    if(pid < 0) {printf("fork FAILURE!"); return-1;}
    else if (pid == 0) {
      //child
      pid_t pid_1 = fork();
      // printf("Gotten child PID: %d\n", pid_1);
      if(pid_1 < 0) {printf("fork FAILURE!"); return-1;}
      else if (pid_1 == 0) {while(1);}
      else{while(1);}
    }
    else{
      //sibling
      pid_t pid_0 = fork();
      // printf("Gotten sibling PID: %d\n", pid_0);
      if(pid_0 < 0) {printf("fork FAILURE!"); return-1;}
      else if (pid_0 == 0) {while(1);}
      //me
      sleep(1);//wait until children are created successfully
      *tar = (unsigned short)pid;
      printf("Gotten my PID: %d\n", pid);
      return (long) syscall(__NR_cs3013_syscall2, tar, anc);
    }
  }

  else{
    printf("user input PID: %d\n", *tar);
    return (long) syscall(__NR_cs3013_syscall2, tar, anc);
  }

  return -1;
}



int main (int argc, char* argv[]) {
  int index = 0;
  //space allocation in user space
  unsigned short* my_target = malloc(sizeof(unsigned short*));
  struct ancestry* my_response = malloc(sizeof(struct ancestry));
  if(argc < 2)  *my_target =(unsigned short) -1;
  else if(argc == 2)*my_target = atoi(argv[1]);
  else{printf("invalid command input\n"); exit(-1);}
  //print report
  printf("|| cs3013_syscall2 returns: %ld\n", testCall2(my_target, my_response));
  printf("|| my children:{");
  while(my_response->children[index] != 0){
    printf("%d, ", my_response->children[index]);
    index++;
  }
  printf("END}\n");
  index = 0;

  printf("|| my siblings:{");
  while(my_response->siblings[index] != 0){
    printf("%d, ", my_response->siblings[index]);
    index++;
  }
  printf("END}\n");
  index = 0;

  printf("|| my ancestors:{");
  while(my_response->ancestors[index] != 0){
    printf("%d, ", my_response->ancestors[index]);
    index++;
  }
  printf("END}\n");

  printf("END OF STORY\n");
  return 0;
}

#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>

// These values MUST match the unistd_32.h modifications:
#define __NR_cs3013_syscall1 377
#define __NR_cs3013_syscall2 378
#define __NR_cs3013_syscall3 379

#define BUF_SIZE 500

int main () {
	// create buffer, open files
	char buf1[BUF_SIZE+2];
	char buf2[BUF_SIZE+2];
	FILE* virus = fopen("virus", "r");
	FILE* novirus = fopen("novirus", "r");
	// read file content to buffer
	fread(buf1, 1, BUF_SIZE, virus);
	printf("%s",buf1);
	fread(buf2, 1, BUF_SIZE, novirus);
	// close files
	fclose(virus);
	fclose(novirus);

	printf("Check the syslog for testing result.\n");
	return 0;
}


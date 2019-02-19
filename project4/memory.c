/**
 * CS3013 - C19
 * Project 4 - Virtual Memory
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

#define PMEM_SIZE 64
#define VMEM_SIZE 64
#define PAGE_SIZE 16
#define PTE_SIZE 4

#define PFN 0
#define VALID 1
#define PROTECTION 2
#define PRESENT 3

#define WRITE 1
#define READ 0

typedef struct __PTE {
    unsigned char pframe;
    unsigned char valid;
    unsigned char protection;
    unsigned char present;
} PTE;


char* input;
char** args;
unsigned char* memory;
int argCounter = 0;
int hardreg[4] = {-1, -1, -1, -1};
int freepage[4] = {1, 1, 1, 1};

/**
 * Print the statistics after executing a process
 */
void print_stats(double time, long majflt, long minflt) {
    printf("\n-- Statistics --\n");
    printf("Elapsed time: %.3f milliseconds\n", time);
    printf("Page Faults: %ld \n", majflt);
    printf("Page Faults (reclaimed): %ld \n", minflt);
}

void print_memory(){
    printf("=====  Memory statistics  =====\n");
    for (int i = 0; i < PMEM_SIZE; i++){
        printf("%d ", memory[i]);
        if (i%16 == 15) printf("\n");
    }
}

/**
 * Free the memory and exit the program
 */
void exitmc(){
    free(input);
    free(args);
    free(memory);
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
        char* delim = ",";
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

int nextfreepage(){
    for (int i = 0; i < 4; i++){
        if (freepage[i])
            return i;
    }
    return -1;
}

int entry(unsigned char pid, unsigned char vaddr){
    int pagetable = hardreg[pid];
    // Page table not found
    if (pagetable == -1)
        return -1;
    int vpage = vaddr / PAGE_SIZE;
    return pagetable*PAGE_SIZE + vpage*PTE_SIZE;
}

int convert(unsigned char pid, unsigned char vaddr){
    int offset = vaddr % PAGE_SIZE;
    int entryidx = entry(pid, vaddr);
    // invalid mapping
    if (entryidx == -1 || memory[entryidx + VALID] == 0)
        return -1;
    int paddr = memory[entryidx + PFN] * PAGE_SIZE + offset;
    return paddr;
}

int map(unsigned char pid, unsigned char vaddr, unsigned char value){
    // CREATE PAGE TABLE
    int pagetable = hardreg[pid];
    int vpage = vaddr / PAGE_SIZE;
    if (pagetable == -1){
        // if page table not in the memory, try to allocate one page for PT
        pagetable = nextfreepage();
        if (pagetable == -1){
            printf("ERROR: not enough space in memory to create page table.\n");
            return -1;
        }
        freepage[pagetable] = 0;
        hardreg[pid] = pagetable;
        int fstart = pagetable * PAGE_SIZE;
        int fend = fstart + PAGE_SIZE;
        // clear the page table
        for (int i = fstart; i < fend; i++){
            memory[i] = 0;
        }
        printf("Put page table for PID %u into physical frame %d\n", pid, pagetable);
    }
    // MAPPING
    // if already being mapped, return error
    int entryidx = pagetable*PAGE_SIZE + vpage*PTE_SIZE;
    if (memory[entryidx + VALID] == 1){
        printf("ERROR: virtual page already mapped into physical frame %u\n", memory[entryidx + PFN]);
        return -1;
    }
    int frame = nextfreepage();
    if (frame == -1){
        printf("ERROR: not enough space in memory for mapping.\n");
        return -1;
    }
    // else map a virtual page to physical frame
    freepage[frame] = 0;
    memory[entryidx + PFN] = frame;
    memory[entryidx + VALID] = 1;
    memory[entryidx + PROTECTION] = value;
    memory[entryidx + PRESENT] = 1;
    printf("Mapped virtual address %d (page %d) into physical frame %d\n", vaddr, vpage, frame);
    return 0;
}

int store(unsigned char pid, unsigned char vaddr, unsigned char value){
    int entryidx = entry(pid, vaddr);
    int paddr = convert(pid, vaddr);
    if (paddr == -1){
        printf("No mapping for process %u virtual address %u\n", pid, vaddr);
        return -1;
    }
    if (memory[entryidx + PROTECTION] != WRITE){
        printf("The physical frame %u is not writable\n", memory[entryidx + PFN]);
        return -1;
    }

    memory[paddr] = value;
    printf("Stored value %u at virtual address %u (physical address %d)\n", value, vaddr, paddr);
    return 0;
}

int load(unsigned char pid, unsigned char vaddr, unsigned char value){
    int paddr = convert(pid, vaddr);
    if (paddr == -1){
        printf("No mapping for process %u virtual address %u\n", pid, vaddr);
        return -1;
    }
    
    printf("The value %u is virtual address %u (physical address %d)\n", memory[paddr], vaddr, paddr);
    return 0;
}

/**
 * Run Memory Manager program.
 */
int main(int argc, char const *argv[]){
    printf("===== Memory Manager v1 =====\n");

    // allocate memories
    input = (char*) malloc(sizeof(char*) * (MAX_INPUT + 3));
    args = (char**) malloc(sizeof(char**) * (MAX_ARGS + 2));
    memory = (unsigned char*) malloc(sizeof(unsigned char*) * (PMEM_SIZE));

    while(1){
        // --- INITIALIZE ---
        argCounter = 0;
        printf("\nInstruction? ");

        // --- INPUT CHECK ---
        if(readInput(input)) continue;
        if(input[0] == 'p'){
            print_memory();
            continue;
        }
        if(readInputArgs(args, input)) continue;
        if(argCounter != 4){
            printf("Wrong number of arguments.\n");
            continue;
        }

        unsigned char pid = atoi(args[0]);
        char* instruction = args[1];
        unsigned char vaddr = atoi(args[2]);
        unsigned char value = atoi(args[3]);

        // --- SELECT COMMAND ---
        if (strcmp(instruction, "map") == 0){
            // map instruction 
            map(pid, vaddr, value);
        } else if (strcmp(instruction, "store") == 0){
            // store instruction
            store(pid, vaddr, value);
        } else if (strcmp(instruction, "load") == 0){
            // load instruction
            load(pid, vaddr, value);
        } else {
            // invalid instruction
            printf("Invalid instruction, instruction can only be map/store/load.\n");
            continue;
        }
    }
    return 0;
}
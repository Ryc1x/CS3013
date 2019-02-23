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

#define DISK_SIZE 1024
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

#define EMPTY -1
#define OCCUPY_VAL 0
#define OCCUPY_PT 1

/* Not used
typedef struct __PTE {
    unsigned char pframe;
    unsigned char valid;
    unsigned char protection;
    unsigned char present;
} PTE;
*/

// global variables
char* input;
char** args;
unsigned char* memory;
unsigned char* disk;
int argCounter = 0;
int hardreg[4] = {-1, -1, -1, -1};
int hardregdisk[4] = {-1, -1, -1, -1};
int mempid[4] = {-1, -1, -1, -1};
int mempages[4] = {EMPTY, EMPTY, EMPTY, EMPTY};
int diskpages[64];
int roundrobin = 0;

// function prototypes
int nextfreepage(int pid);
int evict(int pid);
int findpte(int pid, int vaddr);


/**
 * Print the memory array
 */
void print_memory(){
    printf("=====  Memory statistics  =====\n");
    for (int i = 0; i < PMEM_SIZE; i++){
        printf("%d ", memory[i]);
        if (i%16 == 15) printf("\n");
    }
    printf("mempages: %d %d %d %d\n", mempages[0], mempages[1], mempages[2], mempages[3]);
    printf("mempid: %d %d %d %d\n", mempid[0], mempid[1], mempid[2], mempid[3]);
    printf("hardreg: %d %d %d %d\n", hardreg[0], hardreg[1], hardreg[2], hardreg[3]);
    printf("hardregdisk: %d %d %d %d\n", hardregdisk[0], hardregdisk[1], hardregdisk[2], hardregdisk[3]);
    printf("diskpages: %d %d %d %d\n", diskpages[0], diskpages[1], diskpages[2], diskpages[3]);
    printf("round-robin: %d\n", roundrobin);
}

void print_disk(){
    printf("=====================  Disk statistics  =====================\n");
    for (int i = 0; i < DISK_SIZE; i++){
        printf("%d ", disk[i]);
        if (i%32 == 31) printf("\n");
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

int nextfreepage(int pid) {
    for (int i = 0; i < 4; i++){
        if (mempages[i] == EMPTY)
            return i;
    }
    // if all pages are full, evict one page
    return evict(pid);
}

int nextfreepagedisk() {
    for (int i = 0; i < 64; i++){
        if (diskpages[i] == EMPTY)
            return i;
    }
    return -1;
}

int swap(int mempage, int diskpage){
    int memstart = mempage * PAGE_SIZE;
    int diskstart = diskpage * PAGE_SIZE;
    unsigned char temp;
    for (int i = 0; i < PAGE_SIZE; i++){
        temp = memory[memstart + i];
        memory[memstart + i] = disk[diskstart + i];
        disk[diskstart + i] = temp;
    }
    return 0;
}

int convert(unsigned char pid, unsigned char vaddr){
    int offset = vaddr % PAGE_SIZE;
    int entryidx = findpte(pid, vaddr);
    // invalid mapping
    if (entryidx == -1 || memory[entryidx + VALID] == 0 || memory[entryidx + PRESENT] == 0)
        return -1;
    int paddr = memory[entryidx + PFN] * PAGE_SIZE + offset;
    return paddr;
}

/**
 * return page table index
 */
int findpt(int pid){
    // if PT not in memory
    if (hardreg[pid] == -1){
        // if PT doesn't exist, return -1
        if (hardregdisk[pid] == -1){
            return -1;
        }
        // else on disk, swap it to the memory and return idx
        int mempage = nextfreepage(pid);
        int diskpage = hardregdisk[pid];
        swap(mempage, diskpage);
        hardreg[pid] = mempage;
        mempages[mempage] = OCCUPY_PT;
        mempid[mempage] = pid;
        diskpages[diskpage] = EMPTY;
        printf("Swapped disk page %d to memory page %d\n", diskpage, mempage);
    }
    // return the PT index
    return hardreg[pid];
}

int updatepte(int pid, int frame, int diskpage){
    if (hardreg[pid] > -1){
        int entryidx = hardreg[pid] * PAGE_SIZE;
        for (int i = 0; i < 4; i++){
            if (memory[entryidx + PFN] == frame){
                printf("updating evicted entryidx: %d\n", entryidx);
                memory[entryidx + PRESENT] = 0;
                memory[entryidx + PFN] = diskpage;
                break;
            }
            entryidx += PTE_SIZE;
        }
    } else {
        int entryidx = hardregdisk[pid] * PAGE_SIZE;
        for (int i = 0; i < 4; i++){
            if (disk[entryidx + PFN] == frame){
                printf("updating evicted entryidx: %d\n", entryidx);
                disk[entryidx + PRESENT] = 0;
                disk[entryidx + PFN] = diskpage;
                break;
            }
            entryidx += PTE_SIZE;
        }
    }
    return 0;
}

/**
 * return overall index in entire memory
 */
int findpte(int pid, int vaddr){
    int pagetable = findpt(pid);
    if (pagetable == -1){
        return -1;
    }
    int vpage = vaddr / PAGE_SIZE;
    return pagetable*PAGE_SIZE + vpage*PTE_SIZE;
}

/**
 * evict a page from the memory using round-robin return eviced page
 */
int evict(int pid) {
    roundrobin = roundrobin % 4;
    int evicttype = mempages[roundrobin];
    int evictpid = mempid[roundrobin];
    int diskpage = nextfreepagedisk();
    if (evicttype == OCCUPY_VAL){
        // find the pte to be updated
        updatepte(evictpid, roundrobin, diskpage);
        // evict the page
        swap(roundrobin, diskpage);
        diskpages[diskpage] = OCCUPY_VAL;
    } else if (evicttype == OCCUPY_PT) {
        // a page table to be evicted
        if (evictpid == pid){
            roundrobin++;
            evict(pid);
            return roundrobin - 1;
        }
        hardreg[evictpid] = -1;
        hardregdisk[evictpid] = diskpage;
        swap(roundrobin, diskpage);
        diskpages[diskpage] = OCCUPY_PT;
    } else {
        printf("Trying to evict an empty page\n");
        return -1;
    }
    printf("Evict memory page %d out to disk page %d\n", roundrobin, diskpage);
    return roundrobin++;
}

int map(unsigned char pid, unsigned char vaddr, unsigned char value){
    // FIND PAGE TABLE
    int pagetable = findpt(pid);
    int vpage = vaddr / PAGE_SIZE;
    // the page table doesn't exist, try to allocate one page for PT
    if (pagetable == -1){
        // get a free page (evict one if full)
        pagetable = nextfreepage(pid);
        // set the hardware register and page status
        mempages[pagetable] = OCCUPY_PT;
        mempid[pagetable] = pid;
        hardreg[pid] = pagetable;
        // clear the page table
        int fstart = pagetable * PAGE_SIZE;
        int fend = fstart + PAGE_SIZE;
        for (int i = fstart; i < fend; i++){
            memory[i] = 0;
        }
        printf("Put page table for PID %u into physical frame %d\n", pid, pagetable);
    }
    int entryidx = pagetable*PAGE_SIZE + vpage*PTE_SIZE;
    // MAPPING
    // if already being mapped, update permission or return error
    if (memory[entryidx + VALID] == 1){
        if (value == memory[entryidx + PROTECTION]){
            printf("ERROR: virtual page %d already mapped with r/w_bit = %u\n", vpage, value);
            return -1;
        } else {
            memory[entryidx + PROTECTION] = value;
            printf("Updating permissions for virtual page %d", vpage);
            return 0;
        }
    }
    int frame = nextfreepage(pid);
    // else map a virtual page to physical frame
    mempages[frame] = OCCUPY_VAL;
    mempid[frame] = pid;
    memory[entryidx + PFN] = frame;
    memory[entryidx + VALID] = 1;
    memory[entryidx + PROTECTION] = value;
    memory[entryidx + PRESENT] = 1;
    printf("Mapped virtual address %d (page %d) into physical frame %d\n", vaddr, vpage, frame);
    return 0;
}

int store(unsigned char pid, unsigned char vaddr, unsigned char value){
    int entryidx = findpte(pid, vaddr);
    // if page on disk, swap it to the memory
    if (memory[entryidx + PRESENT] == 0){
        int frame = nextfreepage(pid);
        int diskframe = memory[entryidx + PFN];
        swap(frame,diskframe);
        mempages[frame] = OCCUPY_VAL;
        mempid[frame] = pid;
        diskpages[diskframe] = EMPTY;
        memory[entryidx + PRESENT] = 1;
        memory[entryidx + PFN] = frame;
    }
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
    int entryidx = findpte(pid, vaddr);
    // if page on disk, swap it to the memory
    if (memory[entryidx + PRESENT] == 0){
        int frame = nextfreepage(pid);
        int diskframe = memory[entryidx + PFN];
        swap(frame,diskframe);
        mempages[frame] = OCCUPY_VAL;
        mempid[frame] = pid;
        diskpages[diskframe] = EMPTY;
        memory[entryidx + PRESENT] = 1;
        memory[entryidx + PFN] = frame;
    }
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
    disk = (unsigned char*) malloc(sizeof(unsigned char*) * (DISK_SIZE));
    for (int i = 0; i < 64; i++){
        diskpages[i] = EMPTY;
    }
    while(1){
        // --- INITIALIZE ---
        argCounter = 0;
        printf("\nInstruction? ");

        // --- INPUT CHECK ---
        if(readInput(input)) continue;
        if(input[0] == 'm'){
            print_memory();
            continue;
        } else if(input[0] == 'd'){
            print_disk();
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
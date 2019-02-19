#####################################################################################
                                CS3013 Project4
                                Memory Manager
                       Author: Rui Huang, Zonglin Peng
#####################################################################################

# INSTRUCTIONS
To run the program, do:
    $ make
then:
    $ ./memory
To use our test cases:
    $ ./memory < input

# DESCRIPTION
In this project, we simulated a memory that has 64 bytes. When we mapping the page 
two physical frame will be allocated. One for page table and the other one for mapped 
memory. So only two process can be mapped for now.

To debug the program, we added a input command "p", e.g.:

Instruction? p
=====  Memory statistics  =====
0 0 0 0 1 1 1 1 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 0 0 3 1 0 1 
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 

It will print out the memory.


# TESTING
The result of our sample test cases is copied below.


# OUTPUT 
    $ ./memory < input 
===== Memory Manager v1 =====
Instruction? Put page table for PID 1 into physical frame 0
Mapped virtual address 16 (page 1) into physical frame 1

Instruction? ERROR: virtual page already mapped into physical frame 1

Instruction? Put page table for PID 2 into physical frame 2
Mapped virtual address 48 (page 3) into physical frame 3

Instruction? ERROR: not enough space in memory for mapping.

Instruction? =====  Memory statistics  =====
0 0 0 0 1 1 1 1 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
0 0 0 0 0 0 0 0 0 0 0 0 3 1 0 1 
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 

Instruction? No mapping for process 1 virtual address 15

Instruction? Stored value 99 at virtual address 19 (physical address 19)

Instruction? Stored value 255 at virtual address 28 (physical address 28)

Instruction? No mapping for process 1 virtual address 47

Instruction? The physical frame 3 is not writable

Instruction? No mapping for process 2 virtual address 0

Instruction? No mapping for process 3 virtual address 0

Instruction? =====  Memory statistics  =====
0 0 0 0 1 1 1 1 0 0 0 0 0 0 0 0 
0 0 0 99 0 0 0 0 0 0 0 0 255 0 0 0 
0 0 0 0 0 0 0 0 0 0 0 0 3 1 0 1 
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 

Instruction? No mapping for process 1 virtual address 15

Instruction? The value 99 is virtual address 19 (physical address 19)

Instruction? The value 255 is virtual address 28 (physical address 28)

Instruction? No mapping for process 1 virtual address 47

Instruction? The value 0 is virtual address 48 (physical address 48)

Instruction? No mapping for process 2 virtual address 0

Instruction? No mapping for process 3 virtual address 0

Instruction? 
 -- No Input Detected. -- 
Logging you out, Commander.
***********************************************************
                      CS3013 - C19
              Project 1 - Midday Commander
***********************************************************
# Team members: 
              Rui Huang, Zonglin Peng

# Summary
We complete this assignment seperately and both of us finished 
the project 1. Thefore we dicide to use Rui's phase 1&2 and 
Zonglin's phase 3.

# Running & Testing
Compiling code by make in each folder:
	$ make
Then run corresponding mc version (e.g.: mc2):
	$ ./mc2
To redirect input for testing:
	$ ./mc2 < input

******************* PHASE 1 AND PHASE 2 *********************

# Assumptions
For phase 1&2:
 - Max input length is 128
 - Max invidual args is 32
 - Max command definiton is 128
 - Program exits after read EOF


# Code structure and algorihtms

Phase 1 and 2:
v0 and v1 has similar code structure, they both has a main()
function which runs a while loop keep asking for input and 
call other helper functions.

Once variables are initialized, the program will prompt user 
to input a command, then the program will call readInput() 
to process input, and do a series of conditional statement to
get the actual argument for command (or simply run a function
based on the input). Then the program will record inforamtion, 
fork a process, then process the time and rusage inforamtion.

For user-defined commands, we use strtok() to tokenize the 
copied string then put then in args[].

No special algorihtms or data structure used in phase 1 and 2.


# Test cases

For phase 1 and phase 2, we have one test case for each of 
them, named "input". To use them, just redirect the file into 
the program.

The test case for phase 1 includes test for 0, 1, and directory
listing with several different arguments.

The test case for phase 2 includes user-added commands like 
rm/touch/ls etc. In addition to that, we also test a lot of 
invalid input or really long input.  


************************* PHASE 3 **************************

Instructions:
Use Makefile to compile and clean the program. According
to the prompt input valid commands.

Test Case:
	Test cases are designed in file.txt, where it test 
all the given options in the shell. Each background process
are ran along with other foreground and background 
processes, and each time a process list is printed to 
varify that each child is created and terminated properly.
From running the test cases (32 in total), tester can 
observe that background outputs may intermingle with the
prompt or the foreground outputs, under all circumstances
that the shell promotes a prompt and wait for user input.
All the background are return in their finished order, yet
if the shell is hung by an user input and by the end a 
number of backgrounds are finished, the output will be
printed in random order. Upone exit, the shell will check
if all backgrounds are done; if not it will refuse to 
finish, prints the process list, and promotes another 
prompt.
	The first part of the test file creates all the
necessary foreground and background command. The second 
part run the command with fore to fore, back to back, back
to fore, and fore to back. The third part test the exit
when background is not done. At last, the EOF will exit
the shell successfully.


Assumptions & Results:
1. when user try to input nothing and hits enter key,
	the shell will assume user is done and will exit
	the shell in the normal way.
2. the user is only allow to input exactly one charactor 
	as a command; otherwise will be treated as invalid.
3. the max input is set as 7 for options, whereas for
	special arguements such as path allow to input
	up to 128 chars.
4. the given ID of each background process is an interger
	from 0, and each time a new background is created
	the ID will be increamented by 1, and decreamented
	by 1 when a background is terminated.


Thanks!

                                            Rui & Zonglin

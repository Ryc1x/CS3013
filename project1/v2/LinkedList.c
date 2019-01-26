/**
 * CS3013 - C19
 * Project 1 - Midday Commander
 * Author: Rui Huang
 * 
 * Implemented a linked list for background processes
 */

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>

#include "LinkedList.h"

node* head = NULL;
node* tail = NULL;

void init(){
    listsize = 0;
    head = (node*) malloc(sizeof(struct Node));
    head->pid = -1;
    head->cmd = NULL;
    head->next = NULL;
    tail = head;
}

node* listGet(pid_t pid){
    node* p = head;
    int i = 0;
    while (p != NULL){
        if(p->pid == pid){
            printf("\n-- Job Complete [%d] --\n", i);
            printf("Process ID: %d\n", pid);
            return p;
        }
        p = p->next;
        i++;
    }
    return NULL;
}

int listAppend(pid_t pid, char* cmd, double startTime){
    node* n = (node*) malloc(sizeof(struct Node));
    n->pid = pid;
    n->cmd = cmd;
    n->startTime = startTime;
    n->next = NULL;
    tail->next = n;
    tail = n;
    listsize++;
    return 0;
}

int listRemove(pid_t pid){
    node* p = head;
    node* q = p;
    while (p != NULL){
        if(p->pid == pid){
            listsize--;
            if (p == tail) tail = q;
            q->next = p->next;
            free(p);
            return 0;
        }
        q = p;
        p = p->next;
    }
    return -1;
}

void listPrint(){
    node* p = head->next;
    printf("\n-- Background Processes --\n");
    printf("Processes number: %d\n",listsize);
    printf("Head: %d, Tail: %d\n", head->pid, tail->pid);
    for (int i = 1; p != NULL; i++){
        printf("[%d] pid:%d command:%s\n", i, p->pid, p->cmd);
        p = p->next;
    }
}
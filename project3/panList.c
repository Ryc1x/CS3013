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

#include "panList.h"

Node* head = NULL;
Node* tail = NULL;

void init(){
    listsize = 0;
    countP = 0;
    countN = 0;
    head = (Node*) malloc(sizeof(Node));
    head->tid = -1;
    head->type = -1;
    head->next = NULL;
    tail = head;
}

pthread_t listAnyFirst(){
    if(head->next != NULL){
        return head->next->tid;
    }
    return -1;
}

pthread_t listFirst(int type){
    Node* p = head;
    int i = 0;
    while (p != NULL){
        if(p->type == type){
            return p->tid;
        }
        p = p->next;
        i++;
    }
    return -1;
}

int listAppend(pthread_t tid, int type){
    Node* n = (Node*) malloc(sizeof(Node));
    n->tid = tid;
    n->type = type;
    n->next = NULL;
    tail->next = n;
    tail = n;
    listsize++;
    if (type == 1){
        countP++;
    } else {
        countN++;
    }
    first = listAnyFirst();
    firstP = listFirst(1);
    firstN = listFirst(2);
    return 0;
}

int listRemove(pthread_t tid){
    Node* p = head;
    Node* q = p;
    while (p != NULL){
        if(p->tid == tid){
            listsize--;
            if (p->type == 1){
                countP--;
            } else {
                countN--;
            }
            if (p == tail) tail = q;
            q->next = p->next;
            free(p);
            first = listAnyFirst();
            firstP = listFirst(1);
            firstN = listFirst(2);
            return 0;
        }
        q = p;
        p = p->next;
    }
    return -1;
}

void listPrint(){
    Node* p = head->next;
    printf("\n-- Queue status --\n");
    printf("Total nodes: %d\n",listsize);
    printf("Head: %lu, Tail: %lu\n", head->tid, tail->tid);
    for (int i = 1; p != NULL; i++){
        printf("[%d] tid:%lu type:%d\n", i, p->tid, p->type);
        p = p->next;
    }
}
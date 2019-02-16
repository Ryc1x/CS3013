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

#include "carList.h"

Node **heads, **tails;

void init(){
    heads = (Node**) malloc(4 * sizeof(Node*));
    tails = (Node**) malloc(4 * sizeof(Node*));
    for(int i = 0; i < 4; i++){
        listsize[i] = 0;
        first[i] = -1;
        heads[i] = (Node*) malloc(sizeof(Node));
        heads[i]->tid = -1;
        heads[i]->turn = -1;
        heads[i]->next = NULL;
        tails[i] = heads[i];
    }
}

void updateFirst(int dir){
    Node* f = (Node*) malloc(sizeof(Node));
    f = heads[dir]->next;
    if(f != NULL){
        first[dir] = f->tid;
    } else {
        first[dir] = -1;
    }
}

int push(pthread_t tid, int dir, int turn){
    Node* n = (Node*) malloc(sizeof(Node));
    n->tid = tid;
    n->turn = turn;
    n->next = NULL;
    tails[dir]->next = n;
    tails[dir] = n;
    listsize[dir]++;
    updateFirst(dir);
    return 0;
}

int pop(int dir){
    Node* p = heads[dir]->next;
    if (p != NULL){
        if (p == tails[dir]){
            tails[dir] = heads[dir];
        }
        heads[dir]->next = p->next;
        listsize[dir]--;
        updateFirst(dir);
        free(p);
        return 0;
    }
    return -1;
}

void listPrint(){
    printf("\n=====    DRIVER QUEUE    =====\n");
    for (int d = 0; d < 4; d++){
        Node* p = heads[d]->next;
        printf("\n-- Direction %d status --\n", d);
        printf("Total cars: %d\n", listsize[d]);
        for (int i = 1; p != NULL; i++){
            printf("  [%d] tid:%lu turn:%d\n", i, p->tid, p->turn);
            p = p->next;
        }    
    }
}
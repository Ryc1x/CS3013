#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <pthread.h>

typedef struct __Node{
    pthread_t tid;
    int type;
    struct __Node* next;
} Node;

Node *head, *tail;
int listsize, firstN, firstP, first, countN, countP;

void init();
pthread_t listFirst(int type);
int listAppend(pthread_t tid, int type);
int listRemove(pthread_t tid);
void listPrint();

#endif 
#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <pthread.h>

#define EAST 0
#define NORTH 1
#define WEST 2
#define SOUTH 3

#define RIGHT 0
#define STRAIGHT 1
#define LEFT 2

typedef struct __Node{
    pthread_t tid;
    int turn;
    struct __Node* next;
} Node;

int listsize[4];
pthread_t first[4];

void init();
int push(pthread_t tid, int dir, int turn);
int pop(int dir);
void listPrint();

#endif 
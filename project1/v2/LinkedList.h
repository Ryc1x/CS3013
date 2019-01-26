#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

typedef struct Node{
    pid_t pid;
    char* cmd;
    double startTime;
    struct Node* next;
} node;

node *head, *tail;
int listsize;

void init();
node* listGet(pid_t pid);
int listAppend(pid_t pid, char* cmd, double startTime);
int listRemove(pid_t pid);
void listPrint();

#endif 
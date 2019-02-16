/**
 * CS3013 - Project 3
 * Author: Rui Huang
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#include "carList.h"

#define PI 3.1415926
#define TOTAL_CARS 20

#define WAITING 0
#define ENTERING 1
#define MOVING 2
#define LEAVING 3


// ===== INITIALIZATIONS =====
int infloop = 0;
int carlimit = 0;

pthread_mutex_t* quadlock;
pthread_mutex_t queue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t randlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t movelock = PTHREAD_MUTEX_INITIALIZER;

typedef struct __Car {
    pthread_t tid;
    int dir;
    int turn;
    int index;
} Car;

Car** carlist;


// ===== HELPFER FUNCTIONS =====

int randDirection() {
    double r = drand48();
    if (r < 0.25){
        return EAST;
    } else if (r < 0.5){
        return SOUTH;
    } else if (r < 0.75){
        return WEST;
    } else {
        return NORTH;
    }
}

int randTurn() {
    double r = drand48();
    if (r < 1.0/3){
        return RIGHT;
    } else if (r < 2.0/3){
        return STRAIGHT;
    } else {
        return LEFT;
    }
}

// ===== PRINTS =====

void print_usage() {
    printf("Usage: \n\t ./drivers [(optional) 1 for infinite loop]\n");
}

// ===== THREADS FUNCTION =====

int pass(int currquad, int nextquad, int dest){
    // check if next quadrant is available
    if (currquad == dest){;
        pthread_mutex_unlock(&quadlock[currquad]);
        return LEAVING;
    }
    if (pthread_mutex_trylock(&quadlock[nextquad]) == 0){
        // the check if exceeds car limit
        if (nextquad == dest)
            carlimit--;
        if (currquad == -1)
            carlimit++;
        // if exceeds limit, restore limit and unlock next quadrant
        if (carlimit >= 4){
            carlimit--;
            pthread_mutex_unlock(&quadlock[nextquad]);
            return WAITING;
        }
        // if it is a valid move, unlock the current quadrant
        if (currquad != -1){
            pthread_mutex_unlock(&quadlock[currquad]);
        }
        return currquad == -1 ? ENTERING : MOVING;
    }
    return WAITING;
}

void* carFunction(void* selfptr){
    Car* self = (Car*) selfptr;
    // determine the direction and turn
    pthread_mutex_lock(&randlock);
    self->dir = randDirection();
    self->turn = randTurn();
    pthread_mutex_unlock(&randlock);
    
    // printf("ARRIVE [%d]: dir[%d], turn[%d]\n", self->index, self->dir, self->turn);

    // join the queue
    pthread_mutex_lock(&queue);  
    push(self->tid, self->dir, self->turn);
    pthread_mutex_unlock(&queue);

    // wait until the first in lane
    while (first[self->dir] != self->tid);
    // printf("READY  [%d]: dir[%d], turn[%d]\n", self->index, self->dir, self->turn);
 
    // wait until there are available spot in the intersection
    int curr = -1;
    int next = self->dir;
    int dest = (next + self->turn) % 4;

    // wait intersection condition allowed to pass
    while (1){
        pthread_mutex_lock(&movelock);
        int result = pass(curr, next, dest);
        pthread_mutex_unlock(&movelock);
        if (result == MOVING || result == ENTERING){
            if (result == ENTERING){
                printf("ENTER  [%d]: dir[%d], turn[%d]\n", self->index, self->dir, self->turn);
                // leave the queue
                pthread_mutex_lock(&queue);  
                pop(self->dir);
                pthread_mutex_unlock(&queue);
            } else {
                printf("MOVING [%d]: from[%d], to[%d]\n", self->index, curr, next);
            }
            curr = next;
            next = (next+1) % 4;
            sleep((int)(drand48()*3));
        } else if (result == LEAVING){
            printf("LEAVE  [%d]: destination[%d]\n", self->index, next);
            break;
        }
    }

    // if allow infinite loop, call self.
    if (infloop)
        carFunction(self);

    return NULL;
}


// ===== MAIN =====
int main(int argc, char** argv) {
    // ** Argument processing **
    if (argc > 2){
        printf("Too many arguments.\n");
        print_usage();
        return EXIT_FAILURE;
    }

    if (argc == 2){
        infloop = atoi(argv[1]);
    }

    // ** Initialization **
    srand48(time(NULL));
    quadlock = (pthread_mutex_t*) malloc(4 * sizeof(pthread_mutex_t));
    for (int i = 0; i < 4; i++){
        pthread_mutex_init(&quadlock[i], NULL);
    }

    // alloc memories
    carlist = (Car**) malloc(TOTAL_CARS * sizeof(Car*));
    init();
    
    for (int i = 0; i < TOTAL_CARS; i++){
        Car* p = (Car*) malloc(sizeof(Car));
        p->index = i;
        carlist[i] = p;
    }

    // ** Simulation **
    // create threads
    for (int i = 0; i < TOTAL_CARS; i++){
        pthread_create(&(carlist[i]->tid), NULL, carFunction, carlist[i]);
    }

    // join threads 
    for (int i = 0; i < TOTAL_CARS; i++){
        pthread_join(carlist[i]->tid, NULL);
    }

    return 0;
}

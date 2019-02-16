/**
 * CS3013 - Project 3
 * Author: Rui Huang
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>

#include "panList.h"

#define PI 3.1415926
#define MAX_VISITS 20

#define EMPTY 0
#define PIRATE 1
#define NINJA 2


// ===== INITIALIZATIONS =====
int teams, pirates, ninjas, avgCustomP, avgCustomN, avgArrivalP, avgArrivalN;
double operatingtime, customtime;
double devarrival = 1.0;
double devcustom = 1.0;
int gold = 0;

sem_t semP, semN;
sem_t mutex, queue, check, building, semtime;
int turn = EMPTY;
int occupy = EMPTY;
int totalNum = 0;
int numN = 0;
int numP = 0;

typedef struct __Actor {
    pthread_t tid;
    int type;
    int index;
    int visits;
    int* arrival;
    int* waiting;
    int goldOwed;
} Actor;

Actor **plist, **nlist;


// ===== HELPFER FUNCTIONS =====
double randNormal(double mean, double stddev){
    double z, r;
    do{
        z = sqrt(-2 * log(drand48())) * cos(2 * PI * drand48());
        r = z * stddev * mean + mean;
    } while (r < 0); // remove results < 0
    return r;
}

// ===== PRINTS =====

void print_usage() {
    printf("Usage: \n\t ./pan [# teams] [# pirates] [# ninjas] [avg. P custuming] [avg. N custuming] [avg. P arrival] [avg. N arrival] [(optional) custom stddev] [(optional) arrival stddev]\n");
}

void print_stats() {
    double totalVisits;
    printf("\n==========     FINAL STATISTICS    ==========\n");
    printf("\n----------          PIRATES        ----------\n");
    for (int i = 0; i < pirates; i++){
        Actor* p = plist[i];
        int visits = p->visits;
        totalVisits += visits;
        printf("\nPirates [%d]:\n", i);
        printf("thread ID: %lu\n", p->tid);
        printf("visits: %d\n", visits);
        for (int j = 0; j < visits; j++){
            printf("\tvisit [%d]: arrival: %d, waiting: %d\n", j, p->arrival[j], p->waiting[j]);
        }
        printf("Gold owed: %d\n", p->goldOwed);
    }
    printf("\n----------           NINJAS        ----------\n");
    for (int i = 0; i < ninjas; i++){
        Actor* p = nlist[i];
        int visits = p->visits;
        totalVisits += (visits+1);
        printf("\nNinja [%d]:\n", i);
        printf("thread ID: %lu\n", p->tid);
        printf("visits: %d\n", visits);
        for (int j = 0; j < visits; j++){
            printf("\tvisit [%d]: arrival: %d, waiting: %d\n", j, p->arrival[j], p->waiting[j]);
        }
        printf("Gold owed: %d\n", p->goldOwed);
    }
    printf("\n----------            Shop         ----------\n");
    double avgBusy = customtime/teams;
    double avgFree = operatingtime - avgBusy;
    double profits = gold-5*teams;
    printf("Operating time: %.3f\n", operatingtime);
    printf("Average busy time: %.3f\n", avgBusy);
    printf("Average free time: %.3f\n", avgFree);
    printf("Total revenue: %d\n", gold);
    printf("Total profits: %.3f\n", profits);
    printf("Profit per visits: %.3f\n", profits/totalVisits);
}


// ===== THREADS FUNCTION =====

void* pirateFunction(void* selfptr){
    // sleep to simulate arrival time
    Actor* self = (Actor*) selfptr;
    int arrival = (int)randNormal(avgArrivalP, devarrival);
    self->arrival[self->visits] = arrival;
    printf("Pirate [%d] is on the way (%d sec)!\n", self->index, arrival);
    sleep(arrival);
    printf("Pirate [%d] arrives!\n", self->index);
    int startwait = time(NULL);
    
    // join the queue
    sem_wait(&queue);
    listAppend(self->tid, PIRATE);
    sem_post(&queue);

    // check if next one to go is self (spinning)
    while(1){
        sem_wait(&check);
        if (numN == 0){
            if ((turn != NINJA && firstP == self->tid) || first == self->tid){
                //do something
                sem_wait(&mutex);
                numP++;
                totalNum++;
                if (totalNum >= teams){
                    turn = NINJA;
                }
                sem_post(&mutex);
                sem_post(&check);
                break;
            }
        }
        sem_post(&check);
    }

    // printf("Pirate [%d] enters!\n", self->index);

    // enter the building
    sem_wait(&building);

    // if self is the next one, go customing
    int waiting = (int) time(NULL) - (int) startwait;
    self->waiting[self->visits] = waiting;
    if (waiting <= 30){
        self->goldOwed++;
        gold++;
    }

    // leave the queue
    sem_wait(&queue);
    listRemove(self->tid);
    sem_post(&queue);

    // ~ customing ~
    int custom = (int)randNormal(avgCustomP, devcustom);
    printf("Pirate [%d] is customing (%d sec)!\n", self->index, custom);
    sem_wait(&semtime);
    customtime += custom;
    sem_post(&semtime);
    sleep(custom);

    // leave the building
    sem_wait(&mutex);
    numP--;
    if(numP == 0 && countN > 0){
        turn = NINJA;
        totalNum = 0;
    }
    sem_post(&mutex);

    sem_post(&building);

    printf("Pirate [%d] leaves!\n", self->index);
    
    self->visits++;

    if (drand48()<0.25){
        printf("Pirate [%d] will come again!\n", self->index);
        pirateFunction(self);
    }

    return NULL;
}

void* ninjaFunction(void* selfptr){
    // sleep to simulate arrival time
    Actor* self = (Actor*) selfptr;
    int arrival = (int)randNormal(avgArrivalN, devarrival);
    self->arrival[self->visits] = arrival;
    printf("Ninja [%d] is on the way (%d sec)!\n", self->index, arrival);
    sleep(arrival);
    printf("Ninja [%d] arrives!\n", self->index);
    int startwait = time(NULL);
    
    // join the queue
    sem_wait(&queue);
    listAppend(self->tid, NINJA);
    sem_post(&queue);

    // check if next one to go is self (spinning)
    while(1){
        sem_wait(&check);
        if (numP == 0){
            if ((turn != PIRATE && firstN == self->tid) || first == self->tid){
                //do something
                sem_wait(&mutex);
                numN++;
                totalNum++;
                if (totalNum >= teams){
                    turn = PIRATE;
                }
                sem_post(&mutex);
                sem_post(&check);
                break;
            }
        }
        sem_post(&check);
    }

    // printf("Ninja [%d] enters!\n", self->index);

    // enter the building
    sem_wait(&building);

    // if self is the next one, go customing
    int waiting = (int) time(NULL) - (int) startwait;
    self->waiting[self->visits] = waiting;
    if (waiting <= 30){
        self->goldOwed++;
        gold++;
    }

    // leave the queue
    sem_wait(&queue);
    listRemove(self->tid);
    sem_post(&queue);

    // ~ customing ~
    int custom = (int)randNormal(avgCustomN, devcustom);
    printf("Ninja [%d] is customing (%d sec)!\n", self->index, custom);
    sem_wait(&semtime);
    customtime += custom;
    sem_post(&semtime);
    sleep(custom);

    // leave the building
    sem_wait(&mutex);
    numN--;
    if(numN == 0 && countP > 0){
        turn = PIRATE;
        totalNum = 0;
    }
    sem_post(&mutex);

    sem_post(&building);

    printf("Ninja [%d] leaves!\n", self->index);
    
    self->visits++;

    if (drand48()<0.25){
        printf("Ninja [%d] will come again!\n", self->index);
        ninjaFunction(self);
    }

    return NULL;
}


// ===== MAIN =====
int main(int argc, char** argv) {
    // ** Argument processing **
    if (argc < 8 || argc > 10){
        printf("Too many or too few arguments.\n");
        print_usage();
        return EXIT_FAILURE;
    }
    
    teams = atoi(argv[1]);
    pirates = atoi(argv[2]);
    ninjas = atoi(argv[3]);
    avgCustomP = atoi(argv[4]);
    avgCustomN = atoi(argv[5]);
    avgArrivalP = atoi(argv[6]);
    avgArrivalN = atoi(argv[7]);
    if (argc >= 9){
        devcustom = atof(argv[8]);
    }
    if (argc == 10){
        devarrival = atof(argv[9]);
    }


    // ** Initialization **
    srand48(time(NULL));
    sem_init(&mutex, 0, 1);
    sem_init(&queue, 0, 1);
    sem_init(&check, 0, 1);
    sem_init(&building, 0, teams);
    sem_init(&semtime, 0, 1);
    time_t opentime = time(NULL);

    // alloc memories
    plist = (Actor**) malloc(pirates * sizeof(Actor*));
    nlist = (Actor**) malloc(ninjas * sizeof(Actor*));
    init();
    
    for (int i = 0; i < pirates; i++){
        Actor* p = (Actor*) malloc(sizeof(Actor));
        p->arrival = (int*) malloc(sizeof(int) * MAX_VISITS);
        p->waiting = (int*) malloc(sizeof(int) * MAX_VISITS);
        p->visits = 0;
        p->index = i;
        plist[i] = p;
    }
    for (int i = 0; i < ninjas; i++){
        Actor* n = (Actor*) malloc(sizeof(Actor));
        n->arrival = (int*) malloc(sizeof(int) * MAX_VISITS);
        n->waiting = (int*) malloc(sizeof(int) * MAX_VISITS);
        n->visits = 0;
        n->index = i;
        nlist[i] = n;
    }

    // ** Simulation **
    // create threads
    for (int i = 0; i < pirates; i++){
        pthread_create(&(plist[i]->tid), NULL, pirateFunction, plist[i]);
    }
    for (int i = 0; i < ninjas; i++){
        pthread_create(&(nlist[i]->tid), NULL, ninjaFunction, nlist[i]);
    }

    // join threads 
    for (int i = 0; i < pirates; i++){
        pthread_join(plist[i]->tid, NULL);
    }
    for (int i = 0; i < ninjas; i++){
        pthread_join(nlist[i]->tid, NULL);
    }
    
    operatingtime = time(NULL) - opentime;
    print_stats();

    return 0;
}

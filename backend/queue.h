#ifndef QUEUE_H
#define QUEUE_H

#include "patient.h"

#define MAX_WAIT 300

typedef struct {
    Patient data[MAX_WAIT];
    int front, rear, count;
} Queue;

void queue_init(Queue *q);
int queue_empty(Queue *q);
void enqueue(Queue *q, Patient p);
Patient dequeue(Queue *q);

#endif

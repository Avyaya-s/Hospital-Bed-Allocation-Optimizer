#include "queue.h"

void queue_init(Queue *q) {
    q->front = q->rear = q->count = 0;
}

int queue_empty(Queue *q) {
    return q->count == 0;
}

void enqueue(Queue *q, Patient p) {
    q->data[q->rear] = p;
    q->rear = (q->rear + 1) % MAX_WAIT;
    q->count++;
}

Patient dequeue(Queue *q) {
    Patient p = q->data[q->front];
    q->front = (q->front + 1) % MAX_WAIT;
    q->count--;
    return p;
}

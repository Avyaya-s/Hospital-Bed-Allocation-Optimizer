#ifndef PREDICT_H
#define PREDICT_H

#define MAX_PRED 200

typedef struct {
    int bed_id;
    long discharge_time;
} PredNode;

typedef struct {
    PredNode data[MAX_PRED];
    int size;
} MinHeap;

void pred_init(MinHeap *h);
void pred_insert(MinHeap *h, PredNode n);
PredNode pred_extract_min(MinHeap *h);
PredNode pred_peek(MinHeap *h);
int pred_empty(MinHeap *h);

#endif

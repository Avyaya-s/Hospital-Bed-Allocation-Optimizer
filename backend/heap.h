#ifndef HEAP_H
#define HEAP_H

#include "patient.h"

#define MAX_PATIENTS 300

typedef struct {
    Patient data[MAX_PATIENTS];
    int size;
} MaxHeap;

void heap_init(MaxHeap *h);
void heap_insert(MaxHeap *h, Patient p);
Patient heap_extract_max(MaxHeap *h);
int heap_empty(MaxHeap *h);

#endif

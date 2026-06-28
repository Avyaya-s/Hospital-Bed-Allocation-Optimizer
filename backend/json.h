#ifndef JSON_H
#define JSON_H

#include "bed.h"
#include "queue.h"
#include "predict.h"
#include "heap.h"

void export_full_status_to_json(Bed beds[], int count, Queue *q, MinHeap *h, const char *filename);

#endif

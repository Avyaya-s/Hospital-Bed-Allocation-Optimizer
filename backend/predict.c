#include "predict.h"

static void swap(PredNode *a, PredNode *b) {
    PredNode t = *a;
    *a = *b;
    *b = t;
}

void pred_init(MinHeap *h) {
    h->size = 0;
}

void pred_insert(MinHeap *h, PredNode n) {
    int i = h->size++;
    h->data[i] = n;

    while(i > 0) {
        int parent = (i - 1) / 2;
        if(h->data[parent].discharge_time <= h->data[i].discharge_time)
            break;
        swap(&h->data[parent], &h->data[i]);
        i = parent;
    }
}

PredNode pred_extract_min(MinHeap *h) {
    PredNode root = h->data[0];
    h->data[0] = h->data[--h->size];

    int i = 0;
    while(1) {
        int left = 2*i + 1;
        int right = 2*i + 2;
        int smallest = i;

        if(left < h->size &&
           h->data[left].discharge_time < h->data[smallest].discharge_time)
            smallest = left;

        if(right < h->size &&
           h->data[right].discharge_time < h->data[smallest].discharge_time)
            smallest = right;

        if(smallest == i) break;

        swap(&h->data[i], &h->data[smallest]);
        i = smallest;
    }

    return root;
}

PredNode pred_peek(MinHeap *h) {
    return h->data[0];
}

int pred_empty(MinHeap *h) {
    return h->size == 0;
}

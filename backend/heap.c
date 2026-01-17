#include "heap.h"

void heap_init(MaxHeap *h) {
    h->size = 0;
}

static void swap(Patient *a, Patient *b) {
    Patient t = *a;
    *a = *b;
    *b = t;
}

void heap_insert(MaxHeap *h, Patient p) {
    int i = h->size++;
    h->data[i] = p;

    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->data[parent].severity >= h->data[i].severity)
            break;
        swap(&h->data[parent], &h->data[i]);
        i = parent;
    }
}

Patient heap_extract_max(MaxHeap *h) {
    Patient root = h->data[0];
    h->data[0] = h->data[--h->size];

    int i = 0;
    while (1) {
        int left = 2*i + 1;
        int right = 2*i + 2;
        int largest = i;

        if (left < h->size &&
            h->data[left].severity > h->data[largest].severity)
            largest = left;

        if (right < h->size &&
            h->data[right].severity > h->data[largest].severity)
            largest = right;

        if (largest == i) break;

        swap(&h->data[i], &h->data[largest]);
        i = largest;
    }

    return root;
}

int heap_empty(MaxHeap *h) {
    return h->size == 0;
}

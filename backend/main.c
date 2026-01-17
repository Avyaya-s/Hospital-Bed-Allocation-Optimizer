#include <stdio.h>
#include <time.h>

#include "bed.h"
#include "hash.h"
#include "heap.h"
#include "queue.h"
#include "predict.h"

/* Find first free bed of required type */
Bed* find_free_bed(Bed beds[], int count, int type) {
    for(int i = 0; i < count; i++) {
        if(beds[i].type == type && beds[i].occupied == 0)
            return &beds[i];
    }
    return NULL;
}

int main() {
    Bed beds[MAX_BEDS];
    int bed_count;

    MaxHeap emergency;
    Queue waiting;
    MinHeap predict_heap;

    heap_init(&emergency);
    queue_init(&waiting);
    pred_init(&predict_heap);

    /* Initialize beds and hash table */
    init_beds(beds, &bed_count);
    hash_init();

    for(int i = 0; i < bed_count; i++)
        hash_insert(beds[i].bed_id, &beds[i]);

    printf("Hospital initialized with %d beds.\n\n", bed_count);

    /* Sample incoming patients */
    Patient p1 = {1, 5, 0, -1};
    Patient p2 = {2, 3, 0, -1};
    Patient p3 = {3, 4, 0, -1};
    Patient p4 = {4, 2, 0, -1};

    heap_insert(&emergency, p1);
    heap_insert(&emergency, p2);
    heap_insert(&emergency, p3);
    heap_insert(&emergency, p4);

    time_t now = time(NULL);

    /* Allocation loop */
    while(!heap_empty(&emergency)) {
        Patient p = heap_extract_max(&emergency);
        int required_type = p.severity;

        Bed* bed = find_free_bed(beds, bed_count, required_type);

        if(bed) {
            bed->occupied = 1;
            bed->admit_time = now;
            p.allocated_bed_id = bed->bed_id;

            /* Simulated average stay (higher severity → longer stay) */
            long avg_stay = 60 * (6 - required_type);   // seconds
            long discharge = now + avg_stay;

            PredNode node = {bed->bed_id, discharge};
            pred_insert(&predict_heap, node);

            printf("Patient %d allocated Bed %d (Type %d) | "
                   "Expected discharge in ~%ld seconds\n",
                   p.patient_id, bed->bed_id, bed->type, avg_stay);
        }
        else {
            enqueue(&waiting, p);
            printf("Patient %d queued (no bed available for type %d)\n",
                   p.patient_id, required_type);
        }
    }

    /* Prediction output */
    if(!pred_empty(&predict_heap)) {
        PredNode next = pred_peek(&predict_heap);
        printf("\nNext expected free bed: Bed %d\n", next.bed_id);
    } else {
        printf("\nNo discharge prediction available.\n");
    }

    return 0;
}

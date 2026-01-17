#include <stdio.h>
#include "bed.h"
#include "hash.h"
#include "heap.h"
#include "queue.h"

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

    heap_init(&emergency);
    queue_init(&waiting);

    init_beds(beds, &bed_count);
    hash_init();

    for(int i = 0; i < bed_count; i++)
        hash_insert(beds[i].bed_id, &beds[i]);

    // Sample patients
    Patient p1 = {1, 5, 0, -1};
    Patient p2 = {2, 3, 0, -1};
    Patient p3 = {3, 4, 0, -1};

    heap_insert(&emergency, p1);
    heap_insert(&emergency, p2);
    heap_insert(&emergency, p3);

    while(!heap_empty(&emergency)) {
        Patient p = heap_extract_max(&emergency);
        int required_type = p.severity;

        Bed* bed = find_free_bed(beds, bed_count, required_type);

        if(bed) {
            bed->occupied = 1;
            p.allocated_bed_id = bed->bed_id;
            printf("Patient %d allocated Bed %d (Type %d)\n",
                   p.patient_id, bed->bed_id, bed->type);
        } else {
            enqueue(&waiting, p);
            printf("Patient %d queued\n", p.patient_id);
        }
    }

    return 0;
}

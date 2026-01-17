#include <stdio.h>
#include "bed.h"
#include "hash.h"

int main() {
    Bed beds[MAX_BEDS];
    int bed_count = 0;

    init_beds(beds, &bed_count);
    printf("Total beds initialized: %d\n\n", bed_count);

    hash_init();

    for(int i = 0; i < bed_count; i++) {
        hash_insert(beds[i].bed_id, &beds[i]);
    }

    Bed *b = hash_search(10);
    if(b) {
        printf("Lookup Test -> Bed %d found, Type = %d\n",
               b->bed_id, b->type);
    } else {
        printf("Lookup failed\n");
    }

    return 0;
}

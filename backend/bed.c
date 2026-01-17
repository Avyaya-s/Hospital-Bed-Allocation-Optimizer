#include <stdio.h>
#include "bed.h"

void init_beds(Bed beds[], int *count) {
    int id = 1;
    int i = 0;

    for(int j = 0; j < 100; j++)
        beds[i++] = (Bed){id++, GENERAL, 0, 0};

    for(int j = 0; j < 50; j++)
        beds[i++] = (Bed){id++, SEMI_PRIVATE, 0, 0};

    for(int j = 0; j < 30; j++)
        beds[i++] = (Bed){id++, PRIVATE, 0, 0};

    for(int j = 0; j < 10; j++)
        beds[i++] = (Bed){id++, ICU, 0, 0};

    for(int j = 0; j < 5; j++)
        beds[i++] = (Bed){id++, VENTILATOR, 0, 0};

    *count = i;
}

void print_beds(Bed beds[], int count) {
    for(int i = 0; i < count; i++) {
        printf("Bed %3d | Type %d | Occupied %d\n",
               beds[i].bed_id, beds[i].type, beds[i].occupied);
    }
}

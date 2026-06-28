#include <stdio.h>
#include "bed.h"

void init_beds(Bed beds[], int *count) {
    int id = 1;
    int i = 0;

    // Helper macro for cleaner code
    #define INIT_BED(t) (Bed){id++, t, BED_FREE, 0, 0}

    for(int j = 0; j < 20; j++)
        beds[i++] = INIT_BED(GENERAL);

    for(int j = 0; j < 10; j++)
        beds[i++] = INIT_BED(SEMI_PRIVATE);

    for(int j = 0; j < 6; j++)
        beds[i++] = INIT_BED(PRIVATE);

    for(int j = 0; j < 4; j++)
        beds[i++] = INIT_BED(ICU);

    for(int j = 0; j < 2; j++)
        beds[i++] = INIT_BED(VENTILATOR);

    *count = i;
}

const char* get_state_str(BedState s) {
    switch(s) {
        case BED_FREE: return "FREE";
        case BED_OCCUPIED: return "OCCUPIED";
        case BED_DISCHARGE_APPROVED: return "DISCHARGE_APPROVED";
        default: return "UNKNOWN";
    }
}

void print_beds(Bed beds[], int count) {
    for(int i = 0; i < count; i++) {
        printf("Bed %3d | Type %d | State: %-18s | Patient: %d\n",
               beds[i].bed_id, beds[i].type, get_state_str(beds[i].state), beds[i].current_patient_id);
    }
}

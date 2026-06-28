#ifndef BED_H
#define BED_H

#define MAX_BEDS 42

typedef enum {
    GENERAL = 1,
    SEMI_PRIVATE,
    PRIVATE,
    ICU,
    VENTILATOR
} BedType;

typedef enum {
    BED_FREE,
    BED_OCCUPIED,
    BED_DISCHARGE_APPROVED
} BedState;

typedef struct {
    int bed_id;
    BedType type;
    BedState state;      // Replaces occupied flag
    int current_patient_id; // Added for tracking
    long admit_time;
} Bed;

void init_beds(Bed beds[], int *count);
void print_beds(Bed beds[], int count);

#endif

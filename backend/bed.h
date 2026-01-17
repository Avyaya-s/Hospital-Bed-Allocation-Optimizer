#ifndef BED_H
#define BED_H

#define MAX_BEDS 145   // 100 + 50 + 30 + 10 + 5

typedef enum {
    GENERAL = 1,
    SEMI_PRIVATE,
    PRIVATE,
    ICU,
    VENTILATOR
} BedType;

typedef struct {
    int bed_id;
    BedType type;
    int occupied;        // 0 = free, 1 = occupied
    long admit_time;
} Bed;

void init_beds(Bed beds[], int *count);
void print_beds(Bed beds[], int count);

#endif

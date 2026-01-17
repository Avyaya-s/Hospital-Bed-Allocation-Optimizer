#ifndef PATIENT_H
#define PATIENT_H

typedef struct {
    int patient_id;
    int severity;          // 1–5
    long arrival_time;
    int allocated_bed_id;  // -1 if waiting
} Patient;

#endif

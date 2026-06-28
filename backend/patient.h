#ifndef PATIENT_H
#define PATIENT_H

typedef struct {
    int patient_id;
    int severity;          // 1-5
    int recovery_score;    // 1-5, doctor input, used ONLY for prediction
    int emergency;         // 1 = emergency, 0 = normal
    long arrival_time;
    int allocated_bed_id;  // -1 if waiting
} Patient;

#endif

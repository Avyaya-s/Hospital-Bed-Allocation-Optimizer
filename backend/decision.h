#ifndef DECISION_H
#define DECISION_H

#include "patient.h"
#include "bed.h"

typedef enum {
    DECISION_ADMIT,
    DECISION_RECOMMEND_TRANSFER,
    DECISION_REJECT
} AdmissionDecision;

/*
 * Evaluates feasibility of admission for an emergency patient.
 * Reports ONLY feasibility. Does NOT perform transfer or allocation.
 *
 * Logic:
 * - If BED_FREE exists -> DECISION_ADMIT
 * - Else if BED_DISCHARGE_APPROVED exists -> DECISION_RECOMMEND_TRANSFER
 * - Else -> DECISION_REJECT
 */
AdmissionDecision evaluate_emergency(Patient p, Bed beds[], int bed_count);

#endif

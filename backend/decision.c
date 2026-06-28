#include <stdio.h>
#include "decision.h"

AdmissionDecision evaluate_emergency(Patient p, Bed beds[], int bed_count) {
    int required_type = p.severity; // 1-5 maps directly to BedType
    
    int free_found = 0;
    int transfer_possible = 0;

    for (int i = 0; i < bed_count; i++) {
        if (beds[i].type == required_type) {
            if (beds[i].state == BED_FREE) {
                free_found = 1;
                break; // Found immediate slot
            }
            if (beds[i].state == BED_DISCHARGE_APPROVED) {
                transfer_possible = 1;
            }
        }
    }

    if (free_found) {
        return DECISION_ADMIT;
    } else if (transfer_possible) {
        return DECISION_RECOMMEND_TRANSFER;
    } else {
        return DECISION_REJECT;
    }
}

#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#include "bed.h"
#include "hash.h"
#include "heap.h"
#include "queue.h"
#include "predict.h"
#include "json.h"
#include "decision.h"
#include "logger.h"

/* Find first free bed of required type */
Bed* find_free_bed(Bed beds[], int count, int type) {
    for (int i = 0; i < count; i++) {
        if (beds[i].type == type && beds[i].state == BED_FREE)
            return &beds[i];
    }
    return NULL;
}

/* Manual Bed Discharge (Unallocate) by Bed ID */
void manual_discharge_bed(
    int bed_id,
    Bed beds[],
    int bed_count,
    Queue *waiting,
    MinHeap *predict_heap
) {
    Bed *bed = hash_search(bed_id); // Using Hash Table for lookup

    if (!bed) {
        printf("Invalid bed ID.\n");
        return;
    }

    if (bed->state == BED_FREE) {
        printf("Bed already unallocated.\n");
        return;
    }

    int force = 0;
    if (bed->state == BED_OCCUPIED) {
        printf("Bed %d is NOT discharge approved (Patient %d).\n", bed->bed_id, bed->current_patient_id);
        printf("Force discharge? (1 = Yes, 0 = No): ");
        scanf("%d", &force);
        if (force != 1) {
            printf("Discharge cancelled.\n");
            return;
        }
    }

    // Perform discharge
    if (force) {
         printf("WARNING: Bed %d force discharged.\n", bed_id);
         log_event("FORCE_DISCHARGE", "Patient %d force discharged from Bed %d", bed->current_patient_id, bed_id);
    } else {
         printf("Bed %d discharged successfully.\n", bed_id);
         log_event("DISCHARGE", "Patient %d discharged from Bed %d", bed->current_patient_id, bed_id);
    }
    
    bed->state = BED_FREE;
    bed->current_patient_id = 0;
    bed->admit_time = 0;

    // Attempt allocation from waiting queue (Traverse for FIRST MATCH)
    if (!queue_empty(waiting)) {
        int idx = waiting->front;
        int found = 0;
        int allocation_idx = -1;

        for (int c = 0; c < waiting->count; c++) {
            if (waiting->data[idx].severity == bed->type) {
                allocation_idx = idx; 
                found = 1;
                break;
            }
            idx = (idx + 1) % MAX_WAIT;
        }

        if (found) {
             Patient p = queue_remove_at(waiting, allocation_idx); // Remove specific patient
             
             bed->state = BED_OCCUPIED;
             bed->current_patient_id = p.patient_id;
             bed->admit_time = time(NULL);
             
             // Prediction Logic
             long avg_stay = (60 * (6 - p.recovery_score))*(1 + 0.1*(p.severity - 1));
             long discharge_time = time(NULL) + avg_stay;
             PredNode new_node = { bed->bed_id, discharge_time };
             pred_insert(predict_heap, new_node);

             printf("REALLOCATE: Patient %d (Sev %d) allocated Bed %d from queue.\n", 
                    p.patient_id, p.severity, bed->bed_id);
             log_event("REALLOCATE", "Patient %d (Sev %d) allocated Bed %d from waiting queue", p.patient_id, p.severity, bed_id);
        } else {
             printf("Bed remains FREE. No matching severity in queue.\n");
        }
    }
}

/* Handle patient admission */
void admit_patient(
    Bed beds[],
    int bed_count,
    MaxHeap *emergency,
    Queue *waiting,
    MinHeap *predict_heap
) {
    Patient p;
    time_t now = time(NULL);

    printf("\nEnter Patient ID: ");
    scanf("%d", &p.patient_id);

    printf("Enter Severity (1-5) [Admission Priority]: ");
    scanf("%d", &p.severity);

    printf("Enter Recovery Score (1-5) [1=Slow, 5=Fast]: ");
    scanf("%d", &p.recovery_score);

    printf("Is this an Emergency? (1=Yes, 0=No): ");
    scanf("%d", &p.emergency);

    p.arrival_time = now;
    p.allocated_bed_id = -1;

    // DECISION SUPPORT FOR EMERGENCY
    if (p.emergency == 1) {
        AdmissionDecision dec = evaluate_emergency(p, beds, bed_count);
        
        if (dec == DECISION_REJECT) {
            printf("\nREJECT: Patient %d. Admission not possible - ward full.\n", p.patient_id);
            log_event("REJECT", "Patient %d (Sev %d, Emg 1) rejected - Ward Full", p.patient_id, p.severity);
            return;
        }
        if (dec == DECISION_RECOMMEND_TRANSFER) {
            printf("\nDECISION: Recommend transfer for Patient %d.\n", p.patient_id);
            log_event("TRANSFER_SUGGEST", "Recommend transfer for Patient %d (Sev %d)", p.patient_id, p.severity);
            // Recommendation only. Do not queue.
            return;
        }
        // If DECISION_ADMIT, proceed to standard allocation
    }

    Bed *bed = find_free_bed(beds, bed_count, p.severity);

    if (bed) {
        bed->state = BED_OCCUPIED;
        bed->admit_time = now;
        bed->current_patient_id = p.patient_id;
        p.allocated_bed_id = bed->bed_id;

        // Prediction based on RECOVERY SCORE, not severity
        long avg_stay = (60 * (6 - p.recovery_score))*(1 + 0.1*(p.severity - 1));
        long discharge = now + avg_stay;

        PredNode node = { bed->bed_id, discharge };
        pred_insert(predict_heap, node);

        printf("Patient %d allocated Bed %d.\n",
               p.patient_id, bed->bed_id);
        log_event("ADMIT", "Patient %d (Sev %d) allocated Bed %d", p.patient_id, p.severity, bed->bed_id);
    } else {
        // Only enqueue NON-EMERGENCY patients
        if (p.emergency == 0) {
            // Check for potential transfers before queuing
            for (int i=0; i<bed_count; i++) {
                if (beds[i].type == p.severity && beds[i].state == BED_DISCHARGE_APPROVED) {
                    printf("NOTICE: Bed %d (Patient %d) is discharge-approved. Transfer is possible.\n",
                           beds[i].bed_id, beds[i].current_patient_id);
                    log_event("NOTICE", "Transfer possible to Bed %d for Patient %d", beds[i].bed_id, p.patient_id);
                }
            }

            enqueue(waiting, p);
            printf("No bed available. Patient %d added to waiting queue.\n",
                   p.patient_id);
            log_event("ENQUEUE", "Patient %d (Sev %d) added to waiting queue", p.patient_id, p.severity);
        } else {
            // Emergency rejection if find_free_bed fails (should be caught by evaluate_emergency, but safety net)
             printf("\nREJECT: Patient %d. No standard bed available.\n", p.patient_id);
             log_event("REJECT", "Patient %d (Sev %d, Emg 1) rejected - No Bed", p.patient_id, p.severity);
        }
    }
}

// Keep discharge_patient for "Manage Bed State" (Doctor Approval flow)
void discharge_patient(
    Bed beds[],
    int bed_count,
    Queue *waiting,
    MinHeap *predict_heap
) {
    int bed_id;
    printf("Enter Bed ID to manage: ");
    scanf("%d", &bed_id);

    Bed *bed = NULL;
    // Linear search is fine here, or use hash
    for (int i = 0; i < bed_count; i++) {
        if (beds[i].bed_id == bed_id) {
            bed = &beds[i];
            break;
        }
    }

    if (!bed) {
        printf("Error: Bed %d not found.\n", bed_id);
        return;
    }

    printf("Current State: ");
    if (bed->state == BED_FREE) printf("FREE\n");
    else if (bed->state == BED_OCCUPIED) printf("OCCUPIED (Patient %d)\n", bed->current_patient_id);
    else if (bed->state == BED_DISCHARGE_APPROVED) printf("DISCHARGE APPROVED (Patient %d)\n", bed->current_patient_id);

    if (bed->state == BED_FREE) {
        printf("Bed is already free.\n");
        return;
    }

    if (bed->state == BED_OCCUPIED) {
        int confirm;
        printf("Action: Approve Discharge? (1=Yes, 0=Cancel): ");
        scanf("%d", &confirm);
        if (confirm == 1) {
            bed->state = BED_DISCHARGE_APPROVED;
            printf("Bed %d marked as DISCHARGE APPROVED.\n", bed_id);
            log_event("APPROVE_DISCHARGE", "Bed %d (Patient %d) marked for discharge", bed_id, bed->current_patient_id);
        }
    } 
    else if (bed->state == BED_DISCHARGE_APPROVED) {
        int confirm;
        printf("Action: Finalize Discharge & Clean? (1=Yes, 0=Cancel): ");
        scanf("%d", &confirm);
        if (confirm == 1) {
            printf("Patient %d discharged from Bed %d.\n", bed->current_patient_id, bed_id);
            log_event("DISCHARGE", "Patient %d discharged from Bed %d", bed->current_patient_id, bed_id);
            
            bed->state = BED_FREE;
            bed->current_patient_id = 0;
            bed->admit_time = 0;
            
            // Queue Check (Full Scan)
            if (!queue_empty(waiting)) {
                int idx = waiting->front;
                int found = 0;
                int allocation_idx = -1;

                for (int c = 0; c < waiting->count; c++) {
                    if (waiting->data[idx].severity == bed->type) {
                        allocation_idx = idx; 
                        found = 1;
                        break;
                    }
                    idx = (idx + 1) % MAX_WAIT;
                }

                if (found) {
                     Patient p = queue_remove_at(waiting, allocation_idx);
                     
                     bed->state = BED_OCCUPIED;
                     bed->current_patient_id = p.patient_id;
                     bed->admit_time = time(NULL);
                     
                     long avg_stay = (60 * (6 - p.recovery_score))*(1 + 0.1*(p.severity - 1));
                     long discharge_time = time(NULL) + avg_stay;
                     PredNode new_node = { bed->bed_id, discharge_time };
                     pred_insert(predict_heap, new_node);

                     printf("REALLOCATE: Patient %d (Sev %d) allocated Bed %d from queue.\n", 
                            p.patient_id, p.severity, bed->bed_id);
                     log_event("REALLOCATE", "Patient %d (Sev %d) allocated Bed %d from waiting queue", p.patient_id, p.severity, bed_id);
                } else {
                     printf("Bed remains FREE. No matching severity in queue.\n");
                }
            }
        }
    }
}

void display_waiting_queue(Queue *q) {
    if (queue_empty(q)) {
        printf("Waiting queue is empty.\n");
        return;
    }

    printf("\n--- Waiting Queue (FIFO Order) ---\n");

    int idx = q->front;

    for (int c = 0; c < q->count; c++) {
        Patient p = q->data[idx];
        printf("Patient ID: %d | Severity: %d\n",
               p.patient_id, p.severity);

        idx = (idx + 1) % MAX_WAIT;
    }
}



int main() {
    Bed beds[MAX_BEDS];
    int bed_count;

    MaxHeap emergency;
    Queue waiting;
    MinHeap predict_heap;

    heap_init(&emergency); // Not extensively used in new logic but kept for struct compatibility if needed
    queue_init(&waiting);
    pred_init(&predict_heap);

    init_beds(beds, &bed_count);
    hash_init();

    for (int i = 0; i < bed_count; i++)
        hash_insert(beds[i].bed_id, &beds[i]);

    printf("Hospital initialized with %d beds.\n", bed_count);

    int choice;

    while (1) {
        printf("\n===== Hospital Bed Allocation Menu =====\n");
        printf("1. Admit new patient\n");
        printf("2. Show next predicted free bed (INFO)\n");
        printf("3. Export bed status to JSON\n");
        printf("4. Manage Bed State (Discharge Support)\n");
        printf("5. Show waiting queue\n");
        printf("6. Unallocate / discharge bed by bed ID (Manual Override)\n");
        printf("7. Exit\n");
        printf("Enter choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1:
                admit_patient(beds, bed_count, &emergency, &waiting, &predict_heap);
                break;

            case 2:
                if (!pred_empty(&predict_heap)) {
                    PredNode next = pred_peek(&predict_heap);
                    long remaining = next.discharge_time - time(NULL);
                    if (remaining < 0) remaining = 0;
                    printf("Next predicted free bed: Bed %d in ~%ld hours.\n", next.bed_id, remaining/60);
                } else {
                    printf("No discharge predictions available.\n");
                }
                break;

            case 3:
                export_full_status_to_json(beds, bed_count, &waiting, &predict_heap, "data/beds.json");
                export_logs_to_json("data/events.json");
                break;

            case 4:
                discharge_patient(beds, bed_count, &waiting, &predict_heap);
                break;

            case 5:
                display_waiting_queue(&waiting);
                break;

            case 6: // Manual Discharge
                {
                    int bid;
                    printf("Enter Bed ID to unallocate: ");
                    scanf("%d", &bid);
                    manual_discharge_bed(bid, beds, bed_count, &waiting, &predict_heap);
                }
                break;

            case 7:
                printf("Exiting system.\n");
                return 0;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}

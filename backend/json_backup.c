#include <stdio.h>
#include "json.h"

void export_full_status_to_json(Bed beds[], int count, Queue *q, MinHeap *h, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error: Cannot create JSON file.\n");
        return;
    }

    fprintf(fp, "{\n");

    // 1. BEDS
    fprintf(fp, "  \"beds\": [\n");
    for (int i = 0; i < count; i++) {
        fprintf(fp,
            "    { \"id\": %d, \"type\": %d, \"state\": %d, \"patient_id\": %d }%s\n",
            beds[i].bed_id,
            beds[i].type,
            beds[i].state,
            beds[i].current_patient_id,
            (i < count - 1) ? "," : ""
        );
    }
    fprintf(fp, "  ],\n");

    // 2. QUEUE (FIFO)
    fprintf(fp, "  \"queue\": [\n");
    if (q && q->count > 0) {
        int idx = q->front;
        for (int c = 0; c < q->count; c++) {
            Patient p = q->data[idx];
            fprintf(fp,
                "    { \"patient_id\": %d, \"severity\": %d, \"emergency\": %d }%s\n",
                p.patient_id,
                p.severity,
                p.emergency,
                (c < q->count - 1) ? "," : ""
            );
            idx = (idx + 1) % MAX_WAIT;
        }
    }
    fprintf(fp, "  ],\n");

    // 3. MIN-HEAP (Prediction)
    // Export raw array for visualization
    fprintf(fp, "  \"heap\": [\n");
    if (h) {
        for (int i = 0; i < h->size; i++) {
            fprintf(fp,
                "    { \"bed_id\": %d, \"discharge_time\": %ld }%s\n",
                h->data[i].bed_id,
                h->data[i].discharge_time,
                (i < h->size - 1) ? "," : ""
            );
        }
    }
    fprintf(fp, "  ]\n");

    fprintf(fp, "}\n");
    fclose(fp);

    printf("\nJSON exported successfully to %s\n", filename);
}

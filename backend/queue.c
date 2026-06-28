#include "queue.h"

void queue_init(Queue *q) {
    q->front = q->rear = q->count = 0;
}

int queue_empty(Queue *q) {
    return q->count == 0;
}

void enqueue(Queue *q, Patient p) {
    q->data[q->rear] = p;
    q->rear = (q->rear + 1) % MAX_WAIT;
    q->count++;
}

Patient dequeue(Queue *q) {
    Patient p = q->data[q->front];
    q->front = (q->front + 1) % MAX_WAIT;
    q->count--;
    return p;
}

/* Remove patient at specific physical index (Circular shift) */
Patient queue_remove_at(Queue *q, int phys_idx) {
    Patient removed = q->data[phys_idx];
    
    // Shift elements from phys_idx to rear
    int curr = phys_idx;
    int next = (curr + 1) % MAX_WAIT;
    
    // We traverse until we hit rear. Rear points to next empty slot.
    // So distinct items are up to (rear - 1).
    // Loop count: number of items after phys_idx.
    
    // Alternative: Just iterate count times? No.
    // Use simple shift logic until `next` reaches `rear`
    
    // How many items to shift?
    // It's tricky with circular.
    // Let's rely on count.
    
    // Easier robust way: Rebuild queue?
    // No, let's do the shift. It is standard.
    
    int items_to_move = 0;
    // Calculate distance from phys_idx to rear
    if (q->rear > phys_idx) items_to_move = q->rear - phys_idx - 1;
    else items_to_move = (MAX_WAIT - phys_idx) + q->rear - 1;
    
    // Actually, simpler loop condition:
    while (curr != ((q->rear - 1 + MAX_WAIT) % MAX_WAIT)) {
        q->data[curr] = q->data[next];
        curr = next;
        next = (next + 1) % MAX_WAIT;
    }
    
    q->rear = (q->rear - 1 + MAX_WAIT) % MAX_WAIT;
    q->count--;
    
    return removed;
}

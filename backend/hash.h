#ifndef HASH_H
#define HASH_H

#include "bed.h"

#define TABLE_SIZE 211

typedef struct Node {
    int key;
    Bed *bed;
    struct Node *next;
} Node;

void hash_init();
void hash_insert(int key, Bed *bed);
Bed* hash_search(int key);

#endif

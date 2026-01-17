#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

static Node* table[TABLE_SIZE];

int hash(int key) {
    return key % TABLE_SIZE;
}

void hash_init() {
    for(int i = 0; i < TABLE_SIZE; i++)
        table[i] = NULL;
}

void hash_insert(int key, Bed *bed) {
    int idx = hash(key);
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->key = key;
    newNode->bed = bed;
    newNode->next = table[idx];
    table[idx] = newNode;
}

Bed* hash_search(int key) {
    int idx = hash(key);
    Node *curr = table[idx];
    while(curr) {
        if(curr->key == key)
            return curr->bed;
        curr = curr->next;
    }
    return NULL;
}

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define TABLE_SIZE 1024
#define MAX_CAPACITY 0.75

// fnv1a hash algorithm 
uint64_t hashtable_hash(const void *data, size_t len)
{
    const unsigned char *bytes = data;
    uint64_t hash = 1469598103934665603ULL;

    for (size_t i = 0; i < len; i++) {
        hash ^= bytes[i];
        hash *= 1099511628211ULL;
    }

    return hash;
}

typedef struct node{
  int key;
  int value;
  struct node *next;
} node;

typedef struct {
  int size;
  int count;
  node **buckets;
} hashtable;

int hashtable_hashing(const void *key, int ht_size){
  uint64_t hash_result = hashtable_hash(key, sizeof(int));
  return hash_result % ht_size;
}


hashtable *hashtable_init(int size){
  hashtable *ht = malloc(sizeof(hashtable));
  if (!ht) return NULL; // In failer of malloc asking OS for n bytes of heap memory it return NULL

  ht->size = size;
  ht->count = 0;
  ht->buckets = calloc(ht->size, sizeof(node *));
  if (!ht->buckets) {
    free(ht);
    return NULL;
  }
  
  return ht;
}

// have a function that adds node to the hashtable without checking for the capacity of the hashtable, due to a infinite loop can be occur.
int hashtable_add_raw(hashtable *ht, int key, int value)
{
    int index = hashtable_hashing(&key, ht->size);

    node *n = malloc(sizeof(node));
    if (!n) return 0;

    n->key = key;
    n->value = value;
    n->next = ht->buckets[index];
    ht->buckets[index] = n;
    ht->count++;

    return 1;
}

int hashtable_exists(hashtable *ht, int *key){
    int index = hashtable_hashing(key, ht->size);

    node *current = ht->buckets[index];

    while (current) {
        if (current->key == *key) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

node *hashtable_getk(hashtable *ht, int key){
	int index = hashtable_hashing(&key, ht->size);
	
	node *current = ht->buckets[index];

    while (current) {
        if (current->key == key) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}



void hashtable_print(hashtable *ht){
  int i;
  
  for(i=0; i < ht->size; i++){
    node *current = ht->buckets[i];
    
    for(; current != NULL; current = current->next){
      printf("{key:%d, value:%d} ", current->key, current->value);
    }
  }
}

void hashtable_deinit(hashtable *ht){
  
  for(int i=0; i < ht->size; i++){
    node *current = ht->buckets[i];
    
    while(current){
      node *next = current->next;
      free(current);
      current = next;
    }
  }
  
  free(ht->buckets);
  free(ht);
}


int hashtable_resize(hashtable **htp, int size)
{
    // double pointer because we must update the caller’s pointer
    hashtable *old = *htp;
    hashtable *new = hashtable_init(size);

    if (!new)
        return 0;

    for (int i = 0; i < old->size; i++) {
        node *current = old->buckets[i];
        while (current) {
            if (!hashtable_add_raw(new, current->key, current->value)) {
                hashtable_deinit(new);
                return 0;
            }
            current = current->next;
        }
    }

    hashtable_deinit(old);
    *htp = new;
    return 1;
}

int hashtable_add(hashtable **htp, int key, int value){
  
  hashtable *ht = *htp;
  int index = hashtable_hashing(&key, ht->size);
  
  if (!hashtable_add_raw(ht, key, value))
      return 0;

  double load = (double)ht->count / ht->size;
  if (load > MAX_CAPACITY) {
    if (!hashtable_resize(htp, ht->size * 2)){
        return 0;
    }
  }

  return 1;
}




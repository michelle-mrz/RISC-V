#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"
#include "hashtable.h"

struct hashtable
{
    // TODO: define hashtable struct to use linkedlist as buckets
    linkedlist_t **items; // linked lists
    int num_buckets; // number of buckets in ht
};

/**
 * Hash function to hash a key into the range [0, max_range)
 */
static int hash(int key, int max_range)
{
    // TODO: feel free to write your own hash function (NOT REQUIRED)
    key = (key > 0) ? key : -key;
    return key % max_range;
}

hashtable_t *ht_init(int num_buckets)
{
    // TODO: create a new hashtable
    hashtable_t *hashtable = malloc(sizeof(hashtable_t));
    hashtable->items = malloc(sizeof(linkedlist_t*)*num_buckets);
    hashtable->num_buckets = num_buckets;

    int a ;
    for (a = 0; a<num_buckets; a++) {
        hashtable->items[a] = ll_init();
    }

    return hashtable;

}

void ht_free(hashtable_t *table)
{
    //TODO: free a hashtable from memory
    int a;
    for (a = 0; a < table->num_buckets; a++) {
        ll_free(table->items[a]);
    }
    free(table->items);
    free(table);
}

void ht_add(hashtable_t *table, int key, int value)
{
    // TODO: create a new mapping from key -> value.
    // If the key already exists, replace the value.
    // hash table full????
    int index = hash(key,table->num_buckets);

    linkedlist_t *current_item = table->items[index]; // create
    ll_add(current_item,key,value);
}

int ht_get(hashtable_t *table, int key)
{
    // TODO: retrieve the value mapped to the given key.
    // If it does not exist, return 0
    int index = hash(key,table->num_buckets);

    linkedlist_t *current_item = table->items[index]; // create
    int val = ll_get(current_item,key);

    return val;
}

int ht_size(hashtable_t *table)
{
    // TODO: return the number of mappings in this hashtable
    int size = 0;
    int a;

    for (a = 0; a < table->num_buckets; a++) {
        size = size + ll_size(table->items[a]);
    }

    return size;

}

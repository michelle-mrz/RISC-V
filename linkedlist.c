#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

struct linkedlist
{
    struct linkedlist_node *first;
    // TODO: define linked list metadata
    int size;
};

struct linkedlist_node
{
    // TODO: define the linked list node
    int value;
    int key;
    struct linkedlist_node * next;
};
typedef struct linkedlist_node linkedlist_node_t;

linkedlist_t *ll_init()
{
    // TODO: create a new linked list

    // We have done this TODO for you as an example of how to use malloc().
    // You might want to read more about malloc() from Linux Manual page.
    // Usually free() should be used together with malloc(). For example,
    // the linkedlist you are currently implementing usually have free() in the
    // ll_free() function.

    // First, you use C's sizeof function to determine
    // the size of the linkedlist_t data type in bytes.
    // Then, you use malloc to set aside that amount of space in memory.
    // malloc returns a void pointer to the memory you just allocated, but
    // we can assign this pointer to the specific type we know we created
    linkedlist_t *list = malloc(sizeof(linkedlist_t));

    // TODO: set metadata for your new list and return the new list
    list->first =  NULL;
    list->size = 0;
    return list;
}

void ll_free(linkedlist_t *list)
{
    // TODO: free a linked list from memory
    linkedlist_node_t *current_node = list->first; // start with first node 
    while (current_node != NULL) {
        linkedlist_node_t *temp = current_node;
        current_node = current_node->next;
        free(temp);
    }
    free(list);
    
}

linkedlist_node_t *find_node(linkedlist_t *list,int key) {
    linkedlist_node_t *current_node = list->first; // start with first node 

    if (list->first == NULL) { // if list is empty 
        return NULL;
    }

    while (current_node->key != key) { // looping thru list
        if (current_node->next == NULL) { // if there's no dupe
            return NULL;
        }
        else {
            current_node = current_node->next;
        }
    }

    // if it's found
    return current_node;


}

void ll_add(linkedlist_t *list, int key, int value)
{
    // TODO: create a new node and add to the front of the linked list if a
    // node with the key does not already exist.
    // Otherwise, replace the existing value with the new value.

    // find node with key
    linkedlist_node_t *found_node = find_node(list,key);

    // // Create new node 
    // linkedlist_node_t *new_node = malloc(sizeof(linkedlist_node_t)); //?
    // new_node->key = key;
    // new_node->value = value;
    // new_node->next = NULL;

    if (found_node != NULL) { // if there is dupe 
        found_node->value = value;
    }
    else {
        // Create new node 
        linkedlist_node_t *new_node = malloc(sizeof(linkedlist_node_t)); //?
        new_node->key = key;
        new_node->value = value;
        new_node->next = NULL;

        if (list->size == 0) { // if list is empty
        // assign head to new node
        list->first = new_node;
        list->size = 1;
      }
        else if (found_node == NULL) { // if no dupes 
            new_node->next = list->first;
            list->first = new_node;
            list->size = list->size + 1;
        }
    }
}

int ll_get(linkedlist_t *list, int key)
{
    // TODO: go through each node in the linked list and return the value of
    // the node with a matching key.
    // If it does not exist, return 0.
    linkedlist_node_t *new_node = find_node(list,key);
    if (new_node == NULL) {
        return 0;
    }
    else {
        return new_node->value;
    }
}

int ll_size(linkedlist_t *list)
{
    // TODO: return the number of nodes in this linked list
    return list->size;
}

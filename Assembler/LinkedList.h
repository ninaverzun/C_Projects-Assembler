#ifndef LINKEDLIST_H_INCLUDED
#define LINKEDLIST_H_INCLUDED
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "FatalErrorHandling.h"

typedef struct node Node;

struct node
{
    void* data;
    Node* next;
};

/*Adds a new node with a given data pointer to the list and returns it's head node*/
Node* AddNode(Node* head, void* data);

/*Removes the head (first) node and returns the new head*/
Node* RemoveNode(Node* head);


#endif

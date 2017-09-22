#include "LinkedList.h"

/*For documentation see "LinkedList.h"!!! */

Node* AddNode(Node* head, void* data)
{
    Node* newHead;
    assert(data!=NULL);
    newHead=(Node*)malloc(sizeof(Node));
    if(newHead==NULL)
    {
        FatalMemoryAllocationError();
    }

    newHead->next=head;
    newHead->data=data;
    return newHead;
}

Node* RemoveNode(Node* head)
{
    Node* nextHead;

    if(head==NULL)
    {
        return NULL;
    }

    nextHead=head->next;
    free(head);
    return nextHead;
}

#include <stdio.h>
#include <stdlib.h>

#include "Queue.h"

Queue* createQueue(int size)
{
    int ii;
    Queue* q = (Queue*)malloc(sizeof(Queue) + (sizeof(Task*) * size)); //assigning memory to struct and flexible array member
    q->count = 0;
    q->max = size;

    for(ii = 0; ii < q->max; ii++)
    {
        q->arr[ii] = (Task*)malloc(sizeof(Task)); //initialising each task in queue
        q->arr[ii]->id = -1; //-1 for purposely empty values
        q->arr[ii]->burst = -1;
    }

    return q;
}

void ins(Task x, Queue* q)
{
    if(q->count < q->max) //if free spot in queue
    {
        *(q->arr[q->count]) = x; //add item x at end of queue
        (q->count)++; //increment last item index counter
    }
    else
    {
        perror("Queue full");
    }
}

Task rem(Queue* q)
{
    Task ret;
    int ii;
    ret = *(q->arr[0]); //storing value to be returned

    for(ii = 1; ii < q->count; ii++)
    {
        *(q->arr[ii-1]) = *(q->arr[ii]); //shufflinf the queue down after removal
    }

    if(q->count > 0)
    {
        q->arr[q->count-1]->id = -1; //adding blank item to end of queue after shuffle
        q->arr[q->count-1]->burst = -1;
        (q->count)--;
    }

    return ret;
}

void freeQ(Queue* q)
{
    int ii;
    
    for(ii = 0; ii < q->max; ii++)
    {
        free(q->arr[ii]); //freeing elements from queue
    }

    free(q); //freeing queue itself
}

int hasElement(Queue* q)
{
    int ret = FALSE;
    if(q->count > 0)
    {
        ret = TRUE; //has more than one element
    }

    return ret;
}

int isFull(Queue* q)
{
    int ret = FALSE;
    if(q->count >= q->max)
    {
        ret = TRUE; //has no free spaces
    }

    return ret;
}
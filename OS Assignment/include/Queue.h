#include "macros.h"

/*#ifndef Q
#define Q
typedef struct Queue{
    Task* arr[LEN]; // queue of Tasks of size LEN
    int count; //index of last item
}Queue;

#endif*/

Queue* createQueue(int size);
void ins(Task x, Queue* q);
Task rem(Queue* q);
void freeQ(Queue* q);
int hasElement(Queue* q);
int isFull(Queue* q);
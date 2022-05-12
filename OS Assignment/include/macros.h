//creating print statements for debug mode
#ifdef DEBUG
    #define DEBUG_FPRINTF(file,frmt,prnt) fprintf(file,frmt,prnt)
    #define DBG 1
#else
    #define DEBUG_FPRINTF(file,frmt,prnt)
    #define DBG 0
#endif

//creating boolean operators for C
#define TRUE 1
#define FALSE 0

/*defining max array/queue size*/
//#define MAX 10

/*defining max char array size*/
#define LEN 100

#ifndef PTHREAD
#define PTHREAD
#include <pthread.h>
#endif

#ifndef TASKSTRUCT
#define TASKSTRUCT
typedef struct Task{ 
    int id;
    int burst; 
    char* aTime; //time of arrival in HH:MM:ss + \0
}Task;
#endif

#ifndef Q
#define Q
typedef struct Queue{
    int count; //index of last item
    int max;
    Task* arr[]; // queue of Tasks of dynamic size via flexible array member
}Queue;
#endif

#ifndef PROCESSOR //CPU (macro naming would only remove potential function/variable names)
#define PROCESSOR 
typedef struct CPU{
    int id; //id number of CPU eg CPU-1
    int tasksServed; //total number of tasks completed
}CPU;
#endif

#ifndef CPUARGS
#define CPUARGS
typedef struct cpuArgs{
    CPU* c; //cpu details
    Queue* queue; //ready queue
    FILE* tFile; //task_file
    int* tSize;
    char* logF;
    pthread_mutex_t *mutex;
    pthread_cond_t *full;
    pthread_cond_t *empty;
}cpuArgs;
#endif

#ifndef TASKARGS
#define TASKARGS
typedef struct taskArgs{
    Queue* q;
    FILE* tFile;
    char* simLog;
    pthread_mutex_t *mutex;
    pthread_cond_t *full;
    pthread_cond_t *empty;
}taskArgs;
#endif
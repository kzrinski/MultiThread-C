//system made header files
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <unistd.h> //for sleep() function

//user made header files
#include "fileIO.h"
#include "macros.h"
#include "log.h"

//static functions the rest of the program doesnt need to know about
static void* task(void* inArgs);
static void* cpu(void* args);
static int notInvalid(Task t);
static int validateCMD(char** arr, int count);

//global variables to be shared between threads
//ONLY MODIFIED BY THREADS or ACCESSED BY REFERENCE
int num_tasks;
int total_waiting_time;
int total_turnaround_time;

int main(int argc, char const *argv[])
{
    pthread_t CPU0,CPU1,CPU2,TASK; //decleration of 3 CPU threads
    pthread_mutex_t m; //decleration of mutex lock
    pthread_cond_t full, empty; //decleration of thread condition

    int ii; //integer iterator & error flag for task/main thread
    cpuArgs** arg = (cpuArgs**)malloc(3 * sizeof(cpuArgs*)); //struct for variables to passed to CPU threads
    CPU** processors = (CPU**)malloc(3 * sizeof(CPU*)); //struct for holding CPU id and number of tasks each CPU services
    taskArgs* tArg = (taskArgs*)malloc(sizeof(taskArgs)); //struct holding variables passed to task()
    Queue* q = NULL; //Queue decleration
    FILE* f = NULL; //file decleration
    int* taskSize = (int*)malloc(sizeof(int));
    num_tasks = 0; //num_tasks initialisation 

    char taskFile[20]; //declaring stack memory for taskFile.
    char logFile[] = "./simulation_log"; //name of log file in this case always called simulation_log and to be stored in the root directory
    remove(logFile); //delete any old simulation_logs

    if(validateCMD((char**)argv,argc)) //ensure cmdline arguments are valid before attempting to open filename from cmdline
    {
        strcpy(taskFile,argv[1]);
        f = openFile(taskFile);
    }

    //initialising mutex's and conditions;
    pthread_mutex_init(&m,NULL);
    pthread_cond_init(&full,NULL);
    pthread_cond_init(&empty,NULL);

    if(f != NULL && validateFile(taskFile) && validateCMD((char**)argv,argc)) //check all FileIO and cmdline is valid before starting
    {
        Queue* q = createQueue(atoi(argv[2])); //creates a Queue the size of 3rd cmdline arg
        *taskSize = fileLines(taskFile); //getting number of tasks in file.

        //initialising thread args and CPU info for each thread
        for(ii = 0; ii < 3; ii++)
        {
            processors[ii] = (CPU*)malloc(sizeof(CPU)); //malloc CPU info to be passed around
            processors[ii]->id = ii;
            processors[ii]->tasksServed = 0;

            arg[ii] = (cpuArgs*)malloc(sizeof(cpuArgs)); //malloc thread args for CPU-ii
            arg[ii]->c = processors[ii]; //thread arg-ii gets processor-ii for (thread) CPU-ii
            arg[ii]->queue = q; //pointer to queue
            arg[ii]->tFile = f; //pointer to task file
            arg[ii]->tSize = taskSize; //informing cpu's how many tasks to complete
            arg[ii]->logF = logFile; //pointer to log file

            //addresses of mutex and conditions which are shared between threads
            arg[ii]->mutex = &m; 
            arg[ii]->full = &full;
            arg[ii]->empty = &empty;
        }

        //initialising struct to be passed to task thread
        tArg->q = q;
        tArg->tFile = f;
        tArg->simLog = logFile;
        tArg->mutex = &m;
        tArg->full = &full;
        tArg->empty = &empty;

        //If successful in the creating of thread x print to stderr.
        if(pthread_create(&CPU0,NULL,&cpu,(void*)arg[0]) == 0)
        {
            DEBUG_FPRINTF(stderr,"%s###LOG### [CPU-0] Successfully started\n","");
        }

        if(pthread_create(&CPU1,NULL,&cpu,(void*)arg[1]) == 0)
        {
            DEBUG_FPRINTF(stderr,"%s###LOG### [CPU-1] Successfully started\n","");
        }

        if(pthread_create(&CPU2,NULL,&cpu,(void*)arg[2]) == 0)
        {
            DEBUG_FPRINTF(stderr,"%s###LOG### [CPU-2] Successfully started\n","");
        }

        if(pthread_create(&TASK,NULL,&task,(void*)tArg) == 0)
        {
            DEBUG_FPRINTF(stderr,"%s###LOG### [TASK] Successfully started\n","");
        }

        //wait for CPU threads to complete before continuing
        pthread_join(CPU0,NULL);
        pthread_join(CPU1,NULL);
        pthread_join(CPU2,NULL);
        pthread_join(TASK,NULL);

        //log completion of task and CPU threads
        logDone(logFile, num_tasks, total_waiting_time, total_turnaround_time);

        //free memory in the succesful validation case
        for(ii = 0; ii < 3; ii++)
        {
            //free(arg[ii]->tSize);
            free(processors[ii]); //free cpu info
            free(arg[ii]); //free thread data
        }

        free(taskSize);
        free(tArg);
        free(processors); //free outer array for cpu info
        free(arg); //free outer array for thread data

        freeQ(q); //free queue
        closeFile(f); //close file

        //destroy mutex and conditions
        pthread_mutex_destroy(&m);
        pthread_cond_destroy(&empty);
        pthread_cond_destroy(&full);
    }
    else if (argc != 3) //if incorrect # of cmd args
    {
        if(f != NULL)
        {
            fclose(f); //close only if opened
        }
        fprintf(stderr,"###ERROR### Invalid # command line args\n"); //print error
        free(taskSize);
        free(tArg);
        free(processors); //same as previous free in successful case
        free(arg); // ^^

        if(q != NULL)
        {
            freeQ(q); //free queue if created
        }
    }
    else //same as else if but different error message
    {
        fprintf(stderr,"###ERROR### Error in cmdline arguments\n");
        if(f != NULL)
        {
            fclose(f);
            fprintf(stderr,"###ERROR### Invalid/Missing file: %s\n",taskFile);
        }
        free(taskSize);
        free(tArg);
        free(processors);
        free(arg);

        if(q != NULL)
        {
            freeQ(q);
        }
    }
    
    return 0;
}

//retrieve 1 or 2 tasks from task_file based on free space
static void* task(void* inArgs)
{
    int error = FALSE; //error flag

    Task task1,task2;

    taskArgs* args = (taskArgs*)inArgs; //convert void pointer to taskArgs

    while(error == FALSE) //while not at EOF or invalid line
    {
        pthread_mutex_lock(args->mutex);
        while(isFull(args->q) == TRUE)
        {
            pthread_cond_wait(args->full,args->mutex); //wait for consumer to consume an item
        }
        pthread_mutex_unlock(args->mutex);

        pthread_mutex_lock(args->mutex);
        if(isFull(args->q) == FALSE) //if ANY room
        {
            task1 = getTask(args->tFile); //retrieve from file
            if(notInvalid(task1)) //check validity
            {
                logArrival(args->simLog,task1); //log insertion to queue
                ins(task1,args->q); // insert to queue
                num_tasks++; //increment read elements
            }
            else
            {
                error = -1; //return error code
                ins(task1,args->q); //insert poison pill
            }
        }
        pthread_mutex_unlock(args->mutex);
        pthread_cond_signal(args->empty); //signal consumer of new items to consume


        //same as above but for a second task
        pthread_mutex_lock(args->mutex);
        if(isFull(args->q) == FALSE)
        {
            task2 = getTask(args->tFile);
            if(notInvalid(task2))
            {
                logArrival(args->simLog,task2); //log and insert into queue
                ins(task2,args->q);
                num_tasks++;
            }
            else
            {
                error = -1;
                ins(task2,args->q);
            }
        }

        DEBUG_FPRINTF(stderr, "###LOG### tasks read from file: %d\n",num_tasks); //debug print to stderr
        pthread_mutex_unlock(args->mutex);
        pthread_cond_signal(args->empty); //signal consumer of new items to consume
    }

    logTask(args->simLog, num_tasks); //log completion of task/main thread
    DEBUG_FPRINTF(stderr, "%s###LOG### [TASK] Stopping\n","");
    
    return NULL;
}

//function which runs on pthread_create for CPU's
static void* cpu(void* args)
{
    static int count = 0; //count of total tasks serviced
    int done = FALSE; //flag to indicate all tasks have been serviced
    Task temp;

    cpuArgs* details = (cpuArgs*)args; //conversion from void* back to cpuArgs*

    while(!done)
    {
        pthread_mutex_lock(details->mutex);
        while(hasElement(details->queue) == FALSE && count < *(details->tSize)) //while queue empty
        {
            pthread_cond_wait(details->empty,details->mutex); //wait for producer to produce
        }
        temp = rem(details->queue); //take element from queue
        pthread_cond_signal(details->full); //signal producer to produce
        count++;
        pthread_mutex_unlock(details->mutex);

        pthread_mutex_lock(details->mutex);
        if(temp.id > 0 && count <= *(details->tSize)) //if poison pill not reached and under max size
        {
            total_waiting_time += logService(details->logF,temp,*details->c); //add to total wait time and log service
            pthread_mutex_unlock(details->mutex);

            sleep(temp.burst); //"process" task
            details->c->tasksServed++; //add to individual CPU count

            pthread_mutex_lock(details->mutex);
            total_turnaround_time += logComplete(details->logF,temp,*details->c); //add to total turn around and log completion
            free(temp.aTime); //free arrival time for task as its no longer used.
            pthread_mutex_unlock(details->mutex);
        }
        else
        {
            done = TRUE; //if poison pill reached exit loop to kill thread
        }
    }

    logCPU(details->logF,*details->c); //log cpu thread finishing
    DEBUG_FPRINTF(stderr, "###LOG### [CPU-%d] Stopping\n", details->c->id); //debug print to stderr
    return NULL; //return a void* no return so NULL is used.
}

//basic check that task if not a poison pill (negative number)
static int notInvalid(Task t)
{
    int result = TRUE;

    if(t.id <= 0 || t.burst <= 0)
    {
        result = FALSE; //INVALID
    }

    return result;
}

//validate the command line arguments both for content and amount
static int validateCMD(char** arr, int count)
{
    int valid = FALSE; //default cmdline args are invalid until proven valid

    if(count == 3) //make sure there is a program name, file and queue size
    {
        if(atoi(arr[2]) <= 10 && atoi(arr[2]) > 0) //check valid queue size
        {
            valid = TRUE; //valid cmdline args
        }
    }

    return valid;
}
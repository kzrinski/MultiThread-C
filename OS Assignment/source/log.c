#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

//#include "Queue.h"
#include "fileIO.h"
#include "macros.h"
#include "log.h"

static int timeDiff(char* before, char* after); // static function for getting difference in seconds
static int convertSeconds(char* time); //converts time string to integer of seconds

static char* getTime() //gets current system time via ctime() function
{
    time_t curTime;
    char* strTime = (char*)malloc(9 * sizeof(char)); //HH:MM:ss and room for null terminator

    time(&curTime);
    sscanf(ctime(&curTime),"%*s %*s %*d %s:%s:%s %*s", strTime, &strTime[2], &strTime[4]); //sending relevent info to strTime

    return strTime;
}

//Logs the arrival of a task to the queue and gives a value to the task structs aTime value.
void logArrival(char* fName, Task t)
{
    char entry[LEN];
    char* currTime = getTime();
    strcpy(t.aTime,currTime);

    sprintf(entry, "task#%d: %d\nArrival time: %s\n", t.id, t.burst,t.aTime);
    writeFile(fName,entry); //write to simulation_log

    free(currTime); // as currTime is copied the original can be free'd
}

//Logs when a CPU begins to service a task as well as returning the wait time of the task
int logService(char* fName, Task t, CPU c)
{
    int waitTime;
    char entry1[LEN];
    char entry2[LEN];

    char* currTime = getTime();
    waitTime = timeDiff(t.aTime,currTime); //gets wait time to be returned

    sprintf(entry1,"Statistics for CPU-%d\ntask#%d: %d\n", c.id, t.id, t.burst);
    sprintf(entry2, "Arrival time: %s\nService time: %s\n", t.aTime, currTime);
    writeFile(fName,strcat(entry1,entry2)); //concats the two lines then sends to simulation_log

    free(currTime); // as currTime is copied the original can be free'd
    
    return(waitTime);
}

//Logs when a CPU is finished servincing a task as well as returning the turn around time of the task
int logComplete(char* fName, Task t, CPU c)
{
    int turnTime;
    char entry1[LEN];
    char entry2[LEN];

    char* currTime = getTime();
    turnTime = timeDiff(t.aTime,currTime); //gets turn around time to be returned

    sprintf(entry1,"Statistics for CPU-%d\ntask#%d: %d\n", c.id, t.id, t.burst);
    sprintf(entry2, "Arrival time: %s\nCompletion time: %s\n", t.aTime, currTime);
    writeFile(fName,strcat(entry1,entry2)); //concats the two lines then sends to simulation_log

    free(currTime); //as currTime is no longer used it may be free'd
    return(turnTime);
}

//to be called when all tasks have been read from file.
void logTask(char* fName, int numT)
{
    char entry1[LEN];
    char entry2[LEN];
    char* currTime = getTime();

    sprintf(entry1,"Number of tasks put into Ready-Queue: %d\n", numT);
    sprintf(entry2, "Terminated at time: %s\n", currTime);
    writeFile(fName,strcat(entry1,entry2)); //concats the two lines then sends to simulation_log

    free(currTime); //as currTime is no longer used it may be free'd
}

//to be called when a CPU has no more tasks to be serviced
void logCPU(char* fName, CPU c)
{
    char entry1[LEN];

    sprintf(entry1,"CPU-%d terminates after servicing %d tasks\n", c.id, c.tasksServed);
    writeFile(fName,entry1);

}

//to be called when all threads have completed, also calculating the average wait and turn around times.
void logDone(char* fName, int numT, int wTime, int tTime)
{
    char entry1[LEN];
    char entry2[LEN];
    char entry3[LEN];

    sprintf(entry1, "Number of tasks: %d\n", numT);
    sprintf(entry2, "Average waiting time: %ds\n", (wTime/numT));
    sprintf(entry3, "Average turn around time: %ds\n", (tTime/numT));
    strcat(entry2,entry3); //concats 2 string to make one
    writeFile(fName,strcat(entry1,entry2)); //concats the two lines then sends to simulation_log.

}

//returns an integer (in seconds) of the difference between two times
static int timeDiff(char* before, char* after)
{
    int diff = 0;

    diff = (convertSeconds(after) - convertSeconds(before));

    return diff;
}

//returns an integer of how many seconds a HH:mm:ss formated string contains
static int convertSeconds(char* t)
{
    int totalSecs = 0;
    int hours, mins, secs;

    sscanf(t,"%2d:%2d:%2d",&hours,&mins,&secs);

    totalSecs += secs;
    totalSecs += (mins * 60); // 60 seconds per minute
    totalSecs += (hours * 3600); // 3600 seconds per hour

    return totalSecs;
}
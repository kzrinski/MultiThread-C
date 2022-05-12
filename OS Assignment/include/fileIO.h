#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "macros.h"
#include "Queue.h"

FILE* openFile(char* fName);
Task getTask(FILE* file);
void closeFile(FILE* file);
int writeFile(char* fName,char* entry);
int validateFile(char* file);
int fileLines(char* file);


#include "fileIO.h"

static Task split(char* line);

//function which checks a file contains at least SOMETHING and leaves it open.
FILE* openFile(char* fName)
{
    FILE* file;
    int ch;

    file = fopen(fName,"r"); //opening file in read mode

    if(file != NULL)
    {
        ch = fgetc(file);
        if(ch != EOF) // if file isnt immediately empty rewind it.
        {
            rewind(file);
        }
        else
        {
            perror("###Error### file empty");
        }
    }
    else
    {
        perror("###Error### Error opening file");
    }

    return file; //return file pointer
    
}

//ensures that a task file follows the appropriate format and contains no blank or incorrect lines.
int validateFile(char* file)
{
    char temp[LEN]; // temp string to check each line
    int tempID, tempBurst; //temp string to check values of id and burst
    int ret = FALSE; //returns whether the file is valid (TRUE/FALSE) default is False
    int error = FALSE; //returns IF a line is invalid at ANY point, default is False.
    FILE* f = fopen(file,"r");

    if(f != NULL) //if file exists
    {
        while(fgets(temp,LEN,f) != NULL) //if line not empty or EOF
        {
            if(fscanf(f,"%d %d\n",&tempID,&tempBurst) == 2) //if only contains 2 integers
            {
                if(tempBurst > 50 && tempBurst <= 0) //if burst not within limits of 1-50 inclusive
                {
                    error = TRUE; //error occured
                }
            }
            else
            {
                error = TRUE; //error occured
            }
        }

        //if no errors occured the file is valid
        if(!error)
        {
            ret = TRUE;
        }

        fclose(f); //close file so it may be opened again at the top/
    }

    return ret;
}

//writes a string to specified file.
int writeFile(char* fName, char* entry)
{
    FILE* file;
    file = fopen(fName,"a"); //open file in append mode.
    int error = FALSE; //error flag for return.

    if(file != NULL) //if file opens successfully.
    {
        fputs("---\n",file); //To allow for easy differentiation between entries

        if(entry != NULL)
        {
            fputs(entry,file); //add line to file
            fflush(stdout); //making sure line is printed to file
        }
        else //entry is null
        {
            perror("Invalid string");
            error = TRUE;
        }

        if(ferror(file))//checking for file errors
        {
            perror("###Error### Error writing to file: ");
            fclose(file); //close file after error
            error = TRUE;
        }//else "do nothing"

        fclose(file); //close file after done writing
    }
    else
    {
        perror("###Error### Error Opening log file: ");
        error = TRUE;
    }
    
    return error; //returns error code to calling function
}

//retrieves a single task from an already open file.
Task getTask(FILE* file)
{
    Task t = {-1,-1}; //initialise Task with "poison pill" values so anything that reads a NULL task knows it to be purposefully invalid
    char* line = malloc(LEN * sizeof(char)); //holds line read in from file
    static int count = 1; //maintains a count of which line the program is up to for Warnings.

    if(feof(file) == 0) //if not at EOF
    {
        if(fgets(line,LEN,file) != NULL) //if next line read is not NULL or EOF
        {
            t = split(line); //split line
            t.aTime = (char*)malloc(9 * sizeof(char)); //allocate memory for valid task
            count++;
        }
        else
        {
            fprintf(stderr,"###Warning### Invalid/EOF line at %d\n",count);
        }
    }
    else
    {
        t.id = -2; //poison pill values for EOF
        t.burst = -2;
        //t.aTime = (char*)malloc(9 * sizeof(char));
    }
    
    free(line); //line has been copied elsewhere and may now be free'd

    return t; //return read in task valid or not.
}

//splits a line on spaces and returns a Task.
static Task split(char* line)
{
    char str1[LEN], str2[LEN], str3[LEN];
    int numRead;

    Task t;

    numRead = sscanf(line, "%s %s %s\n",str1,str2,str3); //checks for correct number and format of line
    if(numRead == 2)
    {
        t.id = atoi(str1); //convert id string to integer
        t.burst = atoi(str2); //converts burst string to integer
    }
    else
    {
        perror("###Error### Invalid parameter");
    }

    return t; 
}

//function that returns the number of lines in a file
int fileLines(char* fName)
{
    int count = 0;
    char temp[LEN]; //temp string to hold line and "validate"
    FILE* f = fopen(fName,"r");

    if(f != NULL)//if file exists
    {
        while(fgets(temp,LEN,f) != NULL) //if line not empty or EOF
        {
            count++;
        }
    }

    fclose(f);

    return count;
}

//closes an already open file and checks for error.
void closeFile(FILE* file)
{
    if(fclose(file) != 0)
    {
        perror("###Error### Could not close file");
    }
}
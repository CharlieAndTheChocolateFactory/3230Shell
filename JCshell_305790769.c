/*
    Name: Wong Chor Sing
    UID:3035790769
    develpoment platform : VScode, workbench2 and docker
    remark: As for as I know, I have completed all the requirments in the specification

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define TRUE 1
#define FALSE 0

#define MAX_SIZE 1024
#define MAX_COMMANDS_string 30
#define MAX_COMMANDS 5

char *signal_des[] = {  //all the signals message
    "NULL",                        // 0 - NULL, not used
    "Hangup",                      // 1
    "Interrupt",                   // 2
    "Quit",                        // 3
    "Illegal instruction",         // 4
    "Trace trap",                  // 5
    "Abort",                       // 6
    "Bus error",                   // 7
    "Floating point exception",    // 8
    "Killed",                      // 9
    "User defined signal 1",       // 10
    "Segmentation fault",          // 11
    "User defined signal 2",       // 12
    "Broken pipe",                 // 13
    "Alarm clock",                 // 14
    "Software termination signal", // 15
    "Stack fault",                 // 16
    "Child status has changed",    // 17
    "Continue",                    // 18
    "Stop",                        // 19
    "Keyboard stop",               // 20
    "Background read from tty",    // 21
    "Background write to tty",     // 22
    "Urgent condition on socket",  // 23
    "Cpu time limit exceeded",     // 24
    "File size limit exceeded",    // 25
    "Virtual alarm clock",         // 26
    "Profiling alarm clock",       // 27
    "Window size change",          // 28
    "I/O now possible",            // 29
    "Power failure",               // 30
    "Bad system call",             // 31
};

struct processStat //use a struct to store all the process stat for printing. 
{
    pid_t PID;
    char CMD[50];
    char STATE;
    int EXCODE;
    char *EXSIG;
    int PPID;
    double USER;
    double SYS;
    int VCTX;
    int NVCTX;
};

struct processStat processStatReadyToPrintHaha[MAX_COMMANDS]; // this array is used for print the process stat.

FILE *file;
FILE *file2;

unsigned long h, ut, st;
unsigned long long i, x;
char comm[50]; // command name
char str[50];
char statusStr[50];

int foo_int; // placeholder for a number(4byte) we don't care in a fomrat string

char *commandArg[MAX_COMMANDS][MAX_COMMANDS_string + 1]; //2D array used for storing the command and the argument.

int numCommand = 0;
int inputFlag = 0;
int childProcessWaitFlag = 0;
int pipes[MAX_COMMANDS - 1][2];

void printStatus(struct processStat processStat) //function used for print the process stat.
{
    if (processStat.EXCODE == 47) 
    {
        return;
    }
    printf("(PID)%d ", processStat.PID);
    printf("(CMD)%s ", processStat.CMD);
    printf("(STATE)%c ", processStat.STATE);

    if (processStat.EXSIG == NULL)     // condditional used to deal with whether the exis is caued by signal or the 'exit' input
    {
        printf("(EXCODE)%d ", processStat.EXCODE);
    }
    else
    {
        printf("(EXSIG)%s ", processStat.EXSIG);
    }

    printf("(PPID)%d ", processStat.PPID);
    printf("(USER)%.2f ", processStat.USER);
    printf("(SYS)%.2f ", processStat.SYS);
    printf("(VCTX)%d ", processStat.VCTX);
    printf("(NVCTX)%d\n", processStat.NVCTX);
}

void SIGUSR1_handler(int signum)
{
    childProcessWaitFlag = 1;
}

void interrupt_signal_handler(int signum)  // retriger the user input line if an interrupt occur.
{ // what is the default behavoir of signal handler.
    if (inputFlag == 1)
    {
        printf("\n## JCSHELL [%d] ## ", getpid());
        fflush(stdout); // make sure the printing starts promptly
    }
}

void command_arg_separation(char *command[], int commandSize) //tokenized the command and the arugment.
{
    // Implement command argument separation logic here
    int size = commandSize;

    for (int i = 0; i < size; i++)
    {
        int count = 0;

        char *token = strtok(command[i], " ");  //tokenize the command and store them into and array.
        while (token != NULL && count < 31)
        {
            commandArg[i][count] = token;
            count++;
            token = strtok(NULL, " ");
        }
        commandArg[i][count] = NULL;
    }
}
int command_string(char *command[]) //separate the command but '|'
{
    int i = 0;
    char *tk = strtok(command[0], "|");
    while (tk != NULL && i < MAX_COMMANDS)
    {
        command[i] = strdup(tk); // store the strings
        i++;
        tk = strtok(NULL, "|");
    }
    command[i] = NULL;
    numCommand = i;
    return i;
}

pid_t command_execution(char *command[], int commandIndex) // This function is used for excuting the command, including the creatiion of child process.
{
    pid_t pid = fork();
    int status;

    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    { // child process
        if (commandIndex > 0) //pipes are handled here.
        {
            dup2(pipes[commandIndex - 1][0], STDIN_FILENO);
        }
        if (commandIndex < numCommand - 1)
        {
            dup2(pipes[commandIndex][1], STDOUT_FILENO);
        }
        for (int i = 0; i < 4; i++)
        {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        while (childProcessWaitFlag == 0)
        {
            pause();
        };
        char errorMessage[20 + strlen(command[0])];
        sprintf(errorMessage, "JCshell: '%s'", command[0]);
        execvp(command[0], command);
        perror(errorMessage);
        exit(47);
    }

    return pid;
}

int input_error_check(char *input) //function to handle illegal input of '|'
{
    if (input[0] == '|' || input[strlen(input) - 2] == '|') //handle the case when the first or the last character are '|'
    {
        return 2;
    }
    else if (strstr(input, "||") != NULL) // handle the '||' case
    {
        return 1;
    }
    for (int i = 0; i < strlen(input); i++) //handle the case when there are a lot of spaces between the '|' , e.g. '|            |'
    {
        if (input[i] == '|')
        {
            for (int j = i + 1; j < strlen(input); j++)
            {
                if (input[j] == ' ')
                {
                    continue;
                }
                else if (input[j] == '|')
                {
                    return 1;
                }
                else
                {
                    break;
                }
            }
        }
    }
    return 0;
}

int exitCommandErrorHandling() //function to handle three "exit" error cases.
{

    if (numCommand == 1)
    {
        if (strcmp(commandArg[0][0], "exit") == 0)
        { // first word of the first command is exit
            if (commandArg[0][1] == NULL)
            {
                printf("JCshell: termianted\n");   //exit >>> exit & JCshell: Terminated
                exit(0);
            }
            else
            {
                printf("JCshell: \"exit\" with other arguments!!!\n"); //exit no >>>> JCshell: "exit" with other arguments!!!
                return -1;
            }
        }
    }
    return 0;
}




int main()
{
    signal(SIGUSR1, SIGUSR1_handler); //
    signal(SIGINT, interrupt_signal_handler);
    while (1)
    {
        char input[MAX_SIZE + 1];
        char *command[MAX_COMMANDS];

        childProcessWaitFlag = 0;

        printf("## JCSHELL [%d] ## ", getpid()); //the input line for user with the current process ID.
        inputFlag = 1;
        fgets(input, sizeof(input), stdin); // how to deal with the edge case of interrupt comes while having input.
        inputFlag = 0;


        int outputOfErrorCheck = input_error_check(input); //use a variable to store a flag

        if (outputOfErrorCheck == 1) //if the flag = 1, it is the case where  || and |    | case
        {
            printf("JCshell: should not have two | symbols without in-between command\n");
            continue;
        } 

        if (outputOfErrorCheck == 2) // if the flag =2, the error message is for '|' when it is at the first and at last.
        {
            printf("JCshell: Incomplete '|' sequence\n");
            continue;
        } 


        size_t length = strlen(input);  // remove new line character
        if (length > 0 && input[length - 1] == '\n')
        {
            input[length - 1] = '\0';
        }

        command[0] = input;

        int commandSize = command_string(command); // separate the command with '|'
        command_arg_separation(command, commandSize); // tokenize the command and the arugment. 


        if (exitCommandErrorHandling() == -1) // if there is and error in the command, go back to the beginnering of the main function and force the user to input again.
        {
            continue;
        };


        if (pipe(pipes[0]) == -1 || pipe(pipes[1]) == -1 || pipe(pipes[2]) == -1 || pipe(pipes[3]) == -1) //create the pipes.
        {
            perror("pipe create failure\n");
            exit(1);
        }

        int pid_array[commandSize]; // use loop to create process and handle signal
        for (int i = 0; i < commandSize; i++)
        {
            pid_array[i] = command_execution(commandArg[i], i);
        }
        for (int i = 0; i < 4; i++) //close the pipe properly
        {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        for (int i = 0; i < commandSize; i++)
        { // process has to done concurrently therefore, the kill and waitpid has to seperate
            kill(pid_array[i], SIGUSR1);
        }

        for (int i = 0; i < commandSize; i++)
        {
            // wait zombie
            struct processStat processStatReadyToPrint = {0};

            siginfo_t info; //get the process statistics from the file.
            int status;
            int ret = waitid(P_ALL, 0, &info, WNOWAIT | WEXITED);
            if (!ret)
            {

                processStatReadyToPrint.PID = info.si_pid;

                sprintf(str, "/proc/%d/stat", info.si_pid);
                file = fopen(str, "r");
                if (file == NULL)
                {
                    printf("Error in open my proc file\n");
                    exit(0);
                }
                // read first 15 fields
                int test;
                char foo_str[50];
                test = fscanf(file, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu",
                              &foo_int,
                              foo_str,
                              &processStatReadyToPrint.STATE,
                              &processStatReadyToPrint.PPID,
                              &foo_int,
                              &foo_int,
                              &foo_int,
                              &foo_int,
                              (unsigned *)&foo_int,
                              &h,
                              &h,
                              &h,
                              &h,
                              &ut,
                              &st);

                fclose(file);


                processStatReadyToPrint.USER = ut * 1.0f / sysconf(_SC_CLK_TCK); //convert the clock cycle to seconds
                processStatReadyToPrint.SYS = st * 1.0f / sysconf(_SC_CLK_TCK);


                sprintf(statusStr, "/proc/%d/status", info.si_pid); // open file for the remaining process stat.
                file2 = fopen(statusStr, "r");
                if (file2 == NULL)
                {
                    printf("Error in open my proc file\n");
                    exit(0);
                }

                char line[150];
                while (fgets(line, sizeof(line), file2) != NULL)
                {
                    if (strncmp(line, "Name:", strlen("Name:")) == 0)
                    {
                        sscanf(line, "%*s %s", processStatReadyToPrint.CMD);
                    }
                    if (strncmp(line, "voluntary_ctxt_switches:", strlen("voluntary_ctxt_switches:")) == 0)
                    {
                        sscanf(line, "%*s %d", &processStatReadyToPrint.VCTX);
                    }
                    if (strncmp(line, "nonvoluntary_ctxt_switches:", strlen("nonvoluntary_ctxt_switches:")) == 0)
                    {
                        sscanf(line, "%*s %d", &processStatReadyToPrint.NVCTX);
                    }
                }
                fclose(file2);

                waitpid(info.si_pid, &status, 0); 
                if (WIFEXITED(status)) //use exit status to distinguis weather exit is caused by signal or exit command.
                {
                    processStatReadyToPrint.EXCODE = WEXITSTATUS(status);
                }
                if (WIFSIGNALED(status))
                { // check if is killed
                    processStatReadyToPrint.EXSIG = signal_des[WTERMSIG(status)];
                }

                processStatReadyToPrintHaha[i] = processStatReadyToPrint;
            }
            else
            {
                perror("waitid");
            }


        }

        for (int i = 0; i < commandSize; i++) //print the process stats.
        {
            printStatus(processStatReadyToPrintHaha[i]);
        }

   
    }
}
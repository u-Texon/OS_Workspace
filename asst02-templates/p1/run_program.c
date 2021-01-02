#include "run_program.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define ERROR_CODE 127

int run_program(char *file_path, char *arguments[])
{
    pid_t pid;
    int status;
    
    switch ((pid = fork()))
    {
    case -1: //error
        return ERROR_CODE;
        break;

    case 0: //child
        execl(file_path, *arguments, NULL);
        break;

    default: //parent
        wait(&status);
        return WEXITSTATUS(status);
        break;
    }

    // remember to return ERROR_CODE on error.
    return ERROR_CODE;
}

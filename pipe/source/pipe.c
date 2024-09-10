#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "pipe.h"
#include "parse.h"

static int runCmd (Text* cmds, size_t curCmdIndex);

#define RETURN_ERROR_OR_CONTINUE(expression, errorMessage, retValue, ...) \
{                                                                         \
    if (expression)                                                       \
    {                                                                     \
        perror (errorMessage);                                            \
                                                                          \
        __VA_ARGS__                                                       \
                                                                          \
        return retValue;                                                  \
    }                                                                     \
}                                                                         \

int Pipeline (Text* cmds, int inputFd, size_t iCmd)
{
    printf("hui");

    assert (cmds);

    printf ("iCmd: %d\n", iCmd);

    if (iCmd != cmds->cmdsCount - 1)
    {
        int fds[2] = {};

        RETURN_ERROR_OR_CONTINUE(pipe (fds) == -1,
                                 "Error: unable to create pipe", -1);

        int readDescriptor  = fds[0];
        int writeDescriptor = fds[1];

        pid_t writePid = 0;

        RETURN_ERROR_OR_CONTINUE((writePid = fork ()) == -1,
                                 "Error: unable to fork", -1);

        if (writePid == 0)
        {
            RETURN_ERROR_OR_CONTINUE(dup2(writeDescriptor, STDOUT_FILENO) == -1,
                                     "Error: unable to dup", -1);

            if (inputFd != STDIN_FILENO)
            {
                RETURN_ERROR_OR_CONTINUE(dup2(inputFd, STDIN_FILENO) == -1,
                                         "Error: unable to dup", -1);
            }

            RETURN_ERROR_OR_CONTINUE(close (fds[0]) == -1,
                                     "Error: unable to close fd", -1);
            RETURN_ERROR_OR_CONTINUE(close (fds[1]) == -1,
                                     "Error: unable to close fd", -1);

            runCmd (cmds, iCmd);
        }
        else
        {
            pid_t readPid = 0;
            RETURN_ERROR_OR_CONTINUE((writePid = fork ()) == -1,
                                     "Error: unable to fork", -1);

            if (readPid == 0)
            {
                RETURN_ERROR_OR_CONTINUE(close (fds[1]) == -1,
                                         "Error: unable to close fd", -1);

                Pipeline (cmds, readDescriptor, iCmd + 1);
            }
            else
            {
                RETURN_ERROR_OR_CONTINUE(close (fds[0]) == -1,
                                         "Error: unable to close fd", -1);
                RETURN_ERROR_OR_CONTINUE(close (fds[1]) == -1,
                                         "Error: unable to close fd", -1);

                int status = 0;

                waitpid (writePid, &status, 0);
                            printf("%d status: %d\n", iCmd, WEXITSTATUS(status));

                waitpid (readPid,  &status, 0);
                            printf("%d status: %d\n", iCmd, WEXITSTATUS(status));

            }
        }
    }
    else
    {
        pid_t pid = 0;
        RETURN_ERROR_OR_CONTINUE((pid = fork ()) == -1,
                                 "Error: unable to fork", -1);

        if (pid == 0)
        {
            RETURN_ERROR_OR_CONTINUE(dup2(inputFd, STDIN_FILENO),
                                     "Error: unable to dup", -1);
            RETURN_ERROR_OR_CONTINUE(close(inputFd) == -1,
                                     "Error: unable to close fd", -1)

            runCmd (cmds, iCmd);
        }

        int status = 0;
        waitpid (pid, &status, 0);
        printf("%d status: %d\n", iCmd, WEXITSTATUS(status));

    }

    return 0;
}

static int runCmd (Text* cmds, size_t curCmdIndex)
{
    assert (cmds);

    char** argv = cmds->cmds[curCmdIndex].argv;
    size_t argc = cmds->cmds[curCmdIndex].argc;

    for (size_t iArg = 0; iArg < argc; iArg++)
    {
        printf ("%s ", argv[iArg]);
    }

    int err = execvp (argv[0], argv);

    RETURN_ERROR_OR_CONTINUE(err == -1, "Error, couldn't exec", -1);

    return 0;
}

#undef RETURN_ERROR_OR_CONTINUE


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#include "pipe.h"
#include "parse.h"

static int runCmd (Text* cmds);

#define RETURN_ERROR_OR_CONTINUE(expression, errorMessage, retValue, ...) \
{                                                                         \
    if (expression)                                                       \
    {                                                                     \
        perror (errorMessage);                                            \
                                                                          \
        __VA_ARGS__                                                       \
                                                                          \
    }                                                                     \
}                                                                         \

int Pipeline (Text* cmds, int inputFd)
{
    assert (cmds);

    fprintf (stderr, "iCmd: %d\n", cmds->iCmd);

    if (cmds->iCmd != cmds->cmdsCount - 1)
    {
        int fds[2] = {};

        pipe (fds);

        int readDescriptor  = fds[0];
        int writeDescriptor = fds[1];

        pid_t writePid = 0;
              writePid = fork();

        fprintf (stderr, "%d: %d\n", getpid(), writePid);

        if (writePid == 0)
        {
            fprintf (stderr, "hello, %d: %d\n", getpid(), writePid);

            dup2 (writeDescriptor, STDOUT_FILENO);

            if (inputFd != STDIN_FILENO)
            {
                dup2 (inputFd, STDIN_FILENO);
            }

            close (fds[0]);
            close (fds[1]);

            fprintf (stderr, "bye bye, %d: %d\n", getpid(), writePid);

            runCmd (cmds);
        }
        else
        {
            pid_t readPid = fork ();

            if (readPid == 0)
            {
                close (fds[1]);

                cmds->iCmd++;
                Pipeline (cmds, readDescriptor);
            }
            else
            {
                close (fds[0]);
                close (fds[1]);

                int status = 0;

                // waitpid (writePid, &status, 0);
                fprintf (stderr, "%d status: %d\n", getpid(), WEXITSTATUS(status));

                // waitpid (readPid, &status, 0);
                fprintf (stderr, "%d status: %d\n", getpid(), WEXITSTATUS(status));
            }
        }
    }
    else
    {
        pid_t pid = fork ();

        if (pid == 0)
        {
            fprintf (stderr, "fd: %d, %d\n", inputFd, getpid());

            if (inputFd != STDIN_FILENO)
            {
                dup2 (inputFd, STDIN_FILENO);
            }

            close (inputFd);

            runCmd (cmds);
        }
        else
        {
            int status = 0;

            waitpid(pid, &status, 0);
            fprintf(stderr, "%d status: %d\n", getpid(), WEXITSTATUS(status));
        }
    }

    return 0;
}


static int runCmd (Text* cmds)
{
    fprintf (stderr, "!!!\n");

    assert (cmds);

    char** argv = cmds->cmds[cmds->iCmd].argv;
    size_t argc = cmds->cmds[cmds->iCmd].argc;

    for (size_t iArg = 0; iArg < argc; iArg++)
    {
        printf ("%s ", argv[iArg]);
    }

    // fflush (stdout);

    int err = execvp (argv[0], argv);

    return 0;
}

#undef RETURN_ERROR_OR_CONTINUE


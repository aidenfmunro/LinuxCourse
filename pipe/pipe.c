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

int Pipeline (Text* cmds)
{
    assert (cmds);

    static int iCmd = 0;
    int fds[2] = {};

    if (iCmd == cmds->cmdsCount - 1)
    {
        printf ("%d\n", getpid());
        printf ("%zu\n", iCmd);

        // RETURN_ERROR_OR_CONTINUE(dup2 (fds[1], STDOUT_FILENO) == -1, "Error: unable to close file", -1);

        runCmd (cmds, iCmd);

        return 0;
    }

    pid_t pid = 0;

    int err = pipe (fds);
    RETURN_ERROR_OR_CONTINUE(err == -1,
                             "Error: unable to pipe", -1);

    pid = fork ();
    RETURN_ERROR_OR_CONTINUE(pid == -1,
                             "Error: unable to fork", -1);

    if (pid == 0) // child
    {
        printf ("child pid: %d\n", getpid());

        RETURN_ERROR_OR_CONTINUE(close (fds[1]) == -1,
                                 "Error: unable to close file", -1);

        RETURN_ERROR_OR_CONTINUE(dup2 (fds[0], STDIN_FILENO) == -1,
                                 "Error: unbale to dup", -1);

        iCmd++;

        close (fds[0]);

        Pipeline (cmds);
    }
    else
    {
        printf ("parent pid: %d\n", getpid());

        RETURN_ERROR_OR_CONTINUE(close (fds[0])   == -1,
                                 "Error: unable to close file", -1);

        if (iCmd != 0)
        {
            RETURN_ERROR_OR_CONTINUE(dup2 (fds[1], STDOUT_FILENO) == -1,
                                     "Error: unable to close file", -1);
        }

        printf ("%d\n", iCmd);

        runCmd (cmds, iCmd);
    }

    int status = -1;
	waitpid(pid, &status, 0);

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

    printf ("\n");

    int err = execvp (argv[0], argv);

    RETURN_ERROR_OR_CONTINUE(err == -1, "Error, couldn't exec", -1);

    return 0;
}

#undef RETURN_ERROR_OR_CONTINUE


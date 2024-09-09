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

    static int iCmd = -1;
    int fds[2] = {};

    int err = pipe (fds);

    RETURN_ERROR_OR_CONTINUE(err == -1, "Error: unable to pipe", errno);

    if (iCmd == cmds->cmdsCount)
    {
        return 0;
    }

    pid_t pid = fork ();
    RETURN_ERROR_OR_CONTINUE(pid == -1, "Error: unable to fork", -1);
    // printf ("cur pid: %d\n", pid);
    // printf ("cur cmd num: %zu\n", curCmdIndex);
    if (pid == 0)
    {
        printf ("cur cmd num: %d\n", iCmd);

        RETURN_ERROR_OR_CONTINUE(close (fds[1])   == -1, "Error: unable to close file",          -1);
        RETURN_ERROR_OR_CONTINUE(dup2 (fds[0], 0) == -1, "Error: unable to dup read descritpor", -1);
        RETURN_ERROR_OR_CONTINUE(pipe (fds)       == -1, "Error: unable to pipe",                -1);

        iCmd++;
        Pipeline (cmds);
    }
    else
    {
        RETURN_ERROR_OR_CONTINUE(close (fds[0])   == -1, "Error: unable to close file",          -1);
        RETURN_ERROR_OR_CONTINUE(dup2 (fds[1], 1)   == -1, "Error: unable to dup file",          -1);

        runCmd (cmds, iCmd);
    }

    int status = -1;
	waitpid(pid, &status, 0);
	// printf("Ret code: %d\n", WEXITSTATUS(status));

    return 0;
}

static int runCmd (Text* cmds, size_t curCmdIndex)
{
    assert (cmds);

    char** argv = cmds->cmds[curCmdIndex].argv;

    int err = execvp (argv[0], argv);

    RETURN_ERROR_OR_CONTINUE(err == -1, "Error, couldn't exec", errno);

    return 0;
}

#undef RETURN_ERROR_OR_CONTINUE


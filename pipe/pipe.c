#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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

    char fds[2] = {};

    int err = pipe (fds)

    RETURN_ERROR_OR_CONTINUE(err == -1, "Error, unable to pipe", errno);

    size_t curCmdIndex = 0;

    pid_t pid = fork ();

    RETURN_ERROR_OR_CONTINUE(pid == -1, "Error, unable to fork", errno);

    if (pid == 0)
    {
        close (fds[1]);
        dup2 (fds[0], 0);
        pipe (fds);

        fork();

        close (fds[0]);
        dup2 (fds[1], 1);

        runCmd()
    }
    else
    {
        close (fds[0]);
    }

}

static int runCmd (Text* cmds, size_t curCmdIndex)
{
    assert (cmds);

    char* argv[] = cmds->cmds[curCmdIndex].argv;

    int err = execvp(argv[0], argv);

    RETURN_ERROR_OR_CONTINUE(err = -1, "Error, couldn't exec", errno);

    return 0;
}

#undef RETURN_ERROR_OR_CONTINUE


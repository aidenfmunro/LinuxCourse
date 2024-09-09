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

    int* fd = (int*) calloc ((cmds->cmdsCount - 1) * 2, sizeof(int));

    RETURN_ERROR_OR_CONTINUE(fd == NULL, "Error, unable to allocate memory", -1);

    for (size_t iCmd = 0; iCmd < cmds->cmdsCount - 1; iCmd++)
    {
        RETURN_ERROR_OR_CONTINUE(pipe(fd + 2 * iCmd) < 0, "Error: can't create pipe", -1);
    }

    pid_t pid  = 0;
    int status = 0;

    for (size_t iCmd = 0; iCmd < cmds->cmdsCount; iCmd++)
    {
        RETURN_ERROR_OR_CONTINUE((pid = fork()) < 0, "Error: unable to fork", -1);

        if (pid == 0)
        {
            if (iCmd != 0)
            {
                RETURN_ERROR_OR_CONTINUE(dup2(fd[2 * iCmd - 2], STDIN_FILENO) < 0, "Error: unable to dup", -1);
            }

            if (iCmd != cmds->cmdsCount - 1)
            {
                RETURN_ERROR_OR_CONTINUE(dup2(fd[2 * iCmd + 1], STDOUT_FILENO) < 0, "Error: unable to dup", -1);
            }

            for (size_t iFd = 0; iFd < (cmds->cmdsCount - 1) * 2; iFd++)
            {
                RETURN_ERROR_OR_CONTINUE(close (fd[iFd]) < 0, "Error: unable to dup", -1);
            }

            runCmd(cmds, iCmd);
        }

    }

    pid_t id = wait(&status);

    free(fd);

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


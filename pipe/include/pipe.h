#ifndef PIPE_H
#define PIPE_H

#include "parse.h"

int Pipeline (Text* cmds, int inputFd, size_t iCmd);

#endif // PIPE_H

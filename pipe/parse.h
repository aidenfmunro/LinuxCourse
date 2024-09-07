#ifndef PARSE_H
#define PARSE_H

#include <stddef.h>

struct Command
{
    char*  argv[];
    size_t argc;
};

struct Text
{
    char*  buffer;
    size_t bufferSize;

    struct Command* cmds;
    size_t          cmdsCount;
};





#endif

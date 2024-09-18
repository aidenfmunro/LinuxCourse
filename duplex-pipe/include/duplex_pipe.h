#ifndef DUPLEX_PIPE_H
#define DUPLEX_PIPE_H

#include <stddef.h>

typedef size_t (*receive_t)(Pipe *self);
typedef size_t (*send_t)   (Pipe *self);

typedef struct
{
    receive_t receive;
    send_t    send;

} OpTable;

typedef struct
{
    char*  data;
    size_t dataLength;

    int fdForw[2];
    int fdBack[2];

    OpTable ops;

} Pipe;

#endif DUPLEX_PIPE_H

#include <stddef.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>


#include "parse.h"

size_t getSize (int fd)
{
    struct stat info = {};

    if (fstat(fd, &info) == -1)
    {
        return 0;
    }

    return (size_t) info.st_size;
}

int CreateText (int fd, struct Text* inputText)
{
    assert (inputText);

    intputText->buffer = parseBuffer (fd, inputText);

}

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

char* parseBuffer (int fd, struct Text* inputText)
{
    assert (inputText);

    RETURN_ERROR_OR_CONTINUE(lseek(fd, 0, SEEK_SET) == -1,
                             "Error: requested file position beyond the end of the file", NULL
                            );

    inputText->bufferSize = getSize (fd);

    RETURN_ERROR_OR_CONTINUE(inputText->bufferSize == 0,
                             "Error: file size is zero.\n", NULL
                            );

    char* buffer = (char*) calloc (inputText->bufferSize + 1, sizeof(char));

    RETURN_ERROR_OR_CONTINUE(buffer == NULL,
                             "Error: unable to allocate buffer memory.\n", NULL
                            );

    ssize_t numBytesRead = read (fd, buffer, inputText->bufferSize);

    RETURN_ERROR_OR_CONTINUE(numBytesRead == -1,
                             "Error: unable to read from file.\n", NULL,
                             free (buffer);
                            );

    RETURN_ERROR_OR_CONTINUE(numBytesRead != inputText->bufferSize,
                             "Error: number of bytes read isn't correct", NULL,
                             free (buffer;
                            );


    intputText->cmdsCount = countTokens (buffer, '|');

    return buffer;
}

size_t countTokens (char* buffer, char sepSymbol)
{
    assert (buffer);

    char* begin = buffer;
    char* end   = buffer;

    size_t count = 0;

    while ((end = strchr (begin, sepSymbol) != NULL)
    {
        begin = end + 1;

        count++;
    }


}






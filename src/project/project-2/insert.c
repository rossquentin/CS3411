#include "ufsext.h"
#include <unistd.h>
#include <malloc.h>

int insert(int fd, void *buf, size_t bytes, size_t offset) {
    size_t length = lseek(fd, 0, SEEK_END);
    // Test bounds of offset, bytes, and lseek error.
    int retVal;
    if (length < 0) return (int) length;
    if (offset > length || offset < 0 || bytes < 0) return -1;

    // Get items past offset.
    size_t lengthPastOffset = length - offset;
    char *itemsPastOffset = malloc(sizeof(char) * lengthPastOffset);
    if (itemsPastOffset == NULL) return -2;

    lseek(fd, offset, SEEK_SET);
    retVal = read(fd, itemsPastOffset, lengthPastOffset);
    if (retVal < 0) return retVal;

//    // Remove all items at end.
//    if (ftruncate64(fd, offset) < 0) return -1;

    // Write new content.
    lseek(fd, offset, SEEK_SET);
    retVal = write(fd, buf, bytes);
    if (retVal < 0) return retVal;

    // Write old content.
    retVal = write(fd, itemsPastOffset, lengthPastOffset);
    if (retVal < 0) return retVal;

    free(itemsPastOffset);

    return (int) (lseek(fd, 0, SEEK_END) - length);
}
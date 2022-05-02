#include "ufsext.h"
#include <unistd.h>
#include <malloc.h>

int extract(int fd, void *buf, size_t bytes, size_t offset) {
    size_t length = lseek(fd, 0, SEEK_END);
    int retVal;
    // Test bounds of offset and lseek error.
    if (length < 0) return (int) length;
    if (offset > length || offset < 0 || bytes < 0 || bytes > length) return -1;

    // Get items past offset + bytes.
    size_t lengthPastOffset = length - (offset + bytes);
    char *itemsPastOffset = malloc(sizeof(char) * lengthPastOffset);
    if (itemsPastOffset == NULL) return -2;
    lseek(fd, (offset + bytes), SEEK_SET);
    retVal = read(fd, itemsPastOffset, lengthPastOffset);
    if (retVal < 0) return retVal;

    // Get items for extraction
    lseek(fd, offset, SEEK_SET);
    retVal = read(fd, buf, bytes);
    if (retVal < 0) return retVal;

    // Remove items past offset.
    retVal = ftruncate(fd, offset);
    if (retVal < 0) return retVal;

    // Write old content.
    lseek(fd, 0, SEEK_END);
    retVal = write(fd, itemsPastOffset, lengthPastOffset);
    if (retVal < 0) return retVal;

    free(itemsPastOffset);

    return (int) (length - lseek(fd, 0, SEEK_END));
}
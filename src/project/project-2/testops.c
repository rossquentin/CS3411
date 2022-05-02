#include "ufsext.h"
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>


static int fd;

void resetFD() {
    ftruncate(fd, 0);
    write(fd, "abcde", 5);
    write(1, "\n", 1);
}

void testInsertValid() {
    int retVal = insert(fd, "123", 3, 3);
    if (retVal < 0) write(1, "INSERT TEST 1: FAIL\n", 20);
    else write(1, "INSERT TEST 1: PASS\n", 20);   // Returned success, expected return.
}

void testInsertInvalidFD() {
    int retVal = insert(fd + 1, "123", 3, 3);
    if (retVal < 0) write(1, "INSERT TEST 2: PASS\n", 20);    // Returned exception, expected return.
    else write(1, "INSERT TEST 2: FAIL\n", 20);
}

void testInsertInvalidOffset() {
    int retVal = insert(fd, "123", 3, -1);
    if (retVal < 0) {   // Returned exception, expected return.
        retVal = insert(fd, "123", 3, 200);
        if (retVal < 0) write(1, "INSERT TEST 3: PASS\n", 20);    // Returned exception, expected return.
    } else write(1, "INSERT TEST 3: FAIL\n", 20);
}

void testInsertInvalidBytes() {
    int retVal = insert(fd + 1, "123", -1, 3);
    if (retVal < 0) write(1, "INSERT TEST 4: PASS\n", 20);    // Returned exception, expected return.
    else write(1, "INSERT TEST 4: FAIL\n", 20);
}

void testDeleteValid() {
    int retVal = delete(fd, 2, 2);
    if (retVal < 0) write(1, "DELETE TEST 1: FAIL\n", 20);
    else write(1, "DELETE TEST 1: PASS\n", 20);   // Returned success, expected return.
}

void testDeleteInvalidFD() {
    int retVal = delete(fd + 1, 2, 2);
    if (retVal < 0) write(1, "DELETE TEST 2: PASS\n", 20);    // Returned exception, expected return.
    else write(1, "DELETE TEST 2: FAIL\n", 20);
}

void testDeleteInvalidOffset() {
    int retVal = delete(fd, 2, 20);
    if (retVal < 0) {   // Returned exception, expected return.
        retVal = delete(fd, 2, -1);
        if (retVal < 0) write(1, "DELETE TEST 3: PASS\n", 20);    // Returned exception, expected return.
    } else write(1, "DELETE TEST 3: FAIL\n", 20);
}

void testDeleteInvalidBytes() {
    int retVal = delete(fd, 20, 2);
    if (retVal < 0) {   // Returned exception, expected return.
        retVal = delete(fd, -1, 2);
        if (retVal < 0) write(1, "DELETE TEST 4: PASS\n", 20);
    } else write(1, "DELETE TEST 4: FAIL\n", 20);
}

void testExtractValid() {
    char *tempBuf = malloc(sizeof(char) * 2);

    int retVal = extract(fd, tempBuf, 2, 2);
    if (retVal < 0) write(1, "EXTRACT TEST 1: FAIL\n", 21);
    else write(1, "EXTRACT TEST 1: PASS\n", 21);    // Returned success, expected return.

    free(tempBuf);
}

void testExtractInvalidFD() {
    char *tempBuf = malloc(sizeof(char) * 2);

    int retVal = extract(fd + 1, tempBuf, 2, 2);
    if (retVal < 0) write(1, "EXTRACT TEST 2: PASS\n", 21);    // Returned exception, expected return.
    else write(1, "EXTRACT TEST 2: FAIL\n", 21);

    free(tempBuf);
}

void testExtractInvalidOffset() {
    char *tempBuf = malloc(sizeof(char) * 2);

    int retVal = extract(fd, tempBuf, 2, -1);
    if (retVal < 0) {   // Returned exception, expected return.
        retVal = extract(fd, tempBuf, 2, 100);
        if (retVal < 0) write(1, "EXTRACT TEST 3: PASS\n", 21);    // Returned exception, expected return.
    } else write(1, "EXTRACT TEST 3: FAIL\n", 21);

    free(tempBuf);
}

void testExtractInvalidBytes() {
    char *tempBuf = malloc(sizeof(char) * 2);

    int retVal = extract(fd, tempBuf, -1, 2);
    if (retVal < 0) {   // Returned exception, expected return.
        retVal = extract(fd, tempBuf, 100, 2);
        if (retVal < 0) write(1, "EXTRACT TEST 4: PASS\n", 21);    // Returned exception, expected return.
    } else write(1, "EXTRACT TEST 4: FAIL\n", 21);

    free(tempBuf);
}

void runInsertTests() {
    testInsertValid();
    testInsertInvalidFD();
    testInsertInvalidOffset();
    testInsertInvalidBytes();
}

void runDeleteTests() {
    testDeleteValid();
    testDeleteInvalidFD();
    testDeleteInvalidOffset();
    testDeleteInvalidBytes();
}

void runExtractTests() {
    testExtractValid();
    testExtractInvalidFD();
    testExtractInvalidOffset();
    testExtractInvalidBytes();
}

void runTests() {
    runInsertTests();
    resetFD();
    runDeleteTests();
    resetFD();
    runExtractTests();
}

int main() {
    fd = open("./test.tmp", O_RDWR | O_CREAT | O_TRUNC);
    write(fd, "abcde", 5);

    runTests();

    close(fd);
    remove("./test.tmp");
}

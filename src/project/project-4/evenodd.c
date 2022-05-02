#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *getNextWord(int fd, int *failptr) {
    int cap = 1024;
    char *word = calloc(cap, sizeof(char));
    int done = 0;
    int j = 0;
    while (!done) {
        if (read(fd, &word[j], 1) == 0 && j == 0) {
            *failptr = 1;
            return 0;
        } else if (word[j] != ' ') {
            j++;
            if (j >= cap) {
                cap *= 2;
                word = realloc(word, cap);
            }
        } else {
            word[j] = '\0';
            done = 1;
        }

    }
    return word;
}

void writeToOutput(int fd, char **words, int *counts, int max) {
    char *wordbuf;
    for (int i = 0; i < max; i++) {
        wordbuf = malloc(strlen(words[i]) + 12);
        sprintf(wordbuf, "(%s,%08x)", words[i], counts[i]);
        write(fd, wordbuf, strlen(wordbuf));
        free(wordbuf);
    }
}

// Returns -1 if no word exists
int findIndexOfWord(char *word, char **words, int max) {
    for (int i = 0; i < max; i++) {
        if (strcmp(word, words[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int main(int argc, char **argv) {
    int wordCap;
    char **words;
    int *counts;
    int fail;
    int max;
    int fdIn;
    int fdOut;
    int sleepTime = 10;
    if (strcmp(&argv[0][strlen(argv[0])-4], "even") == 0) {
        wordCap = 256;
        words = malloc(sizeof(char *) * wordCap);
        counts = malloc(sizeof(int) * wordCap);
        fail = 0;
        max = 0;
        fdIn = 3;
        fdOut = 6;
        while (1) {
            char *word = getNextWord(fdIn, &fail);

            // Check EOF
            if (fail) {
                sleep(sleepTime);
                writeToOutput(fdOut, words, counts, max);
                kill(atoi(argv[1]), SIGTERM);
                close(fdIn);
                close(fdOut);
                // Free everything
                free(counts);
                for (int i = 0; i < max; ++i) {
                    free(words[i]);
                }
                free(word);
                free(words);
                exit(0);
            }
            // Add work to list
            int index = findIndexOfWord(word, words, max);
            if (index == -1) {
                if (max >= wordCap) {
                    wordCap *= 2;
                    words = realloc(words, sizeof(char *) * wordCap);
                    counts = realloc(counts, sizeof(int) * wordCap);
                }
                words[max] = malloc((strlen(word) + 1) * sizeof(char));
                strcpy(words[max], word);
                counts[max] = 1;
                max++;

            } else {
                counts[index]++;
            }
            free(word);
        }
    } else if (strcmp(&argv[0][strlen(argv[0])-3], "odd") == 0) {
        wordCap = 256;
        words = malloc(sizeof(char *) * wordCap);
        counts = malloc(sizeof(int) * wordCap);
        fail = 0;
        max = 0;
        fdIn = 7;
        fdOut = 10;

        while (1) {
            char *word = getNextWord(fdIn, &fail);

            // Check EOF
            if (fail) {
                sleep(sleepTime);
                writeToOutput(fdOut, words, counts, max);
                close(fdIn);
                close(fdOut);
                // Free everything
                free(counts);
                for (int i = 0; i < max; ++i) {
                    free(words[i]);
                }
                free(word);
                free(words);
                exit(0);
            }
            // Add work to list
            int index = findIndexOfWord(word, words, max);
            if (index == -1) {
                if (max >= wordCap) {
                    wordCap *= 2;
                    words = realloc(words, sizeof(char *) * wordCap);
                    counts = realloc(counts, sizeof(int) * wordCap);
                }
                words[max] = malloc((strlen(word) + 1) * sizeof(char));
                strcpy(words[max], word);
                counts[max] = 1;
                max++;

            } else {
                counts[index]++;
            }
            free(word);
        }
    }
}
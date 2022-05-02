#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

void printWordsFromFD(int fd) {
    int cap = 1024;
    while (1) {
        char *count = calloc(9, sizeof(char));
        char *word = calloc(cap, sizeof(char));
        int j = 0;
        // Read next word
        while (1) {
            if (read(fd, &word[j], 1) == 0) {
                free(count);
                free(word);
                return;
            }
            // Beginning of next pair
            else if (word[j] == '(' || word[j] == ')') {
                word[j] = '\0';
            }
            // End of word
            else if (word[j] == ',') {
                word[j] = '\0';
                printf("\n\t%s", word);
                break;
            }
            // Still in word
            else {
                //printf("%c", word[j]);
                j++;
                if (j >= cap) {
                    cap *= 2;
                    word = realloc(word, cap);
                }
            }
        }

        // Read count
        read(fd, count, 8);
        printf("\t%d", (int) strtol(count, NULL, 16));
        // Skip trailing ')'
        lseek(fd, 1, SEEK_CUR);
    }
}

char *removePunctAndConvertLower(char *str) {
    char *src = str;
    char *dest = str;
    int j = 0;
    for (int i = 0; src[i] != '\0'; i++) {
        if (!ispunct(src[i])) {
            dest[j++] = tolower(src[i]);
        }
    }
    dest[j] = '\0';
    return dest;
}

void printPairs() {
    printf("\nWords with even letters:");
    printWordsFromFD(5);
    close(5);
    printf("\nWords with odd letters:");
    printWordsFromFD(9);
    close(9);
    printf("\n");
    fflush(stdout);
    exit(0);
}

int main() {
    // Setup signal printPairs
    struct sigaction act;
    act.sa_handler = printPairs;
    act.sa_flags = SA_NODEFER;
    sigemptyset(&act.sa_mask);
    sigaction(SIGTERM, &act, NULL);

    char *word;
    // Get next word
    while (fscanf(stdin, "%ms", &word) > 0) {
        word = removePunctAndConvertLower(word);
        // Send to Even
        if (strlen(word) % 2 == 0) {
            write(4, word, strlen(word));
            write(4, " ", 1);
        }
            // Send to Odd
        else {
            word = realloc(word, sizeof(char) * (strlen(word) + 2));
            strcat(word, " ");
            write(8, word, strlen(word));
        }
        free(word);
    }

    // Close all output pipes
    close(4);
    close(8);
    while (1) {
        sleep(1);
        printf("*");
    }
}
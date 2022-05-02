#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: driver <file>\n");
        return 1;
    }

    int scan2even[2]; // 3,4
    int even2scan[2]; // 5,6
    int scan2odd[2];  // 7,8
    int odd2scan[2];  // 9,10
    int fd;
    int pids[3];
    for (int i = 0; i < 3; i++) {
        pids[i] = 0;
    }

    pipe(scan2even);
    pipe(even2scan);
    pipe(scan2odd);
    pipe(odd2scan);

    // Scanner
    pids[0] = fork();
    if (pids[0] < 0) {
        perror("Didn't fork scanner.\n");
        exit(1);
    }
    if (pids[0] == 0) {
        close(scan2even[0]);
        close(even2scan[1]);
        close(scan2odd[0]);
        close(odd2scan[1]);


        fd = open(argv[1], O_RDONLY);
        if (fd <= 0) exit(1);
        close(0);
        dup(fd);
        close(fd);

        // Scanner will write on 4 (even) and 8 (odd), and will read on 5 (even) and 9 (odd)
        execlp("./scanner", "./scanner", (char *) 0);
        // Did not go into exec, return error.
        exit(1);

    }

    // Even
    pids[1] = fork();
    if (pids[1] < 0) {
        perror("Didn't fork even.\n");
        exit(1);
    }
    if (pids[1] == 0) {
        // Close all odd pipes
        close(scan2odd[0]);
        close(scan2odd[1]);
        close(odd2scan[0]);
        close(odd2scan[1]);

        // Setup even pipes
        close(even2scan[0]);
        close(scan2even[1]);

        char buf[11];
        sprintf(buf, "%d", pids[0]);
        execlp("./even", "./even", buf, (char *) 0);
        // Did not go into exec, return error.
        exit(1);
    }

    // Odd
    pids[2] = fork();
    if (pids[2] < 0) {
        perror("Didn't fork odd.\n");
        exit(1);
    }
    if (pids[2] == 0) {
        // Close all even pipes
        close(scan2even[0]);
        close(scan2even[1]);
        close(even2scan[0]);
        close(even2scan[1]);

        // Setup odd pipes
        close(odd2scan[0]);
        close(scan2odd[1]);

        char buf[11];
        sprintf(buf, "%d", pids[0]);
        execlp("./odd", "./odd", buf, (char *) 0);
        // Did not go into exec, return error.
        exit(1);
    }

    // Close all pipes for driver. Will interfere
    close(scan2even[0]);
    close(scan2even[1]);
    close(even2scan[0]);
    close(even2scan[1]);
    close(scan2odd[0]);
    close(scan2odd[1]);
    close(odd2scan[0]);
    close(odd2scan[1]);

    // Wait for all processes to complete
    while(wait(NULL) > 0);

}
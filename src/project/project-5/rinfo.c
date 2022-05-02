#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <sys/select.h>

#define UDP_MAX_LENGTH 254

void printmessage(char *message) {
    printf("OS Implementation:\t<%s>\n", message);
    message += strlen(message) + 1;
    printf("Hostname:\t<%s>\n", message);
    message += strlen(message) + 1;
    printf("Release Level:\t<%s>\n", message);
    message += strlen(message) + 1;
    printf("Hardware Type:\t<%s>\n", message);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: rinfod <symbolic-hostname>\n");
        exit(1);
    }

    short port;
    FILE *fp = fopen("serverlocation", "rb");

    // Failed to open file
    if (fp == NULL) {
        sleep(4);
        fp = fopen("serverlocation", "rb");
        if (fp == NULL) {
            printf("Could not open 'serverlocation'. File may not exist\n");
            exit(1);
        }
    }

    fread(&port, sizeof(short), 1, fp);
    fclose(fp);

    printf("Server port in the host order is %u\n", ntohs(port));

    // Setup client
    int socket_fd;
    unsigned int reply_len, dest_size;
    fd_set mask;
    struct timeval timeout;
    char msg[UDP_MAX_LENGTH];
    struct sockaddr_in dest;
    struct hostent *hostptr;

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero((char *) &dest, sizeof(dest));
    if ((hostptr = gethostbyname(argv[1])) == NULL) {
        fprintf(stderr, "rinfo: Invalid host name '%s'", argv[1]);
    }
    dest.sin_family = AF_INET;
    bcopy(hostptr->h_addr_list[0], (char *) &dest.sin_addr, hostptr->h_length);
    dest.sin_port = port;

    // Get message contents and send to server
    gethostname(msg, UDP_MAX_LENGTH);

    // Setup select listener
    FD_ZERO(&mask);
    FD_SET(socket_fd, &mask);
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    // Attempt twice
    int i;
    for(i = 0; i < 2; i++) {
        timeout.tv_sec = 2;
        sendto(socket_fd, &msg, sizeof(msg), 0, (struct sockaddr *) &dest, sizeof(dest));
        int hits = select(socket_fd + 1, &mask, (fd_set *) 0, (fd_set *) 0, &timeout);
        // Did not receive
        if (hits == 0) {
            fprintf(stderr, "No message received from server on attempt <%d>\n", i+1);
            fflush(stderr);
            continue;
        }

        // Received message. First packet contains length
        dest_size = sizeof(dest);
        recvfrom(socket_fd, &reply_len, 4, 0, (struct sockaddr *) &dest, &dest_size);
        reply_len = ntohl(reply_len);
        printf("Reading <%d> bytes from server\n", reply_len);
        fflush(stdout);


        // Second packet contains message
        char reply[reply_len];
        recvfrom(socket_fd, reply, reply_len, 0, (struct sockaddr *) &dest, &dest_size);
        printmessage(reply);
        fflush(stdout);
        break;
    }
}

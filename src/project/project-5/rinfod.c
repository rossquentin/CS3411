#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/utsname.h>

#define UDP_MAX_LENGTH 254

void handler(int sigint) {
    remove("./serverlocation");
    printf("Server terminates on signal %d\n", sigint);
    fflush(stdout);
    exit(0);
}

int main(int argc, char **argv) {
    // Setup handler for SIGINT and SIGTERM
    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, NULL);

    // Prepare return data
    struct utsname uts;
    int len;

    uname(&uts);
    len = strlen(uts.sysname) + strlen(uts.nodename) + strlen(uts.release) + strlen(uts.machine) + 4;
    char response[len];
    strcpy(response, uts.sysname);
    sprintf(response, "%s%c%s%c%s%c%s%c", uts.sysname, 0, uts.nodename, 0, uts.release, 0, uts.machine, 0);
    len = htonl(len);

    // Setup server
    int listener;
    unsigned int fsize, length;
    char msg[UDP_MAX_LENGTH]; // Maximum size of UDP request;
    char from_ip[16], msg_ip[16];
    struct sockaddr_in s_in, from;
    struct hostent *clientptr;
    FILE *fp;

    listener = socket(AF_INET, SOCK_DGRAM, 0);
    bzero((char *) &s_in, sizeof(s_in));

    s_in.sin_family = (short) AF_INET;
    s_in.sin_addr.s_addr = htonl(INADDR_ANY);
    s_in.sin_port = htons(0);
    bind(listener, (struct sockaddr *) &s_in, sizeof(s_in));
    length = sizeof(s_in);
    getsockname(listener, (struct sockaddr *) &s_in, &length);

    printf("Port (Network byte order): %u\t(Host order): %u\n", s_in.sin_port, ntohs(s_in.sin_port));
    fflush(stdout);

    fp = fopen("serverlocation", "wr");
    fwrite(&s_in.sin_port, 2, 1, fp);
    fflush(fp);
    fclose(fp);


    // Server listener
    for (;;) {
        printf("\n");
        fflush(stdout);
        fsize = sizeof(from);
        bzero(msg, sizeof(msg));
        bzero(from_ip, 16);
        bzero(msg_ip, 16);
        // Block until message received
        recvfrom(listener, &msg, sizeof(msg), 0, (struct sockaddr *) &from, &fsize);

        strcpy(from_ip, inet_ntoa(from.sin_addr));

        // Print sender data
        printf("IP: %s\nPort (Network byte order): %d\t(Host order): %d\n", from_ip, from.sin_port,
               ntohs(from.sin_port));
        printf("Message from client is: <%s>\n", msg);

        clientptr = gethostbyname(msg);


        // Failed to resolve
        if (clientptr == NULL) {
            printf("Could not resolve DNS for <%s>\n", msg);
            fflush(stdout);
            continue;
        }

        strcpy(msg_ip, inet_ntoa(*(struct in_addr *) clientptr->h_addr_list[0]));
        // Good message
        if (strcmp(msg_ip, from_ip) == 0) {
            // Send response
            sendto(listener, &len, sizeof(int), 0, (struct sockaddr *) &from, sizeof(from));
            sendto(listener, &response, sizeof(response), 0, (struct sockaddr *) &from, sizeof(from));
            printf("Sending reply length: %d (Network byte order)\t%d (Host byte order)\n", len, ntohl(len));
            fflush(stdout);
        }
            // Deceitful client
        else {
            printf("\nServer refuses to reply to deceitful client\n");
            printf("DNS on client sent symbolic name gives: \t<%s>\n", msg_ip);
            printf("From address returned by recvfrom gives: \t<%s>\n", from_ip);
            fflush(stdout);
        }
    }
}

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int read_line(int newSd, char *line_to_return);

int main(int argc, char *argv[]) {

    int sd; // server socket number
    int newSd; // new connection number
    int cliLen;

    const int SERVER_PORT = 1500;
    const int MAX_MSG = 150;

    char line[MAX_MSG];

    struct sockaddr_in servAddr, cliAddr;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("cannot open socket");
        return -1;
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(SERVER_PORT);

    if (bind(sd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
        perror("cannot bind port");
        return -1;
    }

    listen(sd, 5);

    while (1) {
        printf("Server waiting on TCP port %u\n", SERVER_PORT);

        cliLen = sizeof(cliAddr);
        newSd = accept(sd, (struct sockaddr *) &cliAddr, (socklen_t *) &cliLen);
        if (newSd < 0) {
            perror("cannot accept connection");
            return -1;
        }

        memset(line, 0x0, MAX_MSG);

        while (read_line(newSd, line) != 1) {
            printf("%s: received from %s:TCP%d : %s\n", argv[0],
                   inet_ntoa(cliAddr.sin_addr),
                   ntohs(cliAddr.sin_port), line);
            memset(line, 0x0, MAX_MSG);
        }

    }

}

int read_line(int newSd, char *line_to_return) {

    const short ERROR=1;
    const char END_LINE=0x0;
    const int MAX_MSG=100;
    static int rcv_ptr=0;
    static char rcv_msg[100];
    static int n;
    int offset;

    offset=0;

    while(1) {
        if(rcv_ptr==0) {
            /* read data from socket */
            memset(rcv_msg,0x0,MAX_MSG); /* init buffer */
            n = recv(newSd, rcv_msg, MAX_MSG, 0); /* wait for data */
            if (n<0) {
                perror(" cannot receive data ");
                return ERROR;
            } else if (n==0) {
                printf(" connection closed by client\n");
                close(newSd);
                return ERROR;
            }
        }

        /* if new data read on socket */
        /* OR */
        /* if another line is still in buffer */

        /* copy line into 'line_to_return' */
        while(*(rcv_msg+rcv_ptr)!=END_LINE && rcv_ptr<n) {
            memcpy(line_to_return+offset,rcv_msg+rcv_ptr,1);
            offset++;
            rcv_ptr++;
        }

        /* end of line + end of buffer => return line */
        if(rcv_ptr==n-1) {
            /* set last byte to END_LINE */
            *(line_to_return+offset)=END_LINE;
            rcv_ptr=0;
            return ++offset;
        }

        /* end of line but still some data in buffer => return line */
        if(rcv_ptr <n-1) {
            /* set last byte to END_LINE */
            *(line_to_return+offset)=END_LINE;
            rcv_ptr++;
            return ++offset;
        }

        /* end of buffer but line is not ended => */
        /*  wait for more data to arrive on socket */
        if(rcv_ptr == n) {
            rcv_ptr = 0;
            return ++offset;
        }


    } /* while */
}
/**
 * The Sender file pseudo code:
 *
 * 1.for 5 times:
 *   1.1 open TCP socket
 *   1.2 create connection with host
 *   1.3 send "file.txt" to host using cubic algorithm
 *   1.4 close connection
 * 2.for 5 times:
 *   2.1 open TCP socket
 *   2.2 create connection with host
 *   2.3 send "file.txt" to host using reno algorithm
 *   2.4 close connection
 */

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


#define PORT 8879
#define SERVER_IP_ADDRESS "127.0.0.1"

/**
 * Send file function gets a buffer and socket,
 * and send "file.txt" to the socket.
 * @param buffer
 * @param sock
 */
void sendFile(char *buffer, int sock) {
    FILE *file;
    if ((file = fopen("file.txt", "rb")) == NULL)
        printf("opening file failed");

    int byte = 0;
    while ((byte = fread(buffer, 1, 1, file)) > 0) {
        int sent = send(sock, buffer, byte, 0);
        if (sent != byte) printf("error in sending | %d sent from %d\n", sent, byte);
    }
    fclose(file);
}
/**
 * Main function
 */
int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[255];

    //First we will send the file 5 times with the cubic algorithm
    for (int i = 0; i < 5; i++) {
        char CongestionControl[40] = "";
        int length = 40;
        //Create socket
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            printf("socket creation failed\n");
        }
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, (const char *) SERVER_IP_ADDRESS, &serv_addr.sin_addr) <= 0)
            printf("inet_ptopn failed\n");

        //Get socket from client
        if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, CongestionControl, &length) != 0) {
            perror("socket get error from client\n");
        }
        //Printing algo name
        printf("Sending file using %s algorithm\n", CongestionControl);
        //Connect to host
        if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            printf("connection failed\n");
        }

        //Send the file to socket
        sendFile(buffer, sock);
        //Close socket
        close(sock);
    }

    //After sending the file 5 times with cubic,
    //We will send the file again 5 times with Reno algorithm
    printf("Changing algoritm from cubic to reno\n");
    for (int i = 0; i < 5; i++) {

        //change CongestionControl algorithm to reno
        char CongestionControl[40] = "reno";
        int length = 40;

        sock = 0;
        char buffer[255] = {0};
        //Creating socket
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            printf("socket creation failed\n");
        }
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, (const char *) SERVER_IP_ADDRESS, &serv_addr.sin_addr) <= 0) {
            printf("inet_ptopn failed\n");
        }

        //Setting the socket
        if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, CongestionControl, length) != 0) {
            perror("set socket error from client\n");
        }
        //Getting the socket
        if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, CongestionControl, &length) != 0) {
            perror("socket get error from client\n");
        }//Printing algo name
        printf("Sending file using %s algorithm\n", CongestionControl);
        //Connecting to server host
        if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            printf("connection failed\n");
        }

        //Sending the file to the socket
        sendFile(buffer, sock);
        //Closing the socket
        close(sock);
    }

    return 0;
}

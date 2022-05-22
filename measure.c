/**
 * This class:
 *  1. Open TCP socket
 *  2. listen to the socket
 *  3. get connection from the sender
 *  4. get the file 5 times with Cubic algo while measuring times
 *  5. calculate average time
 *  6. change the algo's CC to reno
 *  7. get the file 5 times 
 *  8. calculate average time
 *  9. print times
 *  10. close connection  
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


#define SERVER_PORT 8879


struct sockaddr_in createSockAddr();

void getFileFromNewClient(int bytes, char *buffer, int clientSocket, double *currTime, int *totalbytes, int *ttbytes);

int main() {

    // socket Creation
    int mySocket;
    if ((mySocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Socket failed to create\n");
        return 1;
    }

    printf("Socket has benn created\n");

    struct sockaddr_in serverAddress = createSockAddr();

    //Bind socket to server address
    if (bind(mySocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
        printf("Bind failed \n error code : %d", errno);
        close(mySocket);
        return -1;
    }

    printf("Bind:\n");

    if (listen(mySocket, 5) == -1) {
        printf("Listening has been failed");
        return 1;
    }

    struct sockaddr_in clientAddr;
    socklen_t clientAddressLen = sizeof(clientAddr);

    double totalTime = 0;
    double currentTime = 0;
    int bytes = 0;
    int totalbytes = 0;
    char buffer[65536];
    int ttbytes = 0;

    for (int i = 0; i < 5; i++) {
        memset(&clientAddr, 0, sizeof(clientAddr));
        clientAddressLen = sizeof(clientAddr);
        int clientSocket = accept(mySocket, (struct sockaddr *) &clientAddr, &clientAddressLen);
        if (clientSocket == -1) {
            printf("failed accepting new client\n");
            close(mySocket);
            return 1;
        }
        printf("New client has been connected");

        getFileFromNewClient(bytes, buffer, clientSocket, &currentTime, &totalbytes, &ttbytes);

        printf("total bytes: %d in %f seconds\n", totalbytes, currentTime);

        totalTime += currentTime;
    }
    printf("TOTAL TIME: %f\nAVG TIME: %f\nBYTES RECEIVED: %d\n", totalTime, totalTime / 5, ttbytes);
    ttbytes = 0;
    bytes = 0;
    totalbytes = 0;
    totalTime = 0;
    currentTime = 0;

    //change cc to reno
    char CC[40] = "reno";
    int len = 40;

    if (setsockopt(mySocket, IPPROTO_TCP, TCP_CONGESTION, CC, len) != 0) {
        perror("set socket error from server\n");
    }
    if (getsockopt(mySocket, IPPROTO_TCP, TCP_CONGESTION, CC, &len) != 0) {
        perror("socket get error from server\n");
    }
    printf("the algo is %s\n", CC);

    for (int i = 0; i < 5; i++) {
        memset(&clientAddr, 0, sizeof(clientAddr));
        clientAddressLen = sizeof(clientAddr);
        int clientSocket = accept(mySocket, (struct sockaddr *) &clientAddr, &clientAddressLen);
        if (clientSocket == -1) {
            printf("failed accepting new client\n");
            close(mySocket);
            return 1;
        }
        printf("new client connection!");

        clock_t begin = clock();
        totalbytes = 0;
        while ((bytes = recv(clientSocket, buffer, sizeof(buffer), 0) > 0)) {
            totalbytes += bytes;
        }

        clock_t end = clock();
        currentTime = (double) (end - begin) / CLOCKS_PER_SEC;
        printf("total bytes: %d in %f seconds\n", totalbytes, currentTime);
        ttbytes += totalbytes;
        totalTime += currentTime;
    }
    printf("TOTAL TIME: %f\nAVARAGE TIME: %f\nBYTES RECEIVED: %d\n", totalTime, totalTime / 5, ttbytes);

    return 0;
}

void getFileFromNewClient(int bytes, char *buffer, int clientSocket, double *currTime, int *totalbytes, int *ttbytes) {
    clock_t begin = clock();
    time_t t = time(0);
    (*totalbytes) = 0;

    while ((bytes = recv(clientSocket, buffer, sizeof(buffer), 0) > 0)) {
        (*totalbytes) += bytes;
    }

    time_t e = time(0);
    clock_t end = clock();

    (*ttbytes) += (*totalbytes);
    (*currTime) = (double) (end - begin) / CLOCKS_PER_SEC;
}

struct sockaddr_in createSockAddr() {
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);
    return serverAddress;
}

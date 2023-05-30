#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>

#include <sys/types.h>

#include <sys/socket.h>



#define SERVER_IP "127.0.0.1"

#define PORT 5000



int main() {

    int sockfd;

    struct sockaddr_in serverAddr;



    // Soket oluşturuluyor

    sockfd = socket(AF_INET, SOCK_STREAM, 0);



    serverAddr.sin_family = AF_INET;

    serverAddr.sin_port = htons(PORT);

    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);



    // Sunucuya bağlanma

    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {

        perror("Bağlantı hatası");

        return 1;

    }



    char message[1024];

    fd_set readSet;

    while (1) {

        FD_ZERO(&readSet);

        FD_SET(sockfd, &readSet); // Soketi okuma kümesine ekler

        FD_SET(STDIN_FILENO, &readSet); // Standart girişi okuma kümesine ekler



        // Select fonksiyonu ile gelen veriyi takip eder

        select(sockfd + 1, &readSet, NULL, NULL, NULL);



        if (FD_ISSET(sockfd, &readSet)) {

            char receivedMessage[1024];

            ssize_t bytesRead = recv(sockfd, receivedMessage, sizeof(receivedMessage), 0);

            if (bytesRead > 0) {

                printf("\nİstemci 2'den gelen mesaj: %s", receivedMessage);

                fflush(stdout);

            }

        }



        if (FD_ISSET(STDIN_FILENO, &readSet)) {

            printf("Mesaj gönderildi\n");

            printf("Mesaj Gönder => ");

            fgets(message, sizeof(message), stdin);

            send(sockfd, message, strlen(message), 0);

        }

    }



    close(sockfd);



    return 0;

}
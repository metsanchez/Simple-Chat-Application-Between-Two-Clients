#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>

#include <sys/types.h>

#include <sys/socket.h>



#define PORT_1 5000

#define PORT_2 6000



// İstemciler arasında mesajları yönlendiren fonksiyon

void forwardMessages(int sourceSocket, int destinationSocket, int clientID) {

    char buffer[1024];

    ssize_t bytesRead;

    while ((bytesRead = recv(sourceSocket, buffer, sizeof(buffer), 0)) > 0) {

        send(destinationSocket, buffer, bytesRead, 0);

        

        if (clientID == 1) {

            printf("İstemci 1'den gelen mesaj: %s", buffer);

        } else if (clientID == 2) {

            printf("İstemci 2'den gelen mesaj: %s", buffer);

        }

        

        fflush(stdout);

    }

    close(sourceSocket);

    close(destinationSocket);

}



int main() {

    int sockfd1, sockfd2;

    struct sockaddr_in serverAddr1, serverAddr2;



    // İki soket oluşturuluyor

    sockfd1 = socket(AF_INET, SOCK_STREAM, 0);

    sockfd2 = socket(AF_INET, SOCK_STREAM, 0);



    serverAddr1.sin_family = AF_INET;

    serverAddr1.sin_port = htons(PORT_1);

    serverAddr1.sin_addr.s_addr = htonl(INADDR_ANY);



    serverAddr2.sin_family = AF_INET;

    serverAddr2.sin_port = htons(PORT_2);

    serverAddr2.sin_addr.s_addr = htonl(INADDR_ANY);



    // İlk soket bağlanıyor

    if (bind(sockfd1, (struct sockaddr *)&serverAddr1, sizeof(serverAddr1)) != 0) {

        perror("İstemci 1 için bind hatası");

        return 1;

    }



    // İkinci soket bağlanıyor

    if (bind(sockfd2, (struct sockaddr *)&serverAddr2, sizeof(serverAddr2)) != 0) {

        perror("İstemci 2 için bind hatası");

        return 1;

    }



    // İstemci 1'in dinlenmesi başlatılıyor

    if (listen(sockfd1, 10) != 0) {

        perror("İstemci 1 için dinleme hatası");

        return 1;

    }



    // İstemci 2'nin dinlenmesi başlatılıyor

    if (listen(sockfd2, 10) != 0) {

        perror("İstemci 2 için dinleme hatası");

        return 1;

    }



    printf("Socket başarıyla oluşturuldu, istemcilerin bağlanması bekleniyor.\n");



    while (1) {

        struct sockaddr_storage clientAddr;

        socklen_t clientAddrSize = sizeof(clientAddr);

        

        // İstemci 1'in kabul edilmesi

        int clientSocket1 = accept(sockfd1, (struct sockaddr *)&clientAddr, &clientAddrSize);

        if (clientSocket1 < 0) {

            perror("İstemci 1 için kabul hatası");

            return 1;

        }



        // İstemci 2'nin kabul edilmesi

        int clientSocket2 = accept(sockfd2, (struct sockaddr *)&clientAddr, &clientAddrSize);

        if (clientSocket2 < 0) {

            perror("İstemci 2 için kabul hatası");

            return 1;

        }



        // Her bir istemci için ayrı bir işlem oluşturuluyor ve mesajları yönlendirmek için forwardMessages() fonksiyonu çağrılıyor

        if (fork() == 0) {

            forwardMessages(clientSocket1, clientSocket2, 1);

            exit(0);

        }

        if (fork() == 0) {

            forwardMessages(clientSocket2, clientSocket1, 2);

            exit(0);

        }

    }



    return 0;

}


#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")
#define SIZE 8192

int main() {
    WSADATA wsa_data;
    SOCKET socket_desc;
    struct sockaddr_in server_addr, client_addr;
    socklen_t clientlen = sizeof(client_addr);
    char client_message[SIZE];
    char* server_message = "Hello from server\n";
    int client_struct_length = sizeof(client_addr);

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return -1;
    }

    socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_desc == INVALID_SOCKET) {
        printf("Error while creating socket: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    printf("Socket created successfully\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(23);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //Bind - server deðerleri
    if (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Couldn't bind to the port: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    printf("Bind Success\n");
    int ctr = 0 ;
    char client_ip[INET_ADDRSTRLEN];
    int client_port;

    //File'ý append modunda açma iþlemini yap - eðer file yoksa recv.txt oluþtur
    FILE* recvFile = fopen("recv.txt", "wb");
    if (recvFile == NULL) {
        printf("Failed to create file\n");
        return 1;
    }
 
    while (1) {
        int bytes_recv = recvfrom(socket_desc, client_message, SIZE, 0, (struct sockaddr*)&client_addr, &clientlen);
        if (bytes_recv == SOCKET_ERROR) {
            printf("recvfrom() failed: %d\n", WSAGetLastError());
            WSACleanup();
            return -1;
        }
        if (bytes_recv <= 0) {
            // Veri bitti, döngüden çýk
            break;
        }
        ctr += bytes_recv;
        //recvfrom'dan aldýðýn recvBuf datasýný recvFile'a yaz.
        fwrite(client_message, 1, bytes_recv, recvFile);


        client_port = ntohs(client_addr.sin_port);
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("%.*s\n", bytes_recv, client_message);

        memset(client_message, 0, sizeof(client_message));
        
    }
    printf("TOTAL BYTES:%d, from %s :: %d", ctr, client_ip, client_port);

    // Close the socket:
    closesocket(socket_desc);
    fclose(recvFile);
    WSACleanup();

    return 0;
}
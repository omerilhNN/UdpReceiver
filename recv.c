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

    //Bind - server de�erleri
    if (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Couldn't bind to the port: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    printf("Bind Success\n");
    int ctr = 0 ; //recv ile al�nan bytelar toplam� i�in.
    char client_ip[INET_ADDRSTRLEN];
    int client_port;

    //File'� append modunda a�ma i�lemini yap - e�er file yoksa recv.txt olu�tur
    FILE* recvFile = fopen("recv.txt", "wb");
    if (recvFile == NULL) {
        printf("Failed to create file\n");
        return 1;
    }
    //SIZE boyutunda verileri socketten recvfrom ile al -> recvfrom okunan byte say�s�n� d�nderir onu da bytes_recv'e atamas�n� yap
    int bytes_recv;
    while ((bytes_recv = recvfrom(socket_desc, client_message, SIZE, 0, (struct sockaddr*)&client_addr, &clientlen))> 0) {
        ctr += bytes_recv;
        //recvfrom'dan ald���n bytes_recv boyutundaki datay�(client_message) recvFile'a yaz.
        fwrite(client_message, 1, bytes_recv, recvFile);
        if (strcmp(client_message, "END") == 0) {
            break;
        }
        memset(client_message, 0, sizeof(client_message));
    }
    client_port = ntohs(client_addr.sin_port);
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("\nTOTAL BYTES RECV:%d, from %s :: %d", ctr, client_ip, client_port);

    // Close the socket:
    closesocket(socket_desc);
    fclose(recvFile);
    WSACleanup();

    return 0;
}
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")
#define SIZE 8192
#define RECV_BUFSIZE 8192*10000
#define TIMEO 20000
//1024 gönderildðni bildiðin halde 512 oku

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
    int ctr = 0 ; //recv ile alýnan bytelar toplamý için.
    char client_ip[INET_ADDRSTRLEN];
    int client_port;

    int recv_bufsize = RECV_BUFSIZE;
    int recvbuflen = sizeof(recv_bufsize);
    if (setsockopt(socket_desc, SOL_SOCKET,SO_RCVBUF,(char*)&recv_bufsize,&recvbuflen) == SOCKET_ERROR) {
        printf("Setsockopt failed: %d\n", WSAGetLastError);
        closesocket(socket_desc);
        WSACleanup();
        return 1;
    }
    printf("Setsockopt success\n");

    int timeo = TIMEO;
    int timeolen = sizeof(timeo);
    if (setsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO, &timeo,sizeof(timeo)) == SOCKET_ERROR) {
        printf("SETSOCK fail\n");
        closesocket(socket_desc);
        WSACleanup();
        return 1;
    }
    int actual_recv_buffer_size;
    int optlen = sizeof(actual_recv_buffer_size);
    if (getsockopt(socket_desc, SOL_SOCKET, SO_RCVBUF, (char*)&actual_recv_buffer_size, &optlen) == SOCKET_ERROR) {
        printf("getsockopt failed: %d\n", WSAGetLastError());
        closesocket(socket_desc);
        WSACleanup();
        return 1;
    }
    if (getsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeo, &timeolen) == SOCKET_ERROR) {
        printf("getsockopt failed2: %d\n", WSAGetLastError());
        closesocket(socket_desc);
        WSACleanup();
        return 1;
    }
    printf("Current receive buffer size updated: %d\n", actual_recv_buffer_size);
    //File'ý append modunda açma iþlemini yap - eðer file yoksa recv.txt oluþtur
    FILE* recvFile = fopen("recv.txt", "wb");
    if (recvFile == NULL) {
        printf("Failed to create file\n");
        return 1;
    }
    //SIZE boyutunda verileri socketten recvfrom ile al -> recvfrom okunan byte sayýsýný dönderir onu da bytes_recv'e atamasýný yap
    int bytes_recv;
    while ((bytes_recv = recvfrom(socket_desc, client_message,SIZE, 0, (struct sockaddr*)&client_addr, &clientlen))> 0) {
        ctr += bytes_recv;
        //recvfrom'dan aldýðýn bytes_recv boyutundaki datayý(client_message) recvFile'a yaz.
        fwrite(client_message, 1, bytes_recv, recvFile);
        
        if (strcmp(client_message, "END") == 0) {
            break;
        }

        //client_message'ýn her bir elemanýný 0la
        memset(client_message, 0, sizeof(client_message));
    }
    if (WSAGetLastError() == WSAETIMEDOUT) {
        printf("Recv timed out\n");
    }
    
    client_port = ntohs(client_addr.sin_port);
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("\nTOTAL BYTES RECV:%d, from %s :: %d\n", ctr, client_ip, client_port);

    // Close the socket:
    closesocket(socket_desc);
    fclose(recvFile);
    WSACleanup();

    return 0;
}
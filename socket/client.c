#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080

void *receive_handler(void *arg) {
    int client_socket = *(int*)arg;
    char buffer[1024];
    int bytes;

    while ((bytes = read(client_socket, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';
        printf("서버: %s\n", buffer);  // 서버로부터 받은 메시지 출력
    }
    return NULL;
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    pthread_t thread;
    char message[1024];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("10.42.0.2");
    server_addr.sin_port = htons(PORT);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("서버 연결 실패");
        exit(1);
    }

    printf("서버에 연결됨\n");
    pthread_create(&thread, NULL, receive_handler, (void*)&client_socket);

    while (1) {
        fgets(message, 1024, stdin);

        // 서버로 메시지를 전송
        write(client_socket, message, strlen(message));
        printf("서버에 보냈습니다.\n");
    }

    close(client_socket);
    return 0;
}


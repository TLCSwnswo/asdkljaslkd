#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define MAX_CLIENTS 10

int client_sockets[MAX_CLIENTS];
int client_count = 0;

void *client_handler(void *arg) {
    int client_socket = *(int*)arg;
    int client_id = client_socket;
    char buffer[1024];
    int bytes;

    printf("클라이언트 %d 연결됨\n", client_id);

    while ((bytes = read(client_socket, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';

        // 클라이언트가 보낸 메시지를 서버에 출력 (클라이언트로는 전송하지 않음)
        printf("클라이언트 %d가 보낸 메시지: %s\n", client_id, buffer);
    }

    printf("클라이언트 %d 연결 종료됨\n", client_id);
    close(client_socket);
    return NULL;
}

void *server_input_handler(void *arg) {
    char message[1024];

    while (1) {
        fgets(message, sizeof(message), stdin);

        // 모든 클라이언트에게 서버 메시지 전송
        for (int i = 0; i < client_count; i++) {
            write(client_sockets[i], message, strlen(message));
        }
    }
}

int main() {
    int server_socket, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    pthread_t thread, input_thread;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_socket, MAX_CLIENTS);

    printf("서버 시작, 클라이언트 접속 대기 중...\n");

    // 서버 입력 처리 스레드 생성
    pthread_create(&input_thread, NULL, server_input_handler, NULL);

    while ((new_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len))) {
        client_sockets[client_count++] = new_socket;
        pthread_create(&thread, NULL, client_handler, (void*)&new_socket);
    }

    close(server_socket);
    return 0;
}


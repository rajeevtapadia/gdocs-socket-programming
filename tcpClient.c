#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_BUFFER_SIZE 1024
#define PORT 4444

char buffer[1024];
int clientSocket;

void* receiveMessages(void* arg) {
    while (1) {
        if (recv(clientSocket, buffer, MAX_BUFFER_SIZE, 0) < 0) {
            printf("[-] Error in receiving data.\n");
            break;
        } else {
            printf("Server: \t%s\n", buffer);
        }
    }
    pthread_exit(NULL);
}

int main(){

	int ret;
	struct sockaddr_in serverAddr;

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if(ret < 0){
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");

	pthread_t receiveThread;
    if (pthread_create(&receiveThread, NULL, receiveMessages, NULL) != 0) {
        printf("[-] Error creating receive thread.\n");
        return 1;
    }

	while(1){
		printf("Client: \t");
		scanf("%s", &buffer[0]);
		send(clientSocket, buffer, strlen(buffer), 0);

		if(strcmp(buffer, ":exit") == 0){
			close(clientSocket);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}

		if(recv(clientSocket, buffer, 1024, 0) < 0){
			printf("[-]Error in receiving data.\n");
		}else{
			printf("Server: \t%s\n", buffer);
		}
	}

	return 0;
}

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include "shm_and_sem.h"
#include "client_msg.h"


int main(int argc, char** argv) {	//argv[1]:client_ip      argv[2]:client_port
	int shmid, semid;
	if (argc != 3) {
		printf("usage: ./show_client client_ip client_port\n");
	}
	
	//share memory
	
	char* pchar = get_sharememory(&shmid, sizeof(struct msg), (key_t)atoi(argv[2]), IPC_CREAT|0666);

	//semaphor
	semid = semget((key_t)atoi(argv[2]), 2, 0);
	//do not need to initialize
	struct sembuf sb;

	msg_t message;
	
	while (1) {
		//read share memory
		P(&semid, 0);
		memcpy(&message, pchar, sizeof(message));
		V(&semid, 1);

		if (strcmp(message.msg_buf, "QUIT") == 0) {
			break;
		}

		printf("from %s-%d:\n", inet_ntoa(message.client_addr.sin_addr), ntohs(message.client_addr.sin_port));
		printf("%s\n\n", message.msg_buf);
	}

	shmdt(pchar);
	return 0;
}

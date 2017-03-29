#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "shm_and_sem.h"
#include "client_msg.h"


int main(int argc, char** argv) {	//argv[1]:server_ip  argv[2]: server_port  argv[3]:client_ip  argv[4]:client_port
	int shmid, semid, sfd;

	if(argc != 5) {
		printf("usage: ./input_client server_ip server_port client_ip client_port\n");
		return 1;
	}
	printf("input \"QUIT\" to exit\n");

	//共享内存
	char* pchar = get_sharememory(&shmid, sizeof(struct msg), (key_t)atoi(argv[4]), IPC_CREAT|0666);

	//信号量,用于共享内存同步
	semid = semget((key_t)atoi(argv[4]), 2, IPC_CREAT|0666);
	unsigned short arr[2] = {0, 1};
	sem_initial(&semid, arr);
	struct sembuf sb;

	//client socket
	sfd = socket(AF_INET, SOCK_DGRAM, 0);		//udp
	if (sfd == -1) {
		perror("socket");
		exit(1);
	}

	struct sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(atoi(argv[4]));
	client_addr.sin_addr.s_addr = inet_addr(argv[3]);
	if (-1 == bind(sfd, (struct sockaddr*)&client_addr, sizeof(client_addr))) {
		perror("bind");
		exit(1);
	}

	//server address
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[3]);


	//send login message
	msg_t client_msg;
	memset(&client_msg, 0, sizeof(client_msg));
	client_msg.client_addr = client_addr;
	client_msg.msg_type = MSG_LOGIN;
	sprintf(client_msg.msg_buf, "%s-%s login!", argv[3], argv[4]);
	sendto(sfd, &client_msg, sizeof(client_msg), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

	//select
	fd_set rfds;
	int ret;
	while (1) {
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		FD_SET(sfd, &rfds);
		ret = select(1024, &rfds, NULL, NULL, NULL);
		if (ret == -1) {
			perror("select");
			exit(1);
		}
		if (FD_ISSET(0, &rfds)) {
			//读输入
			memset(&client_msg, 0, sizeof(client_msg));
			if (read(0, client_msg.msg_buf, MSG_SIZE - 1) > 0) {
				client_msg.msg_buf[strlen(client_msg.msg_buf) - 1] = '\0';
			}
			client_msg.client_addr = client_addr;
			client_msg.msg_type = MSG_NORMAL;

			//通过共享内存给显示程序
			//P(&semid, 1);
			//memcpy(pchar, &client_msg, sizeof(client_msg));
			//V(&semid, 0);

			if (strcmp(client_msg.msg_buf, "QUIT") == 0) {
				break;
			}

			//发送给服务器
			sendto(sfd, &client_msg, sizeof(client_msg),0, (struct sockaddr*)&server_addr, sizeof(server_addr));
		}

		if (FD_ISSET(sfd, &rfds)) {
			//接受消息
			memset(&client_msg, 0, sizeof(client_msg));
			recvfrom(sfd, &client_msg, sizeof(client_msg), 0, NULL, NULL);

			//通过共享内存给显示程序
			P(&semid, 1);
			memcpy(pchar, &client_msg, sizeof(client_msg));
			V(&semid, 0);
		}
	}

	//send offlin message
	memset(&client_msg, 0, sizeof(client_msg));
	client_msg.client_addr = client_addr;
	client_msg.msg_type = MSG_OFFLINE;
	sprintf(client_msg.msg_buf, "%s-%s get off line!", argv[3], argv[4]);
	sendto(sfd, &client_msg, sizeof(client_msg), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
	shmdt(pchar);

	//确保信息接收完才删除信共享内存
	P(&semid, 1);
	shmctl(shmid, IPC_RMID, NULL);
	V(&semid, 0);

	semctl(semid, 0, IPC_RMID, NULL);

	return 0;

}


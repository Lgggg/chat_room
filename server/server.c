#include "server_msg.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>


int main(int argc, char** argv) {  //argv[1]:server ip   argv[2]:server port

	if (argc != 3) {
		printf("usage: ./server ip port\n");
		exit(1);
	}


	pnode_t phead = NULL;
	
	int sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sfd == -1) {
		perror("socket");
		exit(1);
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	if(-1 == bind(sfd, (struct sockaddr*)&server_addr, sizeof(server_addr))) {
		perror("bind");
		close(sfd);
		exit(1);
	}

	socklen_t len;
	msg_t client_msg;
	
	while (1) {
		memset(&client_msg, 0, sizeof(client_msg));
		recvfrom(sfd, &client_msg, sizeof(client_msg), 0, NULL, NULL);			//客户端地址在msg_t结构体中
		printf("receive message from %s-%d\n", inet_ntoa(client_msg.client_addr.sin_addr), ntohs(client_msg.client_addr.sin_port));
		if (client_msg.msg_type == MSG_LOGIN) {
			msg_login_handle(sfd, &phead, &client_msg, &msg_send);
		}

		if (client_msg.msg_type == MSG_NORMAL) {
			msg_normal_handle(sfd, &phead, &client_msg, &msg_send);
		}

		if (client_msg.msg_type == MSG_OFFLINE) {
			msg_offline_handle(sfd, &phead, &client_msg, &msg_send);
		}
	}
	
	return 0;
}
	

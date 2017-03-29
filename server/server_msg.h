#ifndef __SERVER_MSG_H__
#define __SERVER_MSG_H__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MSG_SIZE 1024
#define MSG_LOGIN 1
#define MSG_NORMAL 2
#define MSG_OFFLINE 3

typedef struct node {
	struct sockaddr_in client_addr;
	struct node* next;
}node_t, *pnode_t;

typedef struct msg {
	struct sockaddr_in client_addr;
	int msg_type;
	char msg_buf[MSG_SIZE];
}msg_t, *pmsg_t;

typedef void (*psendFun_t)(int sfd, struct sockaddr_in* pclient_addr, msg_t* pclient_msg);

void msg_login_handle(int sfd, pnode_t* pphead, msg_t* pclient_msg, psendFun_t send_func);
void msg_normal_handle(int sfd, pnode_t* pphead, msg_t* pclient_msg, psendFun_t send_func);
void msg_offline_handle(int sfd, pnode_t* pphead, msg_t* pclient_msg, psendFun_t send_func);

void link_insert(pnode_t* pphead, struct sockaddr_in* pclient_addr);
void link_delete(pnode_t* pphead, struct sockaddr_in* pclient_addr);
void link_visit(int sfd, pnode_t* pphead, psendFun_t send_fun, msg_t* pclient_msg);

void msg_send(int sfd, struct sockaddr_in* pclient_addr, msg_t* pclient_msg);
#endif

#ifndef __CLIENT_MSG_H__
#define __CLIENT_MSG_H__
#include <netinet/in.h>

#define MSG_SIZE 1024
#define MSG_LOGIN 1
#define MSG_NORMAL 2
#define MSG_OFFLINE 3

typedef struct msg {
	struct sockaddr_in client_addr;
	int msg_type;
	char msg_buf[MSG_SIZE];
}msg_t, *pmsg_t;

#endif

#include "server_msg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void msg_login_handle(int sfd, pnode_t* pphead, msg_t* pclient_msg, psendFun_t msg_send) {
	link_insert(pphead, &(pclient_msg->client_addr));
	link_visit(sfd, pphead, msg_send, pclient_msg);
}

void msg_normal_handle(int sfd, pnode_t* pphead, msg_t* pclient_msg, psendFun_t msg_send) {
	link_visit(sfd, pphead, msg_send, pclient_msg);
}

void msg_offline_handle(int sfd, pnode_t* pphead, msg_t* pclient_msg, psendFun_t msg_send) {
	link_delete(pphead, &(pclient_msg->client_addr));
	link_visit(sfd, pphead, msg_send, pclient_msg);
}

void link_insert(pnode_t* pphead, struct sockaddr_in* pclient_addr) {
	pnode_t pnew = (pnode_t)calloc(1, sizeof(node_t));
	pnew->client_addr = *pclient_addr;
	pnew->next = *pphead;
	*pphead = pnew;
}

void link_delete(pnode_t* pphead, struct sockaddr_in* pclient_addr) {
	pnode_t pcur = *pphead;
	pnode_t ppre = NULL;
	while (pcur != NULL) {
		if (pcur->client_addr.sin_port == pclient_addr->sin_port
				&& pcur->client_addr.sin_addr.s_addr == pclient_addr->sin_addr.s_addr) {
			break;
		}
		ppre = pcur;
		pcur = pcur->next;
	}

	if (pcur != NULL) {
		if (ppre == NULL) {
			*pphead = pcur->next;
		}
		else {
			ppre->next = pcur->next;
		}
		free(pcur);
	}
}

void link_visit(int sfd, pnode_t* pphead, psendFun_t send_fun, msg_t* pclient_msg) {
	pnode_t pcur = *pphead;
	while (pcur != NULL) {
		(*msg_send)(sfd, &pcur->client_addr, pclient_msg);
		pcur = pcur->next;
	}
}


void msg_send(int sfd, struct sockaddr_in* pclient_addr, msg_t* pclient_msg) {
	sendto(sfd, pclient_msg, sizeof(*pclient_msg), 0, (struct sockaddr*)pclient_addr, sizeof(*pclient_addr));
}




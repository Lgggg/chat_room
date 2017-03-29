#ifndef __SHM_AND_SEM_H__
#define __SHM_AND_SEM_H__
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <sys/types.h>
union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO
								(Linux-specific) */
};

char* get_sharememory(int* pshmid, size_t size, key_t key, int shmflag); 

void sem_initial(int* psemid, unsigned short* arr);
void P(int* psemid, int semnum);
void V(int* psemid, int semnum);


#endif

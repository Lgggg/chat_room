#include "shm_and_sem.h"

char* get_sharememory(int* pshmid, size_t size, key_t key, int shmflag) {
	//¹²ÏíÄÚ´æ
	*pshmid = shmget(key, size, shmflag);
	if (*pshmid == -1) {
		perror("shmget");
		exit(1);
	}

	char* pchar = (char*) shmat(*pshmid, NULL, 0);
	if ((void*)pchar == (void*)-1) {
		perror("shmat");
		exit(1);
	}

	return pchar;
}


void sem_initial(int* psemid, unsigned short* arr) {
	union semun su;
	su.array = arr;
	if (-1 == semctl(*psemid, 0, SETALL, su)) {
		perror("semctl");
		exit(1);
	}
}



void P(int* psemid, int semnum) {
	struct sembuf sb;
	sb.sem_num = semnum;
	sb.sem_op = -1;
	sb.sem_flg = SEM_UNDO;
	if (-1 == semop(*psemid, &sb, 1)) {
		perror("semop");
		exit(1);
	}
}
	

void V(int* psemid, int semnum) {
	struct sembuf sb;
	sb.sem_num = semnum;
	sb.sem_op = 1;
	sb.sem_flg = SEM_UNDO;
	if (-1 == semop(*psemid, &sb, 1)) {
		perror("semop");
		exit(1);
	}
}

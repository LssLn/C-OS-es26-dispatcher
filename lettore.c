#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>

#define SEM_KEY 		7777
#define SHARED_MEMORY_KEY 	7778


struct dati {
	int id_notizia;
	char notizia[200];
	int num_processi_lettori_attivi;
	int num_letture_effettuate;
};

union semun {
    int val;
    struct semid_ds *buf;   
    unsigned short  *array;  
    struct seminfo  *__buf;  
};


int sem_set(int semid, int val);
int sem_down(int semid);
int sem_up(int semid);

int sem_set(int semid, int val){
	union semun un;
	un.val = val;
	return semctl(semid, 0, SETVAL, un);

}
int sem_down(int semid){
	struct sembuf buf;
	buf.sem_num = 0;
	buf.sem_flg = SEM_UNDO;
	buf.sem_op = -1;
	return semop(semid, &buf, 1);
}
int sem_up(int semid){
	struct sembuf buf;
	buf.sem_num = 0;
	buf.sem_flg = SEM_UNDO;
	buf.sem_op = 1;
	return semop(semid, &buf, 1);
}

int main (){
	int semid, shmid, time_to_exit = 0;
	int ultima_notizia_letta = 0;
	struct dati *notizia;
	
	semid = semget((key_t)SEM_KEY, 1, 0666 | IPC_CREAT);
	shmid = shmget((key_t)SHARED_MEMORY_KEY, sizeof(struct dati), 0666 | IPC_CREAT); //creata la shared memory

	notizia = (struct dati *)shmat(shmid, NULL, 0);
	
	sem_down(semid);
	notizia->num_processi_lettori_attivi++;
	sem_up(semid);

	while(!time_to_exit){
		sem_down(semid);
		if(notizia->id_notizia != ultima_notizia_letta) { 
			notizia->num_letture_effettuate++;
			printf("Notizia: %s", notizia->notizia);
			ultima_notizia_letta = notizia->id_notizia;
			if(strncmp(notizia->notizia, "termina", 7) == 0){
				time_to_exit = 1;			
			}
		}
		sem_up(semid);
	} 
}

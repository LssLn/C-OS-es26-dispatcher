//												ESERCIZIO 3
/*
Implementare un programma in C che permetta la gestione di un servizio ansa tra processi.

Il programma deve prevedere un processo chiamato dispatcher che chiede all'utente di inserire una notizia da tastiera e la invia a tutti i processi in ascolto. 

Non si sa a priori quanti processi sono in ascolto.
Ogni nuovo processo che si mete in ascolto incrementa di 1 variabile intera condivisa all'interno all'interno della shared memory.

Ogni notizia deve essere letta da tutti i processi in ascolto una sola volta e stampata a video. 

Il processo dispatcher se legge la notizia (stringa) "termina" la scrive nella shared memory, attende che tutti i processi abbiano letto e dopo termina

Cosa succede al dispatcher: 
1. legge notizia da tastiera
2. invia notizia al lettore

Nella shared memory deve esserci:
1. stringa notizia
2. int numero lettori
3. int conta letture 
4. int id notizia

Cosa fa il processo lettore:
1. all'avvio shm->numlettori++
2. fa partire un ciclo in cui legge la notizia, se è nuova il contalettore viene incrementato

DATI CONDIVISI
struct dati {
	int id_notizia;
	char notizia[200];
	int num_processi_lettori_attivi;
	int num_letture_effettuate;
};

OPERAZIONI DISPATCHER
1. Creazione semaforo/i
2. Creazione e attach shared memory 
3. Richiesta notizia da tastiera
4. Azzeramento num_letture_effettuate
5. Controllo num_letture_effettuate == num_processi_lettori_attivi ossia, attende che tutti i lettori abbiano letto 
6. Controllo notizia == termina, termina, altrimenti
7. Ritorno al punto 3

OPERAZIONI LETTORE
1. Creazione semaforo/i
2. Creazione e attach shared memory 
3. Incremento num_processi_lettori_attivi
4. lettura notizia
5. Se nuova notizia: la stampa a video e incrementa num_letture_effettuate
6. se notizia == termina, termina, altrimenti va al punto 4. 

*/

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
	struct dati *notizia;
	
	semid = semget((key_t)SEM_KEY, 1, 0666 | IPC_CREAT);
	shmid = shmget((key_t)SHARED_MEMORY_KEY, sizeof(struct dati), 0666 | IPC_CREAT); //creata la shared memory

	notizia = (struct dati *)shmat(shmid, NULL, 0);

	sem_set(semid, 1);

	//qui parte il programma dispatcher
	
	sem_down(semid);
	notizia->id_notizia = 0;
	notizia->num_letture_effettuate = 0;
	notizia->num_processi_lettori_attivi = 0;
	notizia->notizia[0] = '\0';
	sem_up(semid);

	printf("Puoi avviare i processi lettori\n");

	while(1){
	//codice dispatcher
		sem_down(semid); //prima cosa da fare
		if(notizia->id_notizia == 0 || notizia->num_letture_effettuate == notizia->num_processi_lettori_attivi){ //non abbiamo ancora scritto nulla oppure ...
				
			if(time_to_exit)
				break;		

		printf("Inserire notizia\n");
		fgets(notizia->notizia, 200, stdin);
		notizia->id_notizia++;
		notizia->num_letture_effettuate = 0;	
		}
		if(strncmp(notizia->notizia, "termina", 7) == 0){
		time_to_exit = 1;
		}
		sem_up(semid); 
		usleep(200000);
	} 
	
	semctl(semid, 0, IPC_RMID);
	shmdt(notizia);
	shmctl(shmid, IPC_RMID, NULL);
}

//Adesso tocca al processo lettore









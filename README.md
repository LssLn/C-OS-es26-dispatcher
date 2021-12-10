# C-OS-es26-dispatcher
Un processo chiamato dispatcher chiede all'utente di inserire una notizia da tastiera e la invia a tutti i processi in ascolto. 

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
2. fa partire un ciclo in cui legge la notizia, se รจ nuova il contalettore viene incrementato

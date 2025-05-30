#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LEN_MESSAGGIO 256
#define MAX_LEN_CAUSALE 50

// La utilizzo per gestire il singolo movimento e il nodo della lista del movimento
typedef struct Movimento {
    int id_operazione;
    float importo;
    char causale[MAX_LEN_CAUSALE];
    struct Movimento *next; // punto al prossimo nodo, ovvero al prossimo movimento nella lista
} Movimento;

void* gestisci_client(void* arg); // la eseguirà ogni client
void init_lista(); // funzione per l'inizializzazione della lista prima dell'utilizzo effettivo
void add_operazione(float importo, char* causale); // aggiungo un movimento in testa alla lista (uso le LIFO)
int delete_operazione(int id_da_cancellare); // restituisce 1 in caso di successo, 0 in caso di errore
int update_operazione(int id_da_aggiornare, float nuovo_importo, char* nuova_causale); // restituisce 1 (successo), 0 (fallimento)
void list_operazione(int client_socket);
void pulizia();

#endif
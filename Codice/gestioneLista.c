#include "header.h"

static Movimento *testa_lista = NULL; // ovviamente, all'inzio la testa della lista non conterrà assolutamente nulla
static pthread_mutex_t lista_mutex;
static int id_operazione_succ = 1;

void init_lista() {
    if (pthread_mutex_init(&lista_mutex, NULL) != 0) {
        perror("Non sono riuscito ad inizializzare il mutex");
        exit(EXIT_FAILURE);
    }
}

void add_operazione(float importo, char *causale) {
    pthread_mutex_lock(&lista_mutex); // vado a bloccare per la mutua esclusione
    Movimento *nuovo_movimento = (Movimento *) malloc(sizeof(Movimento)); // alloco lo spazio necessario per il nuvoo nodo

    if (nuovo_movimento) { // se riesce ad allocare lo spazio
        nuovo_movimento->id_operazione = id_operazione_succ++; // incremento il valore di id
        nuovo_movimento->importo = importo;
        strncpy(nuovo_movimento->causale, causale, MAX_LEN_CAUSALE - 1); // copio il valore di causale in movimento.causale e non mi permette di andare oltre alla lunghezza di massima di len
        nuovo_movimento->causale[MAX_LEN_CAUSALE - 1] = '\0'; // aggiungo carattere di terminazione di stringa alla fine della causale
        nuovo_movimento->next = testa_lista; // punterà al nodo che era primo (prima dell'inserimento), questo diventerà il secondo nodo, mentre quello che ho appena creato, diventerà il primo (testa della lista)
        testa_lista = nuovo_movimento; // la testa della lista è il nodo appena creato
        printf("Aggiungo il movimento ID: %d", nuovo_movimento->id_operazione);
    }
    pthread_mutex_unlock(&lista_mutex); // sblocco il mutex dopo aver inserito l'operazione
}

void list_operazione (int client_socket) {
    char buffer[MAX_LEN_MESSAGGIO *10] = {0}; // Lo utilizzo per contenere l'intero messaggio di risposta da mandare al client, che vado a riempire ogni volta
    char riga[MAX_LEN_MESSAGGIO]; // lo utilizzo per stampe "semplici", quando stampo una riga alla volta

    pthread_mutex_lock(&lista_mutex); // blocco

    strcpy(buffer, "Lista dei movimenti:\n");
    Movimento* corrente = testa_lista; // Prendo l'ultima operazione inserita

    if (corrente == NULL) {
        strcat(buffer, "Nessun movimento presente \n"); // concateno le due stringhe (buffer (vuoto) + "")
    }else {
        while (corrente != NULL) {
            sprintf(riga, "ID: %d, importo: %.2f, causale: %s\n", corrente->id_operazione, corrente->importo, corrente->causale);
            strcat(buffer, riga);
            corrente = corrente->next; // vado a prendere il nodo successivo fino a quando non arrivo a NULL
        }
    }

    pthread_mutex_unlock(&lista_mutex); // sblocco il mutex dopo il calcolo di tutte le operazioni effettuate e salvate bel buffer
    strcat(buffer, "Fine lista movimenti\n");
    send(client_socket, buffer, strlen(buffer), 0); // invio al client tutto il buffer
}

int delete_operazione(int id_da_cancellare) {
    pthread_mutex_lock(&lista_mutex);

    Movimento* corrente = testa_lista; // imposto l'ultima operazione come testa della lista
    Movimento* precedente = NULL;
    int trovato = 0;

    while (corrente != NULL) {
        if (corrente->id_operazione == id_da_cancellare) {
            trovato = 1;
            break; // è inutile continuare il while
        }
        precedente = corrente; // controllo l'altro nodo
        corrente = corrente->next;
    }

    if (trovato) {
        if (precedente == NULL) {
            testa_lista = corrente->next; // se il nodo da cancellare è il primo, non c'è un precedente e la testa diventa il nodo successivo
        }
        else { // salto il nodo corrente e collego il precedente al successivo
            precedente->next = corrente->next;
        }
        free(corrente); // libero la memoria occupata dal nodo eliminato
        printf("Ho cancellato il movimento ID %d\n", id_da_cancellare);
    }

    pthread_mutex_unlock(&lista_mutex);
    return trovato;
}

int update_operazione(int id_da_aggiornare, float nuovo_importo, char* nuova_causale) {
    pthread_mutex_lock(&lista_mutex);
    Movimento* corrente = testa_lista; // prendo l'ultima operazione
    int trovato = 0;

    while (corrente != NULL) {
        if (corrente->id_operazione == id_da_aggiornare) {
            corrente->importo = nuovo_importo;
            strncpy(corrente->causale, nuova_causale, MAX_LEN_CAUSALE - 1);
            corrente->causale[MAX_LEN_CAUSALE - 1] = '\0';
            trovato = 1;
            printf("Aggiornato movimento ID %d\n", id_da_aggiornare);
            break; // inutile continuare il ciclo
        }
        corrente = corrente->next;
    }

    pthread_mutex_unlock(&lista_mutex);
    return trovato;
}

void pulizia() {
    pthread_mutex_lock(&lista_mutex); // blocco il mutex
    Movimento* corrente = testa_lista; // prendo l'ultima operazione salvata
    Movimento* temp;

    while (corrente != NULL) { // effettuo lo swap per prendere il valore successivo ed eliminare quello attuale e così via fino ad arrivare a NULL
        temp = corrente;
        corrente = corrente->next;
        free(temp);
    }

    testa_lista = NULL; // imposto la testa a NULL
    pthread_mutex_unlock(&lista_mutex); // sblocco il mutex
    pthread_mutex_destroy(&lista_mutex); // rimuovo definitivamente il mutex
    printf("Ho eliminato la lista");
}

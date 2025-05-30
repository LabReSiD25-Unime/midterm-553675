#include "header.h"

void* gestisci_client(void* arg) {
    int client_socket = *(int*)arg;
    free(arg); // libero la memoria allocata nel server.c

    char buffer[MAX_LEN_MESSAGGIO];
    int read_size;

    // Invia un messaggio di benvenuto/istruzioni al client
    char* welcome_msg = "Comandi: ADD <imp> <caus>, DEL <id>, UPD <id> <imp> <caus>, LIST, EXIT\n";
    send(client_socket, welcome_msg, strlen(welcome_msg), 0);

    // la chiamata recv è bloccante
    while ((read_size = recv(client_socket, buffer, MAX_LEN_MESSAGGIO, 0)) > 0) {
        buffer[read_size] = '\0';

        char comando[20] = {0}; // Aumentato per sicurezza, inizializzato
        int id;
        float importo;
        char causale[MAX_LEN_CAUSALE] = {0}; // Inizializzato

        // Estrai solo la prima "parola" come comando
        sscanf(buffer, "%19s", comando);

        if (strcasecmp(comando, "ADD") == 0) {
            // %*s salta la prima parola (il comando) che abbiamo già letto con sscanf precedente
            if (sscanf(buffer, "%*s %f %s", &importo, causale) == 2 && strlen(causale) > 0) {
                add_operazione(importo, causale);
                send(client_socket, "OK: Movimento aggiunto.\n", strlen("OK: Movimento aggiunto.\n") ,0);
            } else {
                send(client_socket, "ERRORE: Formato ADD non valido. Usa: ADD <importo> <causale>\n", strlen("ERRORE: Formato ADD non valido. Usa: ADD <importo> <causale>\n"), 0);
            }
        } else if (strcasecmp(comando, "DEL") == 0) {
            if (sscanf(buffer, "%*s %d", &id) == 1) {
                if (delete_operazione(id)) {
                    send(client_socket, "OK: Movimento cancellato.\n", strlen("OK: Movimento cancellato.\n"), 0);
                } else {
                    send(client_socket, "ERRORE: ID non trovato per DEL.\n", strlen("ERRORE: ID non trovato per DEL.\n"), 0);
                }
            } else {
                 send(client_socket, "ERRORE: Formato DEL non valido. Usa: DEL <id>\n", strlen("ERRORE: Formato DEL non valido. Usa: DEL <id>\n"), 0);
            }
        } else if (strcasecmp(comando, "UPD") == 0) {
            if (sscanf(buffer, "%*s %d %f %s", &id, &importo, causale) == 3 && strlen(causale) > 0) {
                if (update_operazione(id, importo, causale)) {
                    send(client_socket, "OK: Movimento aggiornato.\n", strlen("OK: Movimento aggiornato.\n"), 0);
                } else {
                    send(client_socket, "ERRORE: ID non trovato per UPD.\n", strlen("ERRORE: ID non trovato per UPD.\n"), 0);
                }
            } else {
                send(client_socket, "ERRORE: Formato UPD non valido. Usa: UPD <id> <importo> <causale>\n", strlen("ERRORE: Formato UPD non valido. Usa: UPD <id> <importo> <causale>\n"), 0);
            }
        } else if (strcasecmp(comando, "LIST") == 0) {
            list_operazione(client_socket);
        } else if (strcasecmp(comando, "EXIT") == 0) {
            send(client_socket, "Disconnessione...\n", strlen("Disconnessione...\n"), 0);
            break;
        } else {
            if(strlen(comando) > 0){ // Invia errore solo se è stato ricevuto un comando non vuoto
                 send(client_socket, "ERRORE: Comando non riconosciuto.\n", strlen("ERRORE: Comando non riconosciuto.\n"), 0);
            }
            // Se il client invia una riga vuota, strlen(comando) sarà 0, e non inviamo nulla,
            // il client semplicemente mostrerà un nuovo prompt.
        }
    }

    // Questo blocco viene eseguito DOPO che il ciclo while è terminato
    if (read_size == 0) {
        printf("Client (socket %d) ha chiuso la connessione.\n", client_socket);
    } else if (read_size < 0) {
        perror("Errore recv nel thread");
    } else { // Uscito per EXIT
         printf("Client (socket %d) disconnesso tramite EXIT.\n", client_socket);
    }

    close(client_socket);
    printf("Thread per client (socket %d) terminato.\n", client_socket);
    return NULL;
}

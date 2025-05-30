#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "header.h"

#define SERVER_IP "127.0.0.1" // Indirizzo IP del server (localhost)
#define SERVER_PORT 8888      // Porta del server (deve corrispondere a quella del server.c)

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char message[MAX_LEN_MESSAGGIO];
    char server_reply[MAX_LEN_MESSAGGIO * 10]; // Buffer più grande per risposte lunghe del server

    // Creazione del socket TCP, AF_INET per IPv4, SOCK_STREAM per TCP, 0 per il protocollo di default (TCP)
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Errore creazione socket client");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT); // Converte la porta in network byte order
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Questa è una chiamata BLOCCANTE. Il client attende che il server accetti la connessione.
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connessione al server fallita");
        close(sock);
        return 1;
    }
    printf("Connesso al server %s sulla porta %d\n", SERVER_IP, SERVER_PORT);

    // Ricezione del messaggio di benvenuto/istruzioni dal server
    int read_size = recv(sock, server_reply, sizeof(server_reply) - 1, 0);
    if (read_size > 0) {
        server_reply[read_size] = '\0';
        printf("%s", server_reply); // Il server invia già i comandi e un newline
    } else {
        printf("Nessun messaggio di benvenuto ricevuto o errore.\n");
        close(sock);
        return 1;
    }

    // Ciclo di comunicazione
    while (1) {
        printf("> ");
        if (fgets(message, sizeof(message), stdin) == NULL) {
            break; // Errore o EOF
        }
        message[strcspn(message, "\n")] = 0; // Rimuove newline

        if (strlen(message) == 0) {
            continue;
        }
        // Invia il comando al server
        if (send(sock, message, strlen(message), 0) < 0) {
            perror("Invio fallito");
            break;
        }
        if (strcasecmp(message, "EXIT") == 0) {
            printf("Disconnessione...\n");
            break;
        }
        // Ricevi la risposta dal server
        read_size = recv(sock, server_reply, sizeof(server_reply) - 1, 0);
        if (read_size > 0) {
            server_reply[read_size] = '\0';
            printf("Server: %s", server_reply);
        } else if (read_size == 0) {
            printf("Server ha chiuso la connessione.\n");
            break;
        } else {
            perror("Errore ricezione");
            break;
        }
    }

    // Chiusura del socket
    close(sock);
    printf("Socket client chiuso.\n");

    return 0;
}

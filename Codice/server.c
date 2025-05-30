#include "header.h"

#define PORTA 8888

int main () {
    // Creo la lista e la inizializzo
    init_lista();

    int server_socket;
    struct  sockaddr_in server_addr;

    server_socket = socket (AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1 ) {
        perror("Errore nella creazione della socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORTA);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind fallito");
        exit(EXIT_FAILURE);
    }

    listen(server_socket, 10);
    printf("Server in ascolto sulla porta %d...\n", PORTA);


    while (1) {
        // La chiamata accept() è BLOCCANTE
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) {
            perror("Accept fallito");
            continue;
        }

        printf("Nuova connessione accettata (socket %d)\n", client_socket);

        // Alloco memoria per passare il socket al thread in modo sicuro
        int* new_sock = malloc(sizeof(int));
        *new_sock = client_socket;

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, gestisci_client, (void*)new_sock) < 0) {
            perror("Errore creazione thread");
            free(new_sock); // Libera la memoria se il thread non parte
        }
        pthread_detach(thread_id);
    }

    close(server_socket);
    pulizia();
    return 0;
}

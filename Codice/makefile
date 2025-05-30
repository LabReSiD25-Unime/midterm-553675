# Compilatore da usare
CC = gcc

# Flag di compilazione:
# -Wall -> mostra tutti i warning
# -g    -> include i simboli di debug (utile per gdb)
# -pthread -> FONDAMENTALE per linkare la libreria Pthreads
CFLAGS = -Wall -g -pthread

# Nome dell'header file principale (come da tua indicazione)
# Se usi ancora common.h, gestione_client.h, gestione_lista.h separatamente,
# dovrai specificare quelle dipendenze per ogni file .o
HEADER = header.h

# Nomi degli eseguibili
SERVER_EXEC = server
CLIENT_EXEC = client # Assumendo che tu abbia ancora un client.c

# File sorgente per il server
SERVER_SRCS = server.c gestioneClient.c gestioneLista.c
# File oggetto (.o) corrispondenti per il server
SERVER_OBJS = $(SERVER_SRCS:.c=.o)

# File sorgente per il client (se esiste)
CLIENT_SRCS = client.c
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)


# Target di default: compila tutto
all: $(SERVER_EXEC) $(CLIENT_EXEC)

# Regola per compilare il server:
# Prima compila i .c in .o (se necessario), poi li collega insieme.
$(SERVER_EXEC): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $(SERVER_EXEC) $(SERVER_OBJS)

# Regola per compilare il client:
$(CLIENT_EXEC): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $(CLIENT_EXEC) $(CLIENT_OBJS)

# Regole per compilare i singoli file oggetto del server.
# Ognuno dipende dal rispettivo file .c e dal file header.h.
server.o: server.c $(HEADER)
	$(CC) $(CFLAGS) -c server.c -o server.o

gestioneClient.o: gestioneClient.c $(HEADER)
	$(CC) $(CFLAGS) -c gestioneClient.c -o gestioneClient.o

gestioneLista.o: gestioneLista.c $(HEADER)
	$(CC) $(CFLAGS) -c gestioneLista.c -o gestioneLista.o

# Regola per compilare il file oggetto del client (se esiste)
client.o: client.c $(HEADER)
	$(CC) $(CFLAGS) -c client.c -o client.o

# Regola per pulire i file compilati e gli oggetti
clean:
	rm -f $(SERVER_EXEC) $(CLIENT_EXEC) $(SERVER_OBJS) $(CLIENT_OBJS)

# Phony targets: 'all' e 'clean' non sono nomi di file reali.
.PHONY: all clean

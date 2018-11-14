#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <iterator>
#include <locale> 
#include <fstream>

#include "ClientHandler.h"
#include "Functions.h"

#define BUF 1024

/**
 * In der Main Funktion werden die Eingangsparameter auf gültigkeit geprüft und jeweilige Fehler ausgegeben
 * Danach werden in einer Schleife Clients angenommen. Der Clienthandler wird dann erstellt, welcher dann
 * konkret die Anfragen des Clients abarbeitet
 */

int main(int argc, char **argv) {
    int PORT;
    char* FOLDER;
    int create_socket, new_socket;
    socklen_t addrlen;
    struct sockaddr_in address, cliaddress;
    //checking arguments:
    if (argc < 3) {
        printf("Usage: %s Port Verzeichnispfad\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (isDigit(argv[1])) {
        PORT = atoi(argv[1]);
    } else {
        printf("Fehler beim interpretieren des Ports -> Ungültige Nummer!\n");
        return EXIT_FAILURE;
    }

    if (dirExists(argv[2])) {
        FOLDER = argv[2];
    } else {
        printf("Der angegebene Ordner '%s' existiert nicht!\n", argv[2]);
        return EXIT_FAILURE;
    }

    create_socket = socket(AF_INET, SOCK_STREAM, 0);

    memset(&address, 0, sizeof (address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(create_socket, (struct sockaddr *) &address, sizeof (address)) != 0) {
        perror("Fehler beim Verbindungsaufbau");
        return EXIT_FAILURE;
    }
    listen(create_socket, 5);

    addrlen = sizeof (struct sockaddr_in);
    printf("Server wurde gestartet\n");

    while (1) {
        new_socket = accept(create_socket, (struct sockaddr *) &cliaddress, &addrlen);
        if (new_socket > 0) {
            ClientHandler *clientHandler = new ClientHandler(new_socket, FOLDER);
            printf("Ein Client hat sich verbunden!\n");
            clientHandler->handleClient();
        }
    }
    close(create_socket);
    printf("Server wird beendet\n");
    return EXIT_SUCCESS;
}

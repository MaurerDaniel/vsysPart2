#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include "protocol.h"
#include "Functions.h"

#define BUF 1024


int PORT;

//client


int main(int argc, char **argv) {
    int create_socket;
    char buffer[BUF];
    struct sockaddr_in address;
    int size;
    char *serverAdress;
    char operation = 0;
    int num;


    if (argc < 3) {
        printf("Usage: %s IP-Adresse Port\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (isDigit(argv[2])) {
        PORT = atoi(argv[1]);
    } else {
        printf("Fehler beim interpretieren des Ports -> Ungültige Nummer!\n");
        return EXIT_FAILURE;
    }

    PORT = atoi(argv[2]);
    serverAdress = std::strcat(argv[1], ":");
    serverAdress = std::strcat(serverAdress, argv[2]);

    printf("%s", serverAdress);

    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket fehler");
        return EXIT_FAILURE;
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    inet_aton(serverAdress, &address.sin_addr);

    if (connect(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0) {
        printf("Verbindung zum Server (%s) wurde hergestellt\n", inet_ntoa(address.sin_addr));
        size = recv(create_socket, buffer, BUF - 1, 0);
        if (size > 0) {
            buffer[size] = '\0';
            printf("%s", buffer);
        }
    } else {
        perror("Verbindungsfehler - Kein Server gefunden!");
        return EXIT_FAILURE;
    }

    //Hauptmenü
    do {

        std::cout << "Option wählen:\n"
                     "1 - SEND\n"
                     "2 - LIST\n"
                     "3 - READ\n"
                     "4 - DEL\n"
                     "5 - QUIT" << std::endl;

        if (!(std::cin >> operation) || operation == -1) {
            operation = 0;

        };


        switch (operation) {

            case '1': {
                SEND *msg = new SEND;
                msg->OUT(create_socket);

                if (!checkErr(create_socket)) {
                    std::cout << "Sende erneut..." << std::endl;
                    msg->OUT(create_socket);
                }
                memset(buffer, 0, BUF);
                msg->~SEND();
                break;
            }


            case '2': {

                LIST *list = new LIST;
                list->OUT(create_socket);
                memset(buffer, 0, BUF);
                readline(create_socket, buffer, BUF);
                num = std::stoi(buffer);
                std::cout << "\nAnzahl Nachrichten:" << num << std::endl;

                for (int i = 0; i < num; i++) {
                    readline(create_socket, buffer, BUF);
                    std::cout << "Nachricht: " << buffer;
                }
                std::cout << std::endl;

                list->~LIST();
                break;
            }


            case '3': {

                READ *read = new READ;
                read->OUT(create_socket);
                memset(buffer, 0, BUF);
                std::cout << "Nachricht: " << "\n" << std::endl;
                if (!checkErr(create_socket)) {
                    break;
                }
                while (strncmp(buffer, ".\n", 1) != 0) {
                    readline(create_socket, buffer, BUF);

                    std::string line(buffer);
                    std::cout << line;
                }
                std::cout << std::endl;
                read->~READ();
                break;
            }


            case '4': {

                DEL *del = new DEL;
                del->OUT(create_socket);

                if (!checkErr(create_socket)) {
                    break;
                }

                memset(buffer, 0, BUF);
                del->~DEL();
                break;
            }


            case '5': {
                writen(create_socket, "QUIT", 5);
                close(create_socket);
                checkErr(create_socket);
                memset(buffer, 0, BUF);
                return EXIT_SUCCESS;
            }


            default:
                std::cout << "Keine valide Option!" << std::endl;

                break;
        }


    } while (strcmp(buffer, "quit\n") != 0);
    close(create_socket);
    return EXIT_SUCCESS;
}
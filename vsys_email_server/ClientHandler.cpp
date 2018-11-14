/* 
 * File:   FileOperations.h
 * Author: acs
 *
 * Created on October 24, 2018, 2:21 PM
 */

#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iterator>
#include <locale> 
#include <fstream>

#include "ClientHandler.h"
#include "Functions.h"
#include "MessageType.h"

#define MAXLINE 1500
#ifndef VSYS_EMAIL_SERVER_CLIENTHANDLER_H
#define VSYS_EMAIL_SERVER_CLIENTHANDLER_H

/**
 * Konstruktor für den Client Handler
 * @param socket
 * @param folder angegebenes Arbeitsverzeichnis zum behandeln von Userdaten
 */
ClientHandler::ClientHandler(int socket, char* folder) {
    this->socket = socket;
    // this->folder = folder;
    std::string dir(folder);
    this->fileHandler.setRootDirectory(dir);
}

/**
 * Funktion zum behandeln der Anfragen eines Clients.
 * Es wird Zeilenweise gelesen. Mit Hilfe der isMessageType Variable wird
 * erkannt dass die erste Zeile ein Nachrichtentyp ist. Die Variable wird dann auf false gesetzt, und erst
 * dann wieder auf true wenn die Nachricht vollständig gelesen wurde (je nach Nachrichtentyp ändert sich die
 * erwartete Anzahl von Zeilen). Die gelesene Nachricht wird dann inkl. MessageType in die handleRequest Methode übergeben.
 */
void ClientHandler::handleClient() {
    writen(WELCOME_MESSAGE, strlen(WELCOME_MESSAGE));
    char buffer[BUF]={0};
    std::list<std::string> lines;
    bool isMessageType = true;
    int messageLinesLeft = 0;
    MessageType messageType;
    int size;
    do {
        memset(buffer, 0, BUF);
        readline(buffer, BUF);
        size = strlen(buffer);
        if (size > 0) {
            buffer[size] = '\0';
            std::string line(buffer);
            line.erase(line.length()-1);
            if (isMessageType) {
                isMessageType = false; //will stay false until complete message is read
                if (isEqual(line, "send")) {
                    messageType = SEND;
                } else if (isEqual(line, "read")) {
                    messageType = READ;
                    messageLinesLeft = 2;
                } else if (isEqual(line, "del")) {
                    messageType = DEL;
                     messageLinesLeft = 2;
                } else if (isEqual(line, "list")) {
                    messageType = LIST;
                     messageLinesLeft = 1;
                } else if (!isEqual(line, "quit")) {

                }
            } else { //read message line:
                if (messageType == SEND) {
                    if (strncmp(buffer, ".\n", 1) == 0) {
                        lines.push_back(buffer);
                        handleRequest(lines, messageType);
                        lines.clear();
                        isMessageType = true;
                    } else {
                        lines.push_back(line.c_str());
                    }
                } else{
                    lines.push_back(line.c_str());
                    if (--messageLinesLeft == 0){
                        handleRequest(lines, messageType);
                        lines.clear();
                        isMessageType = true;
                    }
                }
            }
        } else if (size == 0) {
            lines.clear();
            break;
        } else {
            printf("%s", QUIT_MESSAGE);
            return;
        }
    } while (!isEqual(buffer, "quit"));
    printf("%s", SHUTDOWN_MESSAGE);
    lines.clear();
    close(socket);
    return;
}

/**
 * Die gelesene nachricht wird an den fileHandler gesendet der eine Response zurückschickt
 * diese wird dann Zeilenweise ausgegeben
 * @param messageLines
 * @param type
 */
void ClientHandler::handleRequest(std::list<std::string> messageLines, MessageType type) {
    std::string messageline;
    std::list<std::string> response = fileHandler.handleRequest(messageLines, type);
    for (int index = 0; index < response.size(); index++){
        messageline = getMessageLine(index, response);
        writen(messageline.data(), strlen(messageline.data()));
    }
}

// write n bytes to a descriptor ...

ssize_t ClientHandler::writen(const char *vptr, size_t n) {
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;
    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(socket, ptr, nleft)) <= 0) {
            if (errno == EINTR)
                nwritten = 0; // and call write() again
            else
                return (-1);
        }
        nleft -= nwritten;
        ptr += nwritten;
    };
    return (n);
};

ssize_t ClientHandler::my_read(char *ptr) {
    static int read_cnt = 0;
    static char *read_ptr;
    static char read_buf[MAXLINE];
    if (read_cnt <= 0) {
again:
        if ((read_cnt = read(socket, read_buf, sizeof (read_buf))) < 0) {
            if (errno == EINTR)
                goto again;
            return (-1);
        } else if (read_cnt == 0)
            return (0);
        read_ptr = read_buf;
    };
    read_cnt--;
    *ptr = *read_ptr++;
    return (1);
}

ssize_t ClientHandler::readline(char *vptr, size_t maxlen) {
    ssize_t n, rc;
    char c, *ptr;
    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ((rc = my_read(&c)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break; // newline is stored
        } else if (rc == 0) {
            if (n == 1)
                return (0); // EOF, no data read
            else
                break; // EOF, some data was read
        } else
            return (-1); // error, errno set by read() in my_read()
    };
    *ptr = 0; // null terminate
    return (n);
}

#endif
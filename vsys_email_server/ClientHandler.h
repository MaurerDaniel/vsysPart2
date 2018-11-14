/* 
 * File:   FileOperations.h
 * Author: acs
 *
 * Created on October 24, 2018, 2:21 PM
 */
#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <list>
#include <string>

#include "FileHandler.h"
#include "MessageType.h"


class ClientHandler {
   
    
public:
    ClientHandler(int socket, char* folder);
    void handleClient();
    
private:
    int socket;
    #define BUF 1024
    const char *WELCOME_MESSAGE = "Verbindung zum Server wurde hergestellt!\n\0";
    const char *QUIT_MESSAGE = "Verbindung zum Client wurde getrennt!\n\0";
    const char *SHUTDOWN_MESSAGE = "Client wurde beendet!\n\0";
    FileHandler fileHandler;
    ssize_t writen (const char *vptr, size_t n);
    ssize_t my_read(char *ptr);
    ssize_t readline(char *vptr, size_t maxlen);
    void handleRequest(std::list<std::string> messageLines, MessageType type);
};

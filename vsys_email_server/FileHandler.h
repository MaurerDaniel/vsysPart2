
/* 
 * File:   FileHandler.h
 * Author: acs
 *
 * Created on October 29, 2018, 1:25 PM
 */

#include <cstdio>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <list>
#include <string>
#include "Functions.h"
#include "MessageType.h"

#ifndef FILEHANDLER_H
#define FILEHANDLER_H

class FileHandler {
public:
    virtual ~FileHandler();
    FileHandler();
    FileHandler(std::string root);
    void setRootDirectory(std::string root);
    std::list<std::string> handleRequest(std::list<std::string> message, MessageType type);

private:
    std::string rootDir;
    const char* OK = "OK\n";
    const char* ERR = "ERR\n";
    const std::string CURRENT_ID_FILENAME = "/currentID.txt";
    int createUserDir(std::string userFolder);
    std::string getUserFolder(std::string user);
    std::list<std::string> safeMessage(std::list<std::string> message, std::string userFolder);
    int getNextMessageID(std::string userFolder);
    void incrementMessageID(std::string userFolder, int current);
    std::string getMessageFilePath(std::string userFolder, int id);
    std::list<std::string>  sendMessage(std::list<std::string> message);
    std::list<std::string>  readMessage(std::list<std::string> message);
    std::list<std::string> listMessages(std::list<std::string> message);
    std::list<std::string>  delMessage(std::list<std::string> message);
    std::string getMessageSubject(std::string path, std::string messageID);
};

#endif /* FILEHANDLER_H */


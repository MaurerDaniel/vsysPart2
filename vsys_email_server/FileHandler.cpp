
/* 
 * File:   FileHandler.cpp
 * Author: acs
 * 
 * Created on October 29, 2018, 1:25 PM
 */

#include "FileHandler.h"

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
#include <iterator>
#include <locale> 
#include <fstream>
#include <dirent.h>
#include <sstream>

#include "MessageType.h"
#include "Functions.h"


FileHandler::~FileHandler() {
}

FileHandler::FileHandler(){
    
}

/**
 * KOnstruktor für den FileHandler
 */
FileHandler::FileHandler(std::string root){
    rootDir = root;
}

/**
 * Setter für das root Directory
 * @param root
 */
void FileHandler::setRootDirectory(std::string root) {
    this->rootDir = root;
}


/**
 * Erstellt anhand vom gewünschten Pfad zu einem User-Directory den jeweiligen Folder.
 * @param userFolder
 * @return 1 bei erstellen -1 bei fehler
 */
bool FileHandler::createUserDir(std::string userFolder) {
    const int dir_err = mkdir(userFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (-1 == dir_err) {
        return false;
    }
    userFolder.append(CURRENT_ID_FILENAME);
    std::ofstream o(userFolder);

    o << "0\n" << std::endl;
    o.close();
    return true;
}

/**
 * Erstellt anhand vom usernamen und dem gegebenen root directory den pfad zum userfolder
 * @param user
 * @return
 */
std::string FileHandler::getUserFolder(std::string user) {
    std::string newFolderPath(rootDir);
    newFolderPath.append("/");
    newFolderPath.append(user);
    return newFolderPath;
}

/**
 * Erstellt anhand vom UserFolder-path und der id einer Nachricht den vollständigen Pfad zu dieser Nachricht
 * beispiel /path/to/root/user/0.txt
 * @param userFolder
 * @param id
 * @return
 */
std::string FileHandler::getMessageFilePath(std::string userFolder, int id){
    std::string filename = userFolder;
    filename.append("/");
    filename.append(std::to_string(id));
    filename.append(".txt");
    return filename;

}

/**
 * Speichert eine gesendete Nachricht im konkreten UserOrdner
 * @param message
 * @param userFolder
 * @return
 */
std::list<std::string> FileHandler::safeMessage(std::list<std::string> message, std::string userFolder){
    std::list<std::string> response;
    int currentID = getNextMessageID(userFolder);
    if (currentID == -1){ //id could not be read
        response.push_back(ERR);
        return response;
    }
    std::ofstream o(getMessageFilePath(userFolder, currentID));
    for (int index = 0; index < message.size(); index++){
            o << getMessageLine(index, message) << std::endl;
    }
    o.close();
    incrementMessageID(userFolder, currentID);
    response.push_back(OK);
    return response;
}

/**
 * Liest aus dem jeweiligen User Ordner die nächste ID für eine neue Nachricht
 * @param userFolder
 * @return
 */
int FileHandler::getNextMessageID(std::string userFolder){
    userFolder.append(CURRENT_ID_FILENAME);
    std::ifstream currentID (userFolder);
    std::string line;
    if (currentID.is_open()){
        getline(currentID, line);
        if (isDigit(line)){
            currentID.close();
            return stoi(line);
        } else{
            currentID.close();
            return -1;
        }
    } else{
        return -1; //could not be opened
    }
}


/**
 * Erhöht die nächste id für eine neue message (nach dem senden)
 * Dies passiert nicht direkt beim lesen der nächsten ID falls ein
 * fehler beim Senden auftritt und die id somit nicht genutzt wurde
 * @param userFolder
 * @param current
 */
void FileHandler::incrementMessageID(std::string userFolder, int current){
    userFolder.append(CURRENT_ID_FILENAME);
    std::ofstream o(userFolder);
    o << ++current << "\n" << std::endl;
    o.close();
}

/**
 * prüft ob der ordner für den benutzer existiert, legt ihn an wenn nicht und
 * ruft das safeMessage auf
 * @param message
 * @return
 */
std::list<std::string>  FileHandler::sendMessage(std::list<std::string> message) {
    std::list<std::string> response;
    std::string user = getMessageLine(1, message);
    std::string userFolder = getUserFolder(user);
    if (dirExists(userFolder)) {
        return safeMessage(message, userFolder);
    } else {
        if (createUserDir(userFolder) == 1) {
            printf("User-Ordner '%s' wurde angelegt\n", userFolder.c_str());
            return safeMessage(message, userFolder);
        } else {
            printf("User-Ordner '%s' konnte nicht angelegt werden\n", userFolder.c_str());
            response.push_back(ERR);
            return response;
        }
    }
}

/**
 * Löscht die jewelige Nachricht
 *
 * @param message
 * @return ERR bei fehlerhafter eingabe
 */
std::list<std::string>  FileHandler::delMessage(std::list<std::string> message){
    std::list<std::string> response;
    std::string userFolder = getUserFolder(getMessageLine(0, message));
    std::string messageNumber = getMessageLine(1, message);
    if (isDigit(messageNumber)){
        if (remove(getMessageFilePath(userFolder, stoi(messageNumber)).data()) == 0){
            response.push_back(OK);
        } else{
            response.push_back(ERR);
        }
    } else{
        response.push_back(ERR);
    }
    return response;
}

/**
 * Liest die jeweilige Nachricht aus und schreibt sie zeilenweise in die response
 * @param message
 * @return
 */
std::list<std::string> FileHandler::readMessage(std::list<std::string> message){
    std::list<std::string> response;
    std::string userFolder = getUserFolder(getMessageLine(0, message));
    std::string messageID = getMessageLine(1, message);
    if (dirExists(userFolder)){
        std::string filename = userFolder;
        if (isDigit(messageID)){
            filename.append("/");
            filename.append(messageID);
            filename.append(".txt");
            std::ifstream messageFile (filename);
            std::string line;
            if (messageFile.is_open()){
                response.push_back(OK);
                std::string line;
                while (std::getline(messageFile, line))
                {
                    if (!line.empty()){
                        line.append("\n");
                        response.push_back(line);
                    } else{
                        break;
                    }

                }
                messageFile.close();
            } else{
                response.push_back(ERR);
            }
        } else{
            response.push_back(ERR);
        }
    } else{
        response.push_back(ERR);
    }
    return response;
}

/**
 * Liest aus der jeweligen Nachricht den Betreff aus (für list)
 */
std::string FileHandler::getMessageSubject(std::string path, std::string messageID){
    std::string subject;
    int counter = 0;
    path.append("/");
    path.append(messageID);
    path.append(".txt");
    std::ifstream messageFile (path);
    std::string line;
    if (messageFile.is_open()){
        std::string line;
        while (std::getline(messageFile, line))
        {
            if (!line.empty()){
                if (counter++ == 2){
                    return line;
                }
            } else{
                break;
            }

        }
        messageFile.close();
    }
    return "error";//sollte nie passieren, da vor aufruf geprüft wird ob file existiert
}

/**
 * Gibt eine Liste aller Nachrichten inkl. Betreff und Gesamtanzahl zurück
 */
std::list<std::string> FileHandler::listMessages(std::list<std::string> message){
    std::list<std::string> response;
    std::string temp;
    std::string filename;
    std::string numberOfFilesStr;
    int numberOfFiles = 0;
    std::string userFolder = getUserFolder(getMessageLine(0, message));
    std::shared_ptr<DIR> directory_ptr(opendir(userFolder.c_str()), [](DIR* dir){ dir && closedir(dir); });
    struct dirent *dirent_ptr;
    if (!directory_ptr) {
        response.push_back("0\n");
        return response;
    }
    while ((dirent_ptr = readdir(directory_ptr.get())) != nullptr) {
        temp = dirent_ptr->d_name;
        if (!isEqual(temp, ".") && !isEqual(temp, "..") && !isEqual(temp, "currentID.txt")){
            if (temp.find(".txt") != std::string::npos) {
                temp.resize(temp.size()-4);
                if (isDigit(temp)){
                    filename = temp;
                    filename.append(" -> ");
                    filename.append(getMessageSubject(userFolder, temp));
                    filename.append("\n");
                    response.push_back(filename);
                    numberOfFiles++;
                }
            }
        }
    }
    numberOfFilesStr = std::to_string(numberOfFiles);
    numberOfFilesStr.append("\n");
    response.push_front(numberOfFilesStr);
    return response;
}

/**
 * Bearbeitet eine Anfrage und ruft je nach Anfrage die jeweilige Funktion dafür auf und
 * schickt die Response als Liste zurück
 */
std::list<std::string> FileHandler::handleRequest(std::list<std::string> message, MessageType type){
    std::list<std::string> response;
    if (type == SEND){
        return sendMessage(message);
    } else if (type == DEL){
       return delMessage(message);
    } else if (type == LIST){
        return listMessages(message);
    }else if (type == READ){
        return readMessage(message);
    }
    response.push_back(ERR);
    return response;
}



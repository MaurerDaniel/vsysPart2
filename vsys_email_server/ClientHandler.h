/*
 * File:   FileOperations.h
 * Author: acs
 *
 * Created on October 24, 2018, 2:21 PM
 */
#include <cstdio>
#include <iostream>
 //#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <list>
#include <string>
#include <map>
#include <set>
#include <vector>
#include "FileHandler.h"
#include "MessageType.h"

using namespace std;

class sockenSchnuffler {

public:
	string readLine(int fd, bool& success);

private:
	ssize_t myRead(int fd, char *ptr);

};
class Server {

	struct bannedClient
	{
		long untilBanned = 0;
		int tries = 0;
	};



public:
	Server(string prgName, int srvrPort, string mailDirect);
	~Server();
	void acceptConnection(int& clientSocket, string& clientIP);
	void sendMessage(int clientSocket, string message);
	void split(string str, char deliimiter);
	void beginListen();
	void extConnect(int cSocket);


private:
	socklen_t addrlen;
	map<string, bannedClient> bannedList;
	string progName;
	int srvrSocket;

};

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
	ssize_t writen(const char *vptr, size_t n);
	ssize_t my_read(char *ptr);
	ssize_t readline(char *vptr, size_t maxlen);
	void handleRequest(std::list<std::string> messageLines, MessageType type);



};

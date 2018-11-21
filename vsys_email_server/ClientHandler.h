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
#include <ldap.h>
#include <mutex>
#include <vector> 

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
	bool acceptConnection(int& cSocket, string& cIP);
	void sendMessage(int cSocket, string message);
	vector<string> split(string str, char delimiter);
	void beginListen();
	void extConnect(int cSocket);
	string recMessage (int cSocket, bool& isGoodBoi, sockenSchnuffler& socketReader);
	// copied from other group
	bool createUserDir(std::string userFolder);
	void SEND(int cSocket, string loggedUs, sockenSchnuffler& socketReader);
	void LIST(int cSocket, string loggedUs);
	void READ(int cSocket, string loggedUs, sockenSchnuffler& socketReader);
	void DEL(int cSocket, string loggedUs, sockenSchnuffler& socketReader);
	string getUUID();
	mutex termMtx, globVarMtx;
	long timeNow(string unit);

	bool storeMail(string sender, string receivers, string subject, string message);
	map<string, string> getMailSubjects(string receiver);
	vector<string> getMailFilenames(string receiver);
	set<string> parseReceivers(string receivers);



private:
	socklen_t addrlen;
	map<string, bannedClient> bannedList;
	string progName;
	int srvrSocket;
	string mailDirect;
	mutex direcMtx, fileMtx;

};

class helferLDAP {

	#define LDAP_URI "ldap://ldap.technikum-wien.at:389"
	#define SEARCHBASE "dc=technikum-wien,dc=at"
	#define SCOPE LDAP_SCOPE_SUBTREE
	#define ANONYMOUS_USER "" // uid=if17b125,ou=People,dc=technikum-wien,dc=at
	#define ANONYMOUS_PW "" // plain text password

	bool login(string us, string pw);
	LDAP* init();
	bool bindUs(LDAP* ld, string usDN, string pw);
	void bindAnonym(LDAP* ld);
	string searchUs(LDAP* ld, string usDN);
	
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

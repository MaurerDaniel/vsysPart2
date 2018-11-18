/*
 * File:   FileOperations.h
 * Author: acs
 *
 * Created on October 24, 2018, 2:21 PM
 */
#include <chrono> // duration_cast, seconds, milliseconds, system_clock::now()
#include <sstream> // istringstream
#include <sys/stat.h> // mkdir(), RWX macros
#include <unistd.h> // read()
#include <termios.h> // struct termios, tcgetattr(), tcsetattr(), ECHO, TCSANOW
#include <uuid/uuid.h> // uuid_t, uuid_generate(), uuid_unparse()
#include <iostream>
#include <arpa/inet.h> // inet_ntoa()
#include <unistd.h> // close()
#include <string.h> // memset(), strcpy(), strlen(), strncmp()
#include <fstream> // ifstream, ofstream
#include <dirent.h> // DIR, opendir(), readdir(), closedir()
#include <algorithm> // sort()
#include <sstream> // stringstream
#include <mutex> // mutex
#include <iostream>
#include "server.h"
#include "../Helper/helper.h"
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
	char buffer[BUF] = { 0 };
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
			line.erase(line.length() - 1);
			if (isMessageType) {
				isMessageType = false; //will stay false until complete message is read
				if (isEqual(line, "send")) {
					messageType = SEND;
				}
				else if (isEqual(line, "read")) {
					messageType = READ;
					messageLinesLeft = 2;
				}
				else if (isEqual(line, "del")) {
					messageType = DEL;
					messageLinesLeft = 2;
				}
				else if (isEqual(line, "list")) {
					messageType = LIST;
					messageLinesLeft = 1;
				}
				else if (!isEqual(line, "quit")) {

				}
			}
			else { //read message line:
				if (messageType == SEND) {
					if (strncmp(buffer, ".\n", 1) == 0) {
						lines.push_back(buffer);
						handleRequest(lines, messageType);
						lines.clear();
						isMessageType = true;
					}
					else {
						lines.push_back(line.c_str());
					}
				}
				else {
					lines.push_back(line.c_str());
					if (--messageLinesLeft == 0) {
						handleRequest(lines, messageType);
						lines.clear();
						isMessageType = true;
					}
				}
			}
		}
		else if (size == 0) {
			lines.clear();
			break;
		}
		else {
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
	for (int index = 0; index < response.size(); index++) {
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
		if ((read_cnt = read(socket, read_buf, sizeof(read_buf))) < 0) {
			if (errno == EINTR)
				goto again;
			return (-1);
		}
		else if (read_cnt == 0)
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
		}
		else if (rc == 0) {
			if (n == 1)
				return (0); // EOF, no data read
			else
				break; // EOF, some data was read
		}
		else
			return (-1); // error, errno set by read() in my_read()
	};
	*ptr = 0; // null terminate
	return (n);
}

Server::Server(string progName, int srvrPort, string mailDirect)
{
	struct sockaddr_in srvrAddr;

	this->progName = progName;
	this->addlren = sizeof(struct sockaddr_in);
	this->mailDirect = mailDirect;
	// mail directory erstellen
	bool good = FileHandler::createUserDir(mailDirect);
	if (!good) return EXIT_FAILURE;

	memset(&srvrAddr, 0, sizeof(srvrAddr));
	srvrAddr.sin_family = AF_INET;
	srvrAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 0
	srvrAddr.sin_port = htons(srvrPort);

	this->srvrSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->srvrSocket == -1)
	{
		perror("Fehler beim erstellen von Socket.");
		return EXIT_FAILURE;
	}
	int bindRtrn = bind(this->srvrSocket, (struct sockaddr*) &srvrAddr, sizeof(srvrAddr));
	if (bindRtrn != 0)
	{
		perror("Fehler beim bind von Socket.");
		return EXIT_FAILURE;
	}
	cout << "Port: " << srvrPort << " Directory: \"" << mailDirect << "\"" << "." << endl;

}
// listener
void Server::beginListen()
{

	cout << "Ich höre.." << endl;
	int listnRet = listen(this->serverSocket, 5);

	if (listnRet != 0)
	{
		perror("Fehler beim zuhören.");
		return EXIT_FAILURE;
	}
}

// accept connetions to client 
bool Server::acceptConnection(int& clientSocket, string&clientIP)
{
	struct sockaddr_in clientAdrr;

	clientSocket = accept(this->serverSocket, (struct sockaddr *) & clientAdrr, this->&addrlen);

	if (clientSocket > 0)
	{
		clientIP = inet_ntoa(clientAdrr.sin_addr);
		int clientPort = ntohs(clientAdrr.sin_port);

		cout << "Verbindungsaufbau von: " << clientIP << " : " << clientPort << "." << endl;
		//logik für gebannte user
		if (bannedList.find(clientIP) != bannedList.end())
		{
			// bekommt die derzeitige Zeit in sekunden
			long now = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count();
			long diffTime = bannedList[clientIP].untilBanned - now;

			if (diffTime > 0)
			{
				cout << "Client mit der IP: " << clientIP << " - ist noch für weitere: " << diffTime << " Sekunden gesperrt." << endl;

				//wurde nicht akzeptiert 
				return false;
			}

			else if (bannedList[clientIP].tries >= 3)
			{
				this->bannedList.erase(clientIP);
				cout << "Sperre wurde aufgehoben.  Client: " << clientIP << endl;
			}
		}
		// klinet akepztiert
		this->sendMessage(clientSocket, "Halli hallo. Bitte geben Sie Ihren Wunsch an:")
	}
}

// nachricht/antowrt wird geschickt
void Server::sendMessage(int clientScoket, string msg)
{
	vector <string> zeilen = split(msg, '\n');

	for (int i = 0; i < (int)lines.size(), i++)
	{
		string zeile = zeile[i] + "\n";
		ssize_t charGsnd = send(clientScoket, zeile.c_str(), zeile.length(), MSG_NOTSIGNAL);

		if (charGsnd == -1)
		{
			if (errno == EPIPE)
			{
				cerr << this->progName << ": Fehler beim Senden. Client hat Socket geschlossen" << endl;
			}
		}
		else
		{
			perror("Fehler beim Verbindungsaufbau");
		}
		return EXIT_FAILURE;
	}
}


// taken from //www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
void Server::split(string str, char deliimiter)
{
	vector<string> tokens;
	string token;
	istringstream tokenStream(str);

	while (getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

void Server::extConnect(int cSocket) {
	{
		cout << "CSchließe Socket mit Client " << cSocket << "." << endl;
	}
	int closeReturn = close(cSocket);

	if (closeReturn != 0)
	{
		perror("Fehler beim Schließen.");
		return EXIT_FAILURE;
	}
}




/**
 * Adapted from tcpip_linux-prog-details.pdf:
 * Read a '\n' terminated line from a descriptor, char by char.
 */
string sockenSchnuffler::readLine(int fd, bool& success) {
	ssize_t rc;
	char c;
	string line = "";

	while (1) {
		rc = SocketReader::myRead(fd, &c);

		if (rc == 1) {        // a char was read
			if (c == '\n') {
				break;            // '\n' is found, break out of loop
			}
			else {
				line += c;        // add char to line (do not add '\n')
			}
		}
		else if (rc == 0) {   // no char was read, EOF, break out of loop
			break;
		}
		else {
			success = false;
			return "";          // error, errno set by read() in myRead() 
		}
	}

	success = true;
	return line;
}

/**
 * Taken from tcpip_linux-prog-details.pdf:
 * Implementation of read() with internal buffer.
 */
ssize_t sockenSchnuffler::myRead(int fd, char *ptr) {
	static int   read_cnt = 0;
	static char  *read_ptr;
	static char  read_buf[BUF];
	if (read_cnt <= 0) {
	again:
		if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) { // < 0 error
			if (errno == EINTR)     // interrupt signal
				goto again;
			return (-1);
		}
		else if (read_cnt == 0) // 0 bytes read
			return (0);
		read_ptr = read_buf;     // no error
	};
	read_cnt--;
	*ptr = *read_ptr++;
	return (1);
}


#endif
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
#include <chrono> // duration_cast, seconds, milliseconds, system_clock::now()
#include <sstream> // istringstream
#include <sys/stat.h> // mkdir(), RWX macros
#include <unistd.h> // read()
#include <termios.h> // struct termios, tcgetattr(), tcsetattr(), ECHO, TCSANOW
#include <uuid/uuid.h> // uuid_t, uuid_generate(), uuid_unparse()
#include <iostream>
#include <signal.h> 
#include <thread>

#include "ClientHandler.h"
#include "Functions.h"

using namespace std;

#define BUF 1024



/**
 * In der Main Funktion werden die Eingangsparameter auf gültigkeit geprüft und jeweilige Fehler ausgegeben
 * Danach werden in einer Schleife Clients angenommen. Der Clienthandler wird dann erstellt, welcher dann
 * konkret die Anfragen des Clients abarbeitet
 */
Server* mServer = nullptr;
string progName;
set<int> goodBois;


void putze();
void hanldeIncoming(int boi);
void CheckArgs(int argc, char* argv[], int& srvrPort, string& mailDirect);
void threadSwitch(int cSocket, string cIP);


int main(int argc, char **argv) {
	//int PORT;
	//char* FOLDER;
	//int create_socket, new_socket;
	//socklen_t addrlen;
	//struct sockaddr_in address, cliaddress;
	////checking arguments:
	//if (argc < 3) {
	//	printf("Usage: %s Port Verzeichnispfad\n", argv[0]);
	//	exit(EXIT_FAILURE);
	//}

	//if (isDigit(argv[1])) {
	//	PORT = atoi(argv[1]);
	//}
	//else {
	//	printf("Fehler beim interpretieren des Ports -> Ungültige Nummer!\n");
	//	return EXIT_FAILURE;
	//}

	//if (dirExists(argv[2])) {
	//	FOLDER = argv[2];
	//}
	//else {
	//	printf("Der angegebene Ordner '%s' existiert nicht!\n", argv[2]);
	//	return EXIT_FAILURE;
	//}


	//create_socket = socket(AF_INET, SOCK_STREAM, 0);

	//memset(&address, 0, sizeof(address));
	//address.sin_family = AF_INET;
	//address.sin_addr.s_addr = INADDR_ANY;
	//address.sin_port = htons(PORT);

	//if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) != 0) {
	//	perror("Fehler beim Verbindungsaufbau");
	//	return EXIT_FAILURE;
	//}
	//listen(create_socket, 5);

	//addrlen = sizeof(struct sockaddr_in);
	//printf("Server wurde gestartet\n");

	//while (1)
	//{

	//	
	//	new_socket = accept(create_socket, (struct sockaddr *) &cliaddress, &addrlen);
	//	if (new_socket > 0) {
	//		ClientHandler *clientHandler = new ClientHandler(new_socket, FOLDER);
	//		printf("Ein Client hat sich verbunden!\n");
	//		clientHandler->handleClient();
	//	}
	//}
	//close(create_socket);
	//printf("Server wird beendet\n");
	//return EXIT_SUCCESS;


	signal(SIGINT, &hanldeIncoming);
	int sport;
	string mailDirect;
	CheckArgs(argc, argv, sport, mailDirect);

	mServer = new Server(progName, sport, mailDirect);
	mServer->beginListen();
	while (1)
	{
		int cSocket;
		string cIP;
		bool goodClient = mServer->acceptConnection(cSocket, cIP);

		// wenn alles passt..
		if (goodClient)
		{
			//mutex afrufe sind da um mehrfachzugriff auf gleiche sachen zu blockierern oÄ.
			goodBois.insert(cSocket);
			thread thrd(threadSwitch, cSocket, cIP);
			thrd.detach();
		}
		else
		{
			mServer->extConnect(cSocket);
		}
	}
	putze();
	return EXIT_SUCCESS;
}

void putze() {
	{
		for (set<int>::iterator obi = goodBois.begin(); obi != goodBois.end(); ++obi) {
			mServer->extConnect(*obi);
		}
	}

	// close server socket, free ressources
	delete mServer;
}

void hanldeIncoming(int boi) {
	if (mServer != nullptr) {
		cout << endl;
		putze();
	}
	exit(EXIT_SUCCESS);
}

void CheckArgs(int argc, char* argv[], int& srvrPort, string& mailDirect)
{
	progName = argv[0];
	if (argc != 3)
	{
		cerr << "Verwendung: " << progName << " server_port mail_directory" << endl;
		exit(EXIT_FAILURE);
	}

	char* arg = argv[1];

	istringstream ss(arg);
	int argAsInt;

	if (!(ss >> argAsInt) || !ss.eof()) {
		cerr << "Invalid number: " << arg << endl;
		exit(EXIT_FAILURE);
	}

	srvrPort = argAsInt;
	mailDirect = argv[2];
}


void threadSwitch(int cSocket, string cIP) {
	bool isAGoodBoi;
	string msg, loggedUs;
	sockenSchnuffler sReader;

	do {
		msg = mServer->recMessage(cSocket, isAGoodBoi, sReader);

		if (isAGoodBoi) {
			cout << "Von " << cIP << " angemeldet als " << loggedUs<<" empfangene Nachricht:  " << msg << "\""  << endl;
			// mach was mit den kommands
			if (msg == "LOGIN") {
				//loggedUs = mServer->LOGIN(cSocket, cIP, sReader);
				if (loggedUs == "_BANNED_") break;
			}
			else if (msg == "SEND") {
				mServer->SEND(cSocket, loggedUs, sReader);
			}
			else if (msg == "LIST") {
				mServer->LIST(cSocket, loggedUs);
			}
			else if (msg == "READ") {
				mServer->READ(cSocket, loggedUs, sReader);
			}
			else if (msg == "DEL") {
				mServer->DEL(cSocket, loggedUs, sReader);
			}
			else if (msg != "QUIT") {
				cout << "Kenn ich nicht." << endl;
			}
		}
		else
		{
			cerr << progName << ": Fehler beim Empfangen." << endl;
		}
	} while (msg != "QUIT" && isAGoodBoi);

	// schließen und putzen
	mServer->extConnect(cSocket);
	{
		goodBois.erase(cSocket);
	}
}

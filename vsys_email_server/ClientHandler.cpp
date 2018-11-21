/*
 * File:   FileOperations.h
 * Author: acs
 *
 * Created on October 24, 2018, 2:21 PM
 */
#include <chrono> 
#include <sstream> 
#include <sys/stat.h> 
#include <unistd.h>
#include <termios.h> 
#include <uuid/uuid.h> 
#include <arpa/inet.h> 
#include <string.h> 
#include <fstream>
#include <dirent.h>
#include <algorithm> 
#include <mutex> 
#include <iostream>
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
#include <ldap.h>

#include "ClientHandler.h"
#include "Functions.h"
#include "MessageType.h"

using namespace std;

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
	this->addrlen = sizeof(struct sockaddr_in);
	this->mailDirect = mailDirect;
	// mail directory erstellen
	bool good = Server::createUserDir(mailDirect);
	if (!good) exit(EXIT_FAILURE);

	memset(&srvrAddr, 0, sizeof(srvrAddr));
	srvrAddr.sin_family = AF_INET;
	srvrAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 0
	srvrAddr.sin_port = htons(srvrPort);

	this->srvrSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->srvrSocket == -1)
	{
		perror("Fehler beim erstellen von Socket.");
		exit(EXIT_FAILURE);
	}
	int bindRtrn = bind(this->srvrSocket, (struct sockaddr*) &srvrAddr, sizeof(srvrAddr));
	if (bindRtrn != 0)
	{
		perror("Fehler beim bind von Socket.");
		exit(EXIT_FAILURE);
	}
	cout << "Port: " << srvrPort << " Directory: \"" << mailDirect << "\"" << "." << endl;

}

bool Server::createUserDir (string path) {
  vector<string> tokens = split(path, '/');
  string concatPath = "";

  for (int i = 0; i < (int) tokens.size(); i++) {
    if (i == 0 && tokens[i] == "") {
      concatPath += "/";
      continue;
    }

    concatPath += tokens[i] + "/";
    int status = mkdir(concatPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    if (status == 0) {
      cout << "Ordner-Struktur " << concatPath << " wurde erfolgreich erstellt." << endl;
    }
    else if (errno != EEXIST) {
      // catch EEXIST: directory already exists, no need for creating
      cerr << "mkdir (" << concatPath << ") error: " << errno << endl;
      return false;
    }
  }

  return true;
}

// listener
void Server::beginListen()
{

	cout << "Ich höre.." << endl;
	int listnRet = listen(this->srvrSocket, 5);

	if (listnRet != 0)
	{
		perror("Fehler beim zuhören.");
		exit(EXIT_FAILURE);
	}
}

// accept connetions to client 
bool Server::acceptConnection(int& clientSocket, string&clientIP)
{
	struct sockaddr_in clientAdrr;

	clientSocket = accept(this->srvrSocket, (struct sockaddr *) & clientAdrr, &this->addrlen);

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
		this->sendMessage(clientSocket, "Halli hallo. Bitte geben Sie Ihren Wunsch an:");
	}
}

string Server::recMessage (int cSocket, bool& isGoodBoi, sockenSchnuffler& socketReader)
{
	bool good;
	string zeile = socketReader.readLine(cSocket, good);
	if(good)
	{
		if(!zeile.empty()) 
		{
			isGoodBoi = true;
			return zeile;
		}
	}
	else
	{
		perror("Fehler beim Lesen");
		exit(EXIT_FAILURE);
	}
	isGoodBoi = false;
	return "";
}

// nachricht/antowrt wird geschickt
void Server::sendMessage(int clientScoket, string msg)
{
	vector <string> zeilen = split(msg, '\n');

	for (int i = 0; i < (int)zeilen.size(); i++)
	{
		string zeile = zeile[i] + "\n";
		ssize_t charGsnd = send(clientScoket, zeile.c_str(), zeile.length(), MSG_NOSIGNAL);

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
		exit(EXIT_FAILURE);
	}
}


// taken from //www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
vector<string> Server::split(string str, char delimiter)
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
		exit(EXIT_FAILURE);
	}
}

/**
 * Takes care of the SEND command from the client. copied
 */
void Server::SEND(int clientSocket, string loggedInUser, sockenSchnuffler& socketReader) {
	bool isClientActive;
	string receivers = this->recMessage(clientSocket, isClientActive, socketReader);
	string subject = this->recMessage(clientSocket, isClientActive, socketReader);
	string message = this->recMessage(clientSocket, isClientActive, socketReader); // always receive first message line
	string currentLine;

	do {
		currentLine = this->recMessage(clientSocket, isClientActive, socketReader);

		if (currentLine != ".") {
			message += "\n" + currentLine;
		}
	} while (currentLine != "." && isClientActive);

	// if client socket not closed
	if (isClientActive) {
		// if user is logged in
		if (loggedInUser != "") {
			set<string> receiversSet = this->parseReceivers(receivers);
			bool errorFlag = false;

			// check length of subject
			if (receiversSet.size() == 0 || subject.length() > 80) {
				{
					lock_guard<mutex> terminalGuard(this->termMtx);
					cout << "No receivers or subject length exceeded." << endl;
				}
				errorFlag = true;
			}
			else {
				bool storeSuccess = this->storeMail(loggedInUser, receivers, subject, message);

				if (!storeSuccess) {
					errorFlag = true;
				}
			}

			this->sendMessage(clientSocket, errorFlag ? "ERR" : "OK");
		}
		else {
			this->sendMessage(clientSocket, "LOGIN REQUIRED");
		}
	}
}

string Server::getUUID() 
{
  uuid_t id;
  uuid_generate(id);

  char buffer[100];
  uuid_unparse(id, buffer);
	// return as string
  return buffer;
}


// in ldap mit un und pw anmelden ertun true/false
bool helferLDAP::login(string us, string pw) {
  bool success = false;

  LDAP* ld = helferLDAP::init();
  helferLDAP::bindAnonym(ld);

  string usDN = helferLDAP::searchUs(ld, us);

  if (usDN != "") {
    // try login
    success = helferLDAP::bindUs(ld, usDN, pw);
  }

  // unbind
  ldap_unbind_ext_s(ld, NULL, NULL);

  return success;
}

bool helferLDAP::bindUs (LDAP* ld, string usDN, string pw) {
  BerValue *servercredp;
  BerValue cred;
  cred.bv_val = const_cast<char*> (pw.c_str());
  cred.bv_len = pw.length();

  int rc = ldap_sasl_bind_s (ld, usDN.c_str(), LDAP_SASL_SIMPLE, &cred, NULL, NULL, &servercredp);

  if (rc != LDAP_SUCCESS) {
    cerr << "LDAP Fehler beim Binding mit User: " << ldap_err2string(rc) << endl;
    return false;
  }
   
  cout << "LDAP Userbinding erfolgreich" << endl; 
  return true;
}

// init von ldap 
LDAP* helferLDAP::init() {
  LDAP *ld; 
  int ldapversion = LDAP_VERSION3;
  int rc = 0;

  if (ldap_initialize(&ld,LDAP_URI) != LDAP_SUCCESS) {
    cerr << "ldap_init fehlgeschlagen" << endl;
    exit (EXIT_FAILURE);
  }

  cout << "Verbinde zu LDAP Server " << LDAP_URI << endl;

  if ((rc = ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &ldapversion)) != LDAP_SUCCESS) {
    cerr << "ldap_set_option(PROTOCOL_VERSION): " << ldap_err2string(rc) << endl;
    ldap_unbind_ext_s(ld, NULL, NULL);
    exit (EXIT_FAILURE);
  }

  if ((rc = ldap_start_tls_s(ld, NULL, NULL)) != LDAP_SUCCESS) {
    cerr << "ldap_start_tls_s(): " << ldap_err2string(rc) << endl;
    ldap_unbind_ext_s(ld, NULL, NULL);
    exit (EXIT_FAILURE);
  }

  return ld;
}

void helferLDAP::bindAnonym(LDAP* ld) {
  BerValue *servercredp;
  BerValue cred;
  cred.bv_val = (char *)ANONYMOUS_PW;
  cred.bv_len = strlen(ANONYMOUS_PW);

  int rc = ldap_sasl_bind_s(ld, ANONYMOUS_USER, LDAP_SASL_SIMPLE, &cred, NULL, NULL, &servercredp);

  if (rc != LDAP_SUCCESS) {
    cerr << "LDAP anonymous bind error: " << ldap_err2string(rc) << endl;
    ldap_unbind_ext_s(ld, NULL, NULL);
    exit (EXIT_FAILURE);
  }
  
  cout << "Anonymer LDAP bind erfolgreich" << endl; 
}


string helferLDAP::searchUs(LDAP* ld, string us) {
  const char* attributes[] = { "cn", NULL }; 
  const char* filter = ((string) ("(uid=" + us + ")")).c_str();
  LDAPMessage *msg; // LDAP result handle
  string usDN = "";

  // search for user
  int rc = ldap_search_ext_s (ld, SEARCHBASE, SCOPE, filter, (char **)attributes, 0, NULL, NULL, NULL, 500, &msg);

  if (rc != LDAP_SUCCESS) {
    cerr << "LDAP Fehler Suche: " << ldap_err2string(rc) << endl;
    ldap_unbind_ext_s (ld, NULL, NULL);
    exit (EXIT_FAILURE);
  }

	// ergebnise mitzählen
  int ergCount = ldap_count_entries(ld, msg);
  if (ergCount == 1) {
    LDAPMessage* a = ldap_first_entry (ld, msg);
    
//name setzen
    usDN = ldap_get_dn (ld, a);
      
    // cn attribute checken
    BerElement *ber; 
    char* attribute = ldap_first_attribute(ld, a, &ber);
    BerValue **values = ldap_get_values_len (ld, a, attribute);

    if (values != NULL) {
      string usCN = "";

// durch loopen
      for (int i = 0; i < ldap_count_values_len(values); i++) {
        usCN += values[i]->bv_val;
      }

      cout << "LDAP User wurde gefunden \"" << us << "\" (" << usCN << ")" << endl;
// memory freigeben von values
      ldap_value_free_len (values);
    }

    // fmemory freigeben für attribte
    ldap_memfree (attribute);
    if (ber != NULL) ber_free(ber, 0);
  }

  // fmemory freigeben für msg
  ldap_msgfree(msg);

  if (usDN == "") cout << "LDAP User wurde nicht gefunden \"" << us << "\"" << endl;
  return usDN;
}



set<string> Server::parseReceivers(string receivers) {
	set<string> receiversSet;
	vector<string> receiversSplitted = Server::split(receivers, ',');

	for (string receiver : receiversSplitted) {
		if (receiver != "") {
			receiversSet.insert(receiver);
		}
	}

	return receiversSet;
}

long Server::timeNow(string einheit)
{
	if (einheit == "s") {
		return chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count();
	}
	else if (einheit == "ms") {
		return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
	}
	return 0;
}

/**
 * Stores a mail in the sent folder of the sender and at the receivers, returns if succesful.
 */
bool Server::storeMail(string sender, string receivers, string subject, string message) {
	bool errorFlag = false;

	// 1: build unique id (time_uuid) for filename
	long now = Server::timeNow("ms");
	string uuid = Server::getUUID();
	string fileName = to_string(now) + "_" + uuid;

	// 2: save in sent folder
	{
		lock_guard<mutex> terminalGuard(this->termMtx);
		cout << "Storing mail in sent folder of " << sender << ": " << subject << endl;
	}
	bool createDirSuccess;
	{
		lock_guard<mutex> dirGuard(this->direcMtx);
		createDirSuccess = Server::createUserDir(this->mailDirect + "/" + sender + "/sent");
	}
	if (createDirSuccess) {
		string content = sender + "\n" + receivers + "\n" + subject + "\n" + message + "\n.";
		lock_guard<mutex> fileGuard(this->fileMtx);
		ofstream mailFile(this->mailDirect + "/" + sender + "/sent/" + fileName);

		if (mailFile) {
			mailFile << content << endl;
			mailFile.close();
		}
		else {
			errorFlag = true;
		}
	}
	else {
		errorFlag = true;
	}

	// 3: create symlinks at receivers
	if (!errorFlag) {
		set<string> receiversSet = this->parseReceivers(receivers);

		for (string receiver : receiversSet) {
			if (receiver.length() > 8) {
				{
					lock_guard<mutex> terminalGuard(this->termMtx);
					cout << "Receiver " << receiver << " exceeded length, skipping." << endl;
				}
				errorFlag = true;
				continue; // set error and skip too long receiver
			}
			{
				lock_guard<mutex> terminalGuard(this->termMtx);
				cout << "Storing mail " << sender << " -> " << receiver << ": " << subject << endl;
			}
			bool createDirSuccess;
			{
				lock_guard<mutex> dirGuard(this->direcMtx);
				createDirSuccess = Server::createUserDir(this->mailDirect + "/" + receiver);
			}
			if (createDirSuccess) {
				string fromFilePath = "../" + sender + "/sent/" + fileName;
				string toFilePath = this->mailDirect + "/" + receiver + "/" + fileName;

				lock_guard<mutex> fileGuard(this->fileMtx);
				int returnSymlink = symlink(fromFilePath.c_str(), toFilePath.c_str());

				if (returnSymlink != 0) {
				perror("Fehler bei Symlink.");
					errorFlag = true;
				}
			}
			else {
				errorFlag = true;
			}
		}
	}

	return !errorFlag;
}

/**
 * Takes care of the LIST command from the client.
 */
void Server::LIST(int clientSocket, string loggedInUser) {
	// if user is logged in
	if (loggedInUser != "") {
		map<string, string> mailSubjects = this->getMailSubjects(loggedInUser);

		// send number of mails
		int mailCount = mailSubjects.size();
		this->sendMessage(clientSocket, to_string(mailCount));

		// send mail id and subject
		int i = 1;

		for (auto pair : mailSubjects) {
			string entry = "[" + to_string(i) + "] " + pair.second;
			this->sendMessage(clientSocket, entry);

			i++;
		}
	}
	else {
		this->sendMessage(clientSocket, "LOGIN REQUIRED");
	}
}

/**
 * Returns the mail subjects with filename from the mail directory.
 * By using map the mails get automatically sorted by the time.
 */
map<string, string> Server::getMailSubjects(string receiver) {
	struct dirent *entry;
	map<string, string> mailSubjects;
	string dirpath = this->mailDirect + "/" + receiver;

	// open directory
	lock_guard<mutex> dirGuard(this->direcMtx);
	DIR *dp = opendir(dirpath.c_str());

	if (dp != NULL) {
		// read files in directory
		while ((entry = readdir(dp)) != NULL) {
			string filename = entry->d_name;

			// exclude sent folder and ".", ".."
			if (filename != "." && filename != ".." && filename != "sent") {
				string filepath = dirpath + "/" + filename;
				lock_guard<mutex> fileGuard(this->fileMtx);
				ifstream inFileStream(filepath);

				if (inFileStream) {
					string subject;
					getline(inFileStream, subject); // skip sender
					getline(inFileStream, subject); // skip receiver
					getline(inFileStream, subject); // only save subject

					mailSubjects[filename] = subject;

					inFileStream.close();
				}
			}
		}

		// close directory
		int closedirReturn = closedir(dp);

		if (closedirReturn != 0) {
			perror("Fehler beim schließen von DIrectory.");
		}
	}
	else {
			perror("Fehler beim öffnen von DIrectory.");
	}

	return mailSubjects;
}

/**
 * Returns the filenames of the mails from the given user.
 */
vector<string> Server::getMailFilenames(string receiver) {
	struct dirent *entry;
	vector<string> mailFilenames;
	string dirpath = this->mailDirect + "/" + receiver;

	// open directory
	lock_guard<mutex> dirGuard(this->direcMtx);
	DIR *dp = opendir(dirpath.c_str());

	if (dp != NULL) {
		// read files in directory
		while ((entry = readdir(dp)) != NULL) {
			string filename = entry->d_name;

			// exclude sent folder and ".", ".."
			if (filename != "." && filename != ".." && filename != "sent") {
				mailFilenames.push_back(filename);
			}
		}

		// close directory
		int closedirReturn = closedir(dp);

		if (closedirReturn != 0) {
			perror("Fehler beim Schließen von DIrectory.");
		}

		// sort vector
		sort(mailFilenames.begin(), mailFilenames.end());
	}
	else {
			perror("Fehler beim Öffnen von DIrectory.");
	}

	return mailFilenames;
}

/**
 * Takes care of the READ command from the client.
 */
void Server::READ(int clientSocket, string loggedInUser, sockenSchnuffler& socketReader) {
	bool isClientActive;
	string mailIDString = this->recMessage(clientSocket, isClientActive, socketReader);

	// if client socket not closed
	if (isClientActive) {
		// if user is logged in
		if (loggedInUser != "") {
			bool success = false;
			string mailContent = "";
			int mailID = 0;

			try {
				mailID = stoi(mailIDString);
			}
			catch (const exception& e) {
				{
					lock_guard<mutex> terminalGuard(this->termMtx);
					cerr << this->progName << ": Could not convert \"" + mailIDString + "\" to integer" << endl;
				}
			}

			if (mailID > 0) {
				vector<string> mailFilenames = this->getMailFilenames(loggedInUser);

				// does mail exist
				if (mailID <= (int)mailFilenames.size()) {
					string filename = mailFilenames[mailID - 1];
					string filepath = this->mailDirect + "/" + loggedInUser + "/" + filename;
					lock_guard<mutex> fileGuard(this->fileMtx);
					ifstream inFileStream(filepath);

					if (inFileStream) {
						// read whole file
						stringstream fileBuffer;
						fileBuffer << inFileStream.rdbuf();
						mailContent = fileBuffer.str();

						inFileStream.close();
						success = true;
					}
				}
			}

			if (success) {
				this->sendMessage(clientSocket, "OK");
				this->sendMessage(clientSocket, mailContent);
			}
			else {
				this->sendMessage(clientSocket, "ERR");
			}
		}
		else {
			this->sendMessage(clientSocket, "LOGIN REQUIRED");
		}
	}
}

/**
 * Takes care of the DEL command from the client.
 */
void Server::DEL(int clientSocket, string loggedInUser, sockenSchnuffler& socketReader) {
	bool isClientActive;
	string mailIDString = this->recMessage(clientSocket, isClientActive, socketReader);

	// if client socket not closed
	if (isClientActive) {
		// if user is logged in
		if (loggedInUser != "") {
			bool success = false;
			int mailID = 0;

			try {
				mailID = stoi(mailIDString);
			}
			catch (const exception& e) {
				{
					lock_guard<mutex> terminalGuard(this->termMtx);
					cerr << this->progName << ": Could not convert \"" + mailIDString + "\" to integer" << endl;
				}
			}

			if (mailID > 0) {
				vector<string> mailFilenames = this->getMailFilenames(loggedInUser);

				// does mail exist
				if (mailID <= (int)mailFilenames.size()) {
					string filename = mailFilenames[mailID - 1];
					string filepath = this->mailDirect + "/" + loggedInUser + "/" + filename;

					// delete file
					lock_guard<mutex> fileGuard(this->fileMtx);
					int removeReturn = remove(filepath.c_str());

					if (removeReturn == 0) {
						success = true;
					}
					else {
						perror("Fehler beim Entfernen.");
					}
				}
			}

			// send response
			this->sendMessage(clientSocket, success ? "OK" : "ERR");
		}
		else {
			this->sendMessage(clientSocket, "LOGIN REQUIRED");
		}
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
		rc = sockenSchnuffler::myRead(fd, &c);

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

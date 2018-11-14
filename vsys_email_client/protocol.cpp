//
// Created by user on 18.10.18.
//

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
#include "protocol.h"
#include "Functions.h"

#define BUF 1024
#define SEN 8
#define REC 8
#define SUB 80
#define RES 8

//user enters data and the array holding the data gets constructed
SEND::SEND() {

    while ((getchar()) != '\n');

    data[0] = head;

    std::cout << "Sender eingeben (8 Zeichen):" << std::endl;
    std::getline(std::cin, sender);
    if (sender.size() > SEN) {
        sender.resize(SEN);
    }
    sender += "\n";
    data[1] = sender;

    std::cout << "Empfänger eingeben (8 Zeichen):" << std::endl;
    std::getline(std::cin, receiver);
    if (receiver.size() > REC) {
        receiver.resize(REC);
    }
    receiver += "\n";
    data[2] = receiver;

    std::cout << "Betreff eingeben (8 Zeichen):" << std::endl;
    std::getline(std::cin, subject);
    if (subject.size() > SUB) {
        subject.resize(SUB);
    }
    subject += "\n";
    data[3] = subject;

    std::string currentline = "";
    std::cout << "Nachricht eingeben (.\\n zum senden):" << std::endl;
    while (strncmp(currentline.data(), ".\n", 1) != 0) {
        std::getline(std::cin, currentline);
        currentline += "\n";
        message += currentline;
    }
    data[4] = message;


}

//sends the data to the server
bool SEND::OUT(int socket) {


    for (auto &i : this->data) {

        writen(socket, i.data(), i.size()); //write to sock


    }


    return true;
}

SEND::~SEND() {

}

//user enters sender to view messages
LIST::LIST() {

    while ((getchar()) != '\n');

    data[0] = head;

    std::cout << "Sender eingeben (8 Zeichen)" << std::endl;
    std::getline(std::cin, user);
    if (user.size() > SEN) {
        user.resize(SEN);
    }
    user += "\n";
    data[1] = user;


}


bool LIST::OUT(int socket) {

    for (auto &i : this->data) {

        writen(socket, i.data(), i.size()); //write to sock


    }


    return true;

}

LIST::~LIST() {

}

//user enteres sender to view specific messagw
READ::READ() {

    while ((getchar()) != '\n');

    data[0] = head;

    std::cout << "Sender eingeben (8 Zeichen)" << std::endl;
    std::getline(std::cin, user);
    if (user.size() > SEN) {
        user.resize(SEN);
    }
    user += "\n";
    data[1] = user;

    std::cout << "Nachrichten Nummer eingeben" << std::endl;
    std::cin >> msgnr;
    msgnr += "\n";
    data[2] = msgnr;


}

//sends data
bool READ::OUT(int socket) {

    for (auto &i : this->data) {

        writen(socket, i.data(), i.size()); //write to sock

    }


    return true;

}

READ::~READ() {

}

/**
 * user enteres sender and the message to delete
 */

DEL::DEL() {

    while ((getchar()) != '\n');

    data[0] = head;

    std::cout << "Sender eingeben (8 Zeichen)" << std::endl;
    std::getline(std::cin, user);
    if (user.size() > SEN) {
        user.resize(SEN);
    }
    user += "\n";
    data[1] = user;

    std::cout << "Nachrichten Nummer eingeben" << std::endl;
    std::cin >> msgnr;
    msgnr += "\n";
    data[2] = msgnr;


}

bool DEL::OUT(int socket) {

    for (auto &i : this->data) {

        writen(socket, i.data(), i.size()); //write to sock

    }


    return true;

}

DEL::~DEL() {

}


/**
 *  write n bytes to a descriptor ...
 */
ssize_t writen(int fd, const char *vptr, size_t n) {
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;
    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) <= 0) {
            if (errno == EINTR)
                nwritten = 0; // and call write() again
            else
                return (-1);
        }
        nleft -= nwritten;
        ptr += nwritten;
    };
    //  std::cout << "sent" << std::endl;
    return (n);
};

/**
 * check if everythin is read correctly
 * @param socket
 * @param ptr
 * @return
 */
ssize_t my_read(int socket, char *ptr) {
    static int read_cnt = 0;
    static char *read_ptr;
    static char read_buf[BUF];
    if (read_cnt <= 0) {
        again:
        if ((read_cnt = read(socket, read_buf, sizeof(read_buf))) < 0) {
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

/**
 * read a line
 */


ssize_t readline(int socket, char *vptr, size_t maxlen) {
    ssize_t n, rc;
    char c, *ptr;
    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ((rc = my_read(socket, &c)) == 1) {
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


/**
 * Checks for errors
 * @param socket
 * @return
 */
bool checkErr(int socket) {
    char response[RES];

    readline(socket, response, RES - 1);


    response[RES - 1] = '\0';
    std::string line(response);


    if (isEqual(line, "err\n")) {
        std::cout << "Fehler beim verarbeiten der Anfrage!\n" << std::endl;
        return false;
    } else if (isEqual(line, "ok\n")) {
        std::cout << "Anfrage erfolgreich verarbeitet!\n" << std::endl;
    }
    memset(response, 0, RES);
    return true;

}
//
//
//Created by user on 18.10.18.
//


#include <cstdio>
#include <iostream>

#ifndef VSYS_EMAIL_CLIENT_PROTOCOL_H
#define VSYS_EMAIL_CLIENT_PROTOCOL_H


ssize_t writen(int fd, const char *vptr, size_t n);

ssize_t readline(int socket, char *vptr, size_t maxlen);

ssize_t my_read(int socket, char *ptr);

bool checkErr(int socket);


class SEND {

private:
    std::string head = "SEND\n";;
    std::string sender;
    std::string receiver;
    std::string subject;
    std::string message;



public:
    std::string data[6];

    SEND();
    ~SEND();
    bool OUT(int socket);

};


class LIST {

private:
    std::string head = "LIST\n";
    std::string user;
public:
    LIST();
    ~LIST();
    std::string data[3];

    bool OUT(int socket);

};

class READ {

private:
    std::string head = "READ\n";
    std::string user;
    std::string msgnr;
public:
    std::string data[4];
    ~READ();
    READ();

    bool OUT(int socket);

};

class DEL {

private:
    std::string head = "DEL\n";
    std::string user;
    std::string msgnr;
public:
    std::string data[4];
    ~DEL();
    DEL();

    bool OUT(int socket);


};


#endif //VSYS_EMAIL_CLIENT_PROTOCOL_H

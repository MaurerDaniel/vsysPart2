/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Functions.h
 * Author: acs
 *
 * Created on October 24, 2018, 2:26 PM
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string.h>
#include <list>
#include <locale> 
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

int dirExists(const char *path);
int dirExists(std::string path);
bool isDigit(std::string string);
bool isDigit(char* string);
std::string toLower(std::string string);
bool isEqual(std::string str1, std::string str2);
std::string getMessageLine(int i, std::list<std::string> message);
//bool acceptConnection(int& clientSocket, string&clientIP);
long timeNow(string einheit);


#endif /* FUNCTIONS_H */


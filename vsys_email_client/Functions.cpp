/* 
 * File:   FileOperations.h
 * Author: acs
 *
 * Created on October 24, 2018, 2:21 PM
 */

#include "Functions.h"
#include <string.h>
#include <list>

bool isDigit(std::string string) {
    std::locale loc;
    for (char c : string) {
        if (!isdigit(c, loc)) {
            return false;
        }
    }
    return true;
}

bool isDigit(char* string) {
    std::string str(string);
    return isDigit(str);
}

bool isEqual(std::string str1, std::string str2) {
    std::string lower1 = toLower(str1);
    std::string lower2 = toLower(str2);
    if (str1.length() != str2.length()) {
        return false;
    }
    for (int i = 0; i < str1.length(); i++) {
        if (lower1[i] != lower2[i]) {
            return false;
        }
    }
    return true;
}

int dirExists(const char *path) {
    struct stat info;

    if (stat(path, &info) != 0) {
        return 0;
    } else if (info.st_mode & S_IFDIR) {
        return 1;
    } else {
        return 0;
    }
}

int dirExists(std::string path) {
    return dirExists(path.c_str());
}

std::string toLower(std::string string) {
    std::string result = "";
    std::locale loc;
    for (std::string::size_type i = 0; i < string.length(); ++i) {
        result += std::tolower(string[i], loc);
    }
    return result;
}





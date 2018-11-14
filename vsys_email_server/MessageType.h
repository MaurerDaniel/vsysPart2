
/* 
 * File:   MessageType.h
 * Author: acs
 * Created on October 29, 2018, 4:32 PM
 */

#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H

/**
 * Nachrichtentyp als Enum für effizientere Vergleiche, da der Nachrichtentyp sehr oft geprüft wird
 * wird hier an stringcompares gespart indem nach dem ersten vergleich nur mehr die enums genutzt werden.
 */
enum MessageType {
    SEND, LIST, QUIT, DEL, READ
};

#endif /* MESSAGETYPE_H */


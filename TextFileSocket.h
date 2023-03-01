///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        TextFileSocket.h
//
// Author:      David Borland
//
// Description: Interface of TextFileSocket class for reading data from a text file
//              as if it were a socket.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef TEXTFILESOCKET_H
#define TEXTFILESOCKET_H


#include "Socket.h"

#include <fstream>


class TextFileSocket : public Socket {
public:
    TextFileSocket(bool readAllData = false);
    virtual ~TextFileSocket();

    virtual bool Init(const char* fileName, unsigned short ignore = 0);
    virtual void Read(std::string& s);

private:
    std::fstream file;

    bool loop;
};


#endif
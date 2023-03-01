///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        TextFileTextFileSocket.h
//
// Author:      David Borland
//
// Description: Implemenation of TextFileSocket class for reading data from a text file
//              as if it were a socket. 
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "TextFileSocket.h"

#include <wx/log.h>


TextFileSocket::TextFileSocket(bool readAllData) : Socket(readAllData) {
}


TextFileSocket::~TextFileSocket() {
    file.close();
}


bool TextFileSocket::Init(const char* hostName, unsigned short ignore) {
    file.open(hostName, std::fstream::in);
    if (file.fail()) {
        wxLogMessage("Couldn't open %s", hostName);
        return false;
    }

    return true;
}


void TextFileSocket::Read(std::string& s) {
    // Check if the previous line read was the last line
    if (file.eof()) {
        s = "EOF";

        file.clear();
        file.seekg(0, std::fstream::beg);

        return;
    }

    // Read the data
    s.clear();
    while (!file.eof()) {
        std::string buff;
        getline(file, buff);

        s += buff + "\n";

        if (!readAll) break;
    }
}
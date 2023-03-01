///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Socket.h
//
// Author:      David Borland
//
// Description: Interface of Socket class for receiving data.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef SOCKET_H
#define SOCKET_H


#include <winsock.h>
#include <string>


class Socket {
public:
    Socket(bool readAllData = false);
    virtual ~Socket();

    virtual bool Init(const char* hostName, unsigned short port = 9001);
    virtual void Read(std::string& s);

    bool GetReadAll();
    void SetReadAll(bool readAllData);

protected:
    std::string remainder;
    bool readAll;

private:
    SOCKET sock;
};


#endif
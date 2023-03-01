///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Socket.h
//
// Author:      David Borland
//
// Description: Implementation of Socket class for receiving data.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "Socket.h"

#include <wx/log.h>


Socket::Socket(bool readAllData) : readAll(readAllData) {
}


Socket::~Socket() {
    closesocket(sock);

    WSACleanup();
}


bool Socket::Init(const char* hostName, unsigned short port) {
    // Initialize the socket library
    WSADATA info;
    if (WSAStartup(MAKEWORD(1, 1), &info) != 0) {
        wxLogMessage("Cannot initialize WinSock.");
        return false;
    }

    // Find the server
    struct hostent* host;
    host = gethostbyname(hostName);
    if (host == NULL) {
        wxLogMessage("Can't find host.");
        return false;
    }

    // Set up the socket
    struct sockaddr_in socketAddress;
    memset(&socketAddress, 0, sizeof(socketAddress));
    memcpy((char*)&socketAddress.sin_addr, host->h_addr, host->h_length);
    socketAddress.sin_family = host->h_addrtype;
    socketAddress.sin_port = htons(port);

    // Connect to the server
    sock = socket(host->h_addrtype, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        wxLogMessage("Can't connect to host.");
        return false;
    }

    // Connect to the socket
    if (connect(sock, (struct sockaddr*)&socketAddress, sizeof(socketAddress)) == SOCKET_ERROR) {
        wxLogMessage("Can't connect to socket.");
        return false;
    }

    return true;
}


void Socket::Read(std::string& s) {
    const int bufSize = 32;
    char buffer[bufSize];

    unsigned long numBytes;

    // Initialize with whatever was leftover from last time
    s = remainder;

    // Read the data
    while (ioctlsocket(sock, FIONREAD, &numBytes) == 0 && numBytes > 0) {
        // Don't read more data than the buffer can handle
        if (numBytes > bufSize - 1) numBytes = bufSize - 1;

        // Clear the buffer
        memset(buffer, 0, bufSize);

        // Receive data
        if (recv(sock, buffer, numBytes, 0) < 0) {
            wxLogMessage("Error receiving data.", buffer);

            // If in the middle of a line, discard everything
            if (s[s.length() -1] != '\n') {
                s.clear();
            }
            return;
        }

        // Append this buffer
        s += buffer;

        if (!readAll) break;
    }

    // Only return completed lines
    std::string::size_type end = s.find_last_of("\n");

    if (end != std::string::npos) {
        // Save the remainder
        remainder = s.substr(end + 1, s.length() - 1 - end);

        // Return the rest
        s = s.substr(0, end + 1);
    }
    else {
        // No complete lines
        remainder = s;
        s.clear();
    }
}


bool Socket::GetReadAll() {
    return readAll;
}


void Socket::SetReadAll(bool readAllData) {
    readAll = readAllData;
}
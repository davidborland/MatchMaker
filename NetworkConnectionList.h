///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        NetworkConnectionList.h
//
// Author:      David Borland
//
// Description: Interface of NetworkConnectionList class for MatchMaker.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef NETWORKCONNECTIONLIST_H
#define NETWORKCONNECTIONLIST_H


#include "NetworkConnection.h"

#include <vtkRenderer.h>

#include <vector>


class NetworkConnectionList {
public:
    NetworkConnectionList(vtkRenderer* ren, bool useDarkBackground);
    ~NetworkConnectionList();

    NetworkConnection* Get(Site* source, Site* dest);

    void Update();

    // Reset the data
    void Reset();

private:
    std::vector<NetworkConnection*> connections;

    bool darkBackground;

    vtkRenderer* renderer;
};


#endif
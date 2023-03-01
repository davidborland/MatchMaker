///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        NetworkConnectionList.cpp
//
// Author:      David Borland
//
// Description: Implementation of NetworkConnectionList class for MatchMaker.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "NetworkConnectionList.h"


NetworkConnectionList::NetworkConnectionList(vtkRenderer* ren, bool useDarkBackground) : renderer(ren), darkBackground(useDarkBackground) {
}

NetworkConnectionList::~NetworkConnectionList() {
    for (int i = 0; i < (int)connections.size(); i++) {
        delete connections[i];
    }
}


NetworkConnection* NetworkConnectionList::Get(Site* source, Site* dest) {
    // Search for source and destination names
    for (int i = 0; i < (int)connections.size(); i++) {
        if ((connections[i]->GetSourceID() == source->GetID() && connections[i]->GetDestID() == dest->GetID()) ||
            (connections[i]->GetSourceID() == dest->GetID() && connections[i]->GetDestID() == source->GetID())) {
            return connections[i];
        }
    }

    // It's not there, so add it
    connections.push_back(new NetworkConnection(source, dest, renderer, darkBackground));
    source->AddNetworkConnection(connections.back());
    dest->AddNetworkConnection(connections.back());

    return connections.back();
}


void NetworkConnectionList::Update() {
    for (int i = 0; i < (int)connections.size(); i++) {
        connections[i]->Update();
    }
}


void NetworkConnectionList::Reset() {
    for (int i = 0; i < (int)connections.size(); i++) {
        delete connections[i];
    }
    connections.clear();
}
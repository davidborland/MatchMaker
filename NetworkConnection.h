///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        NetworkConnection.h
//
// Author:      David Borland
//
// Description: Interface of NetworkConnection class for MatchMaker.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef NETWORKCONNECTION_H
#define NETWORKCONNECTION_H


#include <vtkActor.h>
#include <vtkPlaneSource.h>
#include <vtkRenderer.h>

#include <string>
#include <vector>

#include <Vec3.h>

#include "Site.h"


class Site;


class NetworkConnection {
public:
    NetworkConnection(Site* sourceSite, Site* destSite, vtkRenderer* ren, bool darkBackground);
    ~NetworkConnection();

    void SetBandwidth(double networkBandwidth);

    const std::string& GetSourceID();
    const std::string& GetDestID();

    Vec3* GetSourcePosition();
    Vec3* GetDestPosition();

    void Update();

private:
    Site* source;
    Site* dest;

    vtkActor* actor;
    vtkPlaneSource* plane;
    vtkRenderer* renderer;

    Vec3 sourcePosition;
    Vec3 destPosition;

    double bandwidth;
};


#endif
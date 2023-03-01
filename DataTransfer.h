///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        DataTransfer.h
//
// Author:      David Borland
//
// Description: Interface of DataTransfer class for MatchMaker.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef DATATRANSFER_H
#define DATATRANSFER_H


#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <vector>

#include <Vec3.h>

#include "Job.h"
#include "NetworkConnection.h"
#include "Site.h"


class Job;
class NetworkConnection;
class Site;


class DataTransfer {
public:
    DataTransfer(Site* sourceSite, Site* sinkSite, NetworkConnection* connection, Job* requester, double dataSize, vtkRenderer* ren);
    ~DataTransfer();

    void SetOpacity(double sphereOpacity);

    void Update();

private:
    // Sphere and mapper used for multiple spheres
    vtkSphereSource* sphere;
    vtkPolyDataMapper* mapper;

    // Sphere actors
    std::vector<vtkActor*> fromSource;
    std::vector<vtkActor*> toJob;
    vtkActor* atJob;

    // Lines
    vtkLineSource* fromSourceLine;
    vtkLineSource* toJobLine;
    vtkActor* fromSourceLineActor;
    vtkActor* toJobLineActor;

    // Source and sink position of networkConnection
    Vec3* sourcePos;
    Vec3* sinkPos;

    // The job and source
    Job* job;
    Site* source;

    // The size of the data being transferred
    double size;

    // Animation offset
    double offset;
    double offsetIncrement;

    double opacity;

    // Fraction of job radius
    double radiusScale;

    double radius;

    vtkRenderer* renderer;

    vtkActor* CreateSphere();
    void DoUpdate(std::vector<vtkActor*>& actors, const Vec3& pos1, const Vec3& pos2, double radius, double spacing);
    void DoColor();
};


#endif
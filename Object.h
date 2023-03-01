///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Object.h
//
// Author:      David Borland
//
// Description: Interface of abstract Object class for MatchMaker.  Object is the base class
//              of Site and Job.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef OBJECT_H
#define OBJECT_H


#include <vtkRenderer.h>


class Object {
public:
    Object(std::string objectID, double radius, vtkRenderer* ren);
    virtual ~Object();

    const std::string& GetID();

    virtual void SetColor(double r, double g, double b) = 0;
    virtual void SetRadius(double radius) = 0;

protected:
    // Unique identifier
    std::string id;

    vtkRenderer* renderer;

    int resolution;
};


#endif
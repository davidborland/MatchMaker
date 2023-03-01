///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Object.h
//
// Author:      David Borland
//
// Description: Implementation of Object class for MatchMaker.  Object is the base class
//              of Site and Job.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "Object.h"

#include <vtkProperty.h>


Object::Object(std::string objectID, double radius, vtkRenderer *ren) : id(objectID), renderer(ren) {
    resolution = 16;
}


Object::~Object() {
}


const std::string& Object::GetID() {
    return id;
}
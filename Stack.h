///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Stack.h
//
// Author:      David Borland
//
// Description: Interface of Stack class for MatchMaker.  Handles the geometry of stacks
//              drawn at sites.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef STACK_H
#define STACK_H


#include <vtkActor.h>
#include <vtkCylinderSource.h>
#include <vtkDiskSource.h>
#include <vtkRenderer.h>

#include "Vec3.h"


class Stack {
public:
    Stack(double innerRadius, double outerRadius, double spindleRadius, double resolution, bool showSiteSpindle, vtkRenderer* ren);
    virtual ~Stack();

    virtual void SetPosition(const Vec3& pos);
    void SetRadii(double innerRadius, double outerRadius, double spindleRadius);
    void SetColor(double r, double g, double b);
    virtual void SetSpindleHeight(double height);
    void ShowSpindle(bool show);

    Vec3 GetPosition();

protected:
    vtkDiskSource* disk;
    vtkActor* diskActor;
    vtkCylinderSource* spindle;
    vtkActor* spindleActor;

    vtkRenderer* renderer;

    double spindleOffset;

    virtual void SetSpindlePosition();
};


#endif
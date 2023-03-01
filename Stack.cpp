///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Stack.h
//
// Author:      David Borland
//
// Description: Implementation of Stack class for MatchMaker.  Handles the geometry of stacks
//              drawn at sites.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "Stack.h"

#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>


Stack::Stack(double innerRadius, double outerRadius, double spindleRadius, double resolution, bool showSiteSpindle, vtkRenderer* ren) : renderer(ren) {
    spindleOffset = 0.02;

    // Create the disk
    disk = vtkDiskSource::New();
    disk->SetRadialResolution(1);
    disk->SetCircumferentialResolution(resolution);
    disk->SetInnerRadius(innerRadius);
    disk->SetOuterRadius(outerRadius);
    vtkPolyDataMapper* diskMapper = vtkPolyDataMapper::New();
    diskMapper->SetInputConnection(disk->GetOutputPort());
    diskActor = vtkActor::New();
    diskActor->SetMapper(diskMapper);
    diskActor->GetProperty()->SetAmbient(1.0);
    diskActor->GetProperty()->SetDiffuse(0.0);
    diskActor->GetProperty()->SetSpecular(0.0);


    // Create a spindle representing the maximum number of jobs so far
    spindle = vtkCylinderSource::New();
    spindle->SetResolution(resolution);
    spindle->SetHeight(0.0);
    spindle->SetRadius(spindleRadius);
    vtkPolyDataMapper* spindleMapper = vtkPolyDataMapper::New();
    spindleMapper->SetInputConnection(spindle->GetOutputPort());
    spindleActor = vtkActor::New();
    spindleActor->SetMapper(spindleMapper);
    spindleActor->GetProperty()->SetAmbient(0.0);
    spindleActor->GetProperty()->SetDiffuse(1.0);
    spindleActor->GetProperty()->SetSpecular(0.0);
    spindleActor->RotateX(90.0);


    // Add to the renderer
    renderer->AddViewProp(diskActor);
    if (showSiteSpindle) renderer->AddViewProp(spindleActor);


    // Don't need these reference any more
    diskMapper->Delete();
    spindleMapper->Delete();
}


Stack::~Stack() {
    // Remove actors
    if (diskActor) renderer->RemoveViewProp(diskActor);
    if (spindleActor) renderer->RemoveViewProp(spindleActor);

    // Clean up
    if (disk) disk->Delete();
    if (diskActor) diskActor->Delete();
    if (spindle) spindle->Delete();
    if (spindleActor) spindleActor->Delete();
}


void Stack::SetPosition(const Vec3& pos) {
    diskActor->SetPosition(pos.X(), pos.Y(), pos.Z());
    SetSpindlePosition();
}


void Stack::SetRadii(double innerRadius, double outerRadius, double spindleRadius) {
    disk->SetInnerRadius(innerRadius);
    disk->SetOuterRadius(outerRadius);
    spindle->SetRadius(spindleRadius);
}


void Stack::SetColor(double r, double g, double b) {
    diskActor->GetProperty()->SetColor(r, g, b);
    spindleActor->GetProperty()->SetColor(r, g, b);
}


void Stack::SetSpindleHeight(double height) {
    spindle->SetHeight(height);
    SetSpindlePosition();
}


void Stack::ShowSpindle(bool show) {
    if (show) renderer->AddViewProp(spindleActor);
    else renderer->RemoveViewProp(spindleActor);
}


Vec3 Stack::GetPosition() {
    return Vec3(diskActor->GetPosition()[0],
                diskActor->GetPosition()[1],
                diskActor->GetPosition()[2]);
}


void Stack::SetSpindlePosition() {
    spindleActor->SetPosition(diskActor->GetPosition()[0],
                              diskActor->GetPosition()[1],
                              diskActor->GetPosition()[2] + spindleOffset + spindle->GetHeight() * 0.5);
}
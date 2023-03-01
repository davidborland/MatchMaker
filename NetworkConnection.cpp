///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        NetworkConnection.cpp
//
// Author:      David Borland
//
// Description: Implementation of NetworkConnection class for MatchMaker.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "NetworkConnection.h"

#include <vtkMath.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

#include <Quat.h>


NetworkConnection::NetworkConnection(Site* sourceSite, Site* destSite, vtkRenderer* ren, bool darkBackground) : source(sourceSite), dest(destSite), renderer(ren) {
    plane = vtkPlaneSource::New();
    plane->SetOrigin(0.0, -0.5, 0.0);
    plane->SetPoint1(1.0, -0.5, 0.0);
    plane->SetPoint2(0.0,  0.5, 0.0);
    plane->SetXResolution(1);
    plane->SetYResolution(1);

    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->SetInputConnection(plane->GetOutputPort());

    actor = vtkActor::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0.0, darkBackground ? 0.4 : 0.75, 0.0);
    actor->GetProperty()->SetAmbient(1.0);
    actor->GetProperty()->SetDiffuse(0.0);
    actor->GetProperty()->SetSpecular(0.0);

    bandwidth = -1.0;

    Update();

    mapper->Delete();
}

NetworkConnection::~NetworkConnection() {
    // Remove actor
    renderer->RemoveViewProp(actor);

    plane->Delete();
    actor->Delete();
}


void NetworkConnection::SetBandwidth(double networkBandwidth) {
    bandwidth = networkBandwidth;

    // Ignore negative bandwidths
    if (bandwidth > 0) {
        // Set the size
        double dataScale = 4.0;
        double minRadius = 2.0;
        double maxRadius = source->GetOuterRadius();

        minRadius = minRadius > maxRadius ? maxRadius : minRadius;

        double width = bandwidth * dataScale;
        width = width < minRadius * 2.0 ? minRadius * 2.0 : width;
        width = width > maxRadius * 2.0 ? maxRadius * 2.0 : width;

        actor->GetScale()[1] = width;

        renderer->AddViewProp(actor);
    }
}


const std::string& NetworkConnection::GetSourceID() {
    return source->GetID();
}

const std::string& NetworkConnection::GetDestID() {
    return dest->GetID();
}


Vec3* NetworkConnection::GetSourcePosition() {
    return &sourcePosition;
}

Vec3* NetworkConnection::GetDestPosition() {
    return &destPosition;
}


void NetworkConnection::Update() {
    Vec2 sourcePos(source->GetPosition());
    Vec2 destPos;

    // Get closest destination stack from source position
    dest->GetClosestStackCenter(sourcePos, destPos);
    destPosition.Set(destPos.X(), destPos.Y(), dest->GetPosition().Z() - 0.05);

    // Get closest source stack from destination stack
    source->GetClosestStackCenter(destPos, sourcePos);
    sourcePosition.Set(sourcePos.X(), sourcePos.Y(), source->GetPosition().Z() - 0.05);

    // Check which has highest y value
    Vec3 pos1, pos2;
    if (sourcePosition.Y() < destPosition.Y()) {
        pos1 = sourcePosition;
        pos2 = destPosition;
    }
    else {        
        pos1 = destPosition;
        pos2 = sourcePosition;
    }

    // Get the difference
    Vec3 diff = pos2 - pos1;

    // Unit x vector
    Vec3 vecX(1.0, 0.0, 0.0);

    // Get the rotation angle
    double length = diff.Magnitude();
    diff.Normalize();
    double dot = diff.DotProduct(vecX);

    double theta = Quat::RadiansToDegrees(acos(dot));

    // Set the orientation and position
    actor->SetOrientation(0, 0, theta);
    actor->SetPosition(pos1.X(), pos1.Y(), pos1.Z());
    actor->GetScale()[0] = length;

    SetBandwidth(bandwidth);
}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        DataTransfer.h
//
// Author:      David Borland
//
// Description: Interface of DataTransfer class for MatchMaker.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "DataTransfer.h"

#include <vtkMath.h>
#include <vtkProperty.h>

#include <wx/log.h>


DataTransfer::DataTransfer(Site* sourceSite, Site* sinkSite, NetworkConnection* connection, Job* requester, double dataSize, vtkRenderer* ren) 
: source(sourceSite), job(requester), size(dataSize), renderer(ren) {
    // Create the sphere and mapper used for all spheres
    sphere = vtkSphereSource::New();
    sphere->SetRadius(1.0);
    sphere->SetThetaResolution(12);

    mapper = vtkPolyDataMapper::New();
    mapper->SetInputConnection(sphere->GetOutputPort());


    // Get the position pointers set up correctly
    if (source->GetID() == connection->GetSourceID() && sinkSite->GetID() == connection->GetDestID()) {
        sourcePos = connection->GetSourcePosition();
        sinkPos = connection->GetDestPosition();
    }
    else {
        sourcePos = connection->GetDestPosition();
        sinkPos = connection->GetSourcePosition();
    }

    
    // Create the permanent sphere at the job
    atJob = CreateSphere();


    // Create the lines
    fromSourceLine = vtkLineSource::New();
    vtkPolyDataMapper* fromSourceLineMapper = vtkPolyDataMapper::New();
    fromSourceLineMapper->SetInputConnection(fromSourceLine->GetOutputPort());
    fromSourceLineActor = vtkActor::New();
    fromSourceLineActor->SetMapper(fromSourceLineMapper);

    toJobLine = vtkLineSource::New();
    vtkPolyDataMapper* toJobLineMapper = vtkPolyDataMapper::New();
    toJobLineMapper->SetInputConnection(toJobLine->GetOutputPort());
    toJobLineActor = vtkActor::New();
    toJobLineActor->SetMapper(toJobLineMapper);

    renderer->AddViewProp(fromSourceLineActor);
    renderer->AddViewProp(toJobLineActor);

    fromSourceLineMapper->Delete();
    toJobLineMapper->Delete();


    // Set the initial animation offset
    offset = 0.0;
    offsetIncrement = 1.0;


    // Fraction of job radius
    radiusScale = 0.25;
}

DataTransfer::~DataTransfer() {
    // Clean up
    renderer->RemoveViewProp(atJob);
    atJob->Delete();

    sphere->Delete();
    mapper->Delete();

    for (int i = 0; i < (int)fromSource.size(); i++) {
        renderer->RemoveViewProp(fromSource[i]);
        fromSource[i]->Delete();
    }
    for (int i = 0; i < (int)toJob.size(); i++) {
        renderer->RemoveViewProp(toJob[i]);
        toJob[i]->Delete();
    }

    renderer->RemoveViewProp(fromSourceLineActor);
    renderer->RemoveViewProp(toJobLineActor);
    fromSourceLineActor->Delete();
    toJobLineActor->Delete();
}


void DataTransfer::SetOpacity(double sphereOpacity) {
    opacity = sphereOpacity;
    for (int i = 0; i < (int)fromSource.size(); i++) {
        fromSource[i]->GetProperty()->SetOpacity(opacity);
    }    
    for (int i = 0; i < (int)toJob.size(); i++) {
        toJob[i]->GetProperty()->SetOpacity(opacity);
    }
    atJob->GetProperty()->SetOpacity(opacity);

    fromSourceLineActor->GetProperty()->SetOpacity(opacity);
    toJobLineActor->GetProperty()->SetOpacity(opacity);
}


void DataTransfer::Update() {
    // Spacing between spheres
    double radius = job->GetRadius() * radiusScale * size * 0.1;
    radius = radius < job->GetRadius() * radiusScale ? job->GetRadius() * radiusScale : radius;
    radius = radius > job->GetRadius() * radiusScale * 2.0 ? job->GetRadius() * radiusScale * 2.0 : radius;
    double spacing = radius * 3.0;

    // Outer radius of site
    double outerRadius = source->GetOuterRadius();
    

    // Get the vector from the source to the sink
    Vec3 vec = *sinkPos - *sourcePos;
    double distance = vec.Magnitude();
    vec.Normalize();


    // Get the start point
    Vec3 startPoint = *sourcePos + vec * outerRadius;
    distance -= outerRadius;


    // Get the point at which spheres will leave the path and go towards jobs
    double r = outerRadius * 2.0 > spacing * 3.0 ? outerRadius * 2.0 : spacing * 3.0;
    int num = (distance - r) / spacing;
    double midDistance = num * spacing + 0.1;

    Vec3 midPoint = startPoint + vec * midDistance;


    // Get the vector from the midpoint to the job
    vec = job->GetPosition() - midPoint;
    Vec2 vec2D(vec);
    vec2D.Normalize();

    // Get the end point
    Vec3 endPoint(job->GetPosition().X() - vec2D.X() * job->GetRadius(),
                  job->GetPosition().Y() - vec2D.Y() * job->GetRadius(),
                  job->GetPosition().Z());

    // Do the update
    DoUpdate(fromSource, startPoint, midPoint, radius, spacing);
    DoUpdate(toJob, midPoint, endPoint, radius, spacing);
    atJob->SetPosition(endPoint.X(), endPoint.Y(), endPoint.Z());
    atJob->SetScale(radius, radius, job->GetHeight() * 0.5);

    fromSourceLine->SetPoint1(startPoint.X(), startPoint.Y(), startPoint.Z() + 0.1);
    fromSourceLine->SetPoint2(midPoint.X(), midPoint.Y(), midPoint.Z() + 0.1);
    toJobLine->SetPoint1(midPoint.X(), midPoint.Y(), midPoint.Z() + 0.1);
    toJobLine->SetPoint2(endPoint.X(), endPoint.Y(), endPoint.Z() + 0.1);

    // Color
    DoColor();

    // Increment the animation
    offset += offsetIncrement;
    offset = offset > spacing ? 0.0 : offset;
}


vtkActor* DataTransfer::CreateSphere() {
    vtkActor* actor = vtkActor::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(0.0, 0.4, 0.0);
    actor->GetProperty()->SetOpacity(opacity);

    renderer->AddViewProp(actor);

    return actor;
}


void DataTransfer::DoUpdate(std::vector<vtkActor*>& actors, const Vec3& pos1, const Vec3& pos2, double radius, double spacing) {
    Vec3 vec = pos2 - pos1;
    double distance = vec.Magnitude();
    vec.Normalize();

    int numSpheres = distance / spacing;

    if (numSpheres > (int)actors.size()) {
        int diff = numSpheres - (int)actors.size();
        for (int i = 0; i < diff; i++) {
            actors.push_back(CreateSphere());
        }
    }
    else if (numSpheres < (int)actors.size()) {
        int diff = (int)actors.size() - numSpheres;
        for (int i = 0; i < diff; i++) {
            renderer->RemoveViewProp(actors.back());
            actors.back()->Delete();
            actors.pop_back();
        }
    }

    double height = job->GetHeight() * 0.5;
    for (int i = 0; i < (int)actors.size(); i++) {
        actors[i]->SetPosition(pos1.X(), pos1.Y(), pos1.Z());
        actors[i]->AddPosition(vec.X() * offset + vec.X() * spacing * i, 
                               vec.Y() * offset + vec.Y() * spacing * i,
                               vec.Z() * offset + vec.Z() * spacing * i);
        actors[i]->SetScale(radius, radius, height);
    }
}

void DataTransfer::DoColor() {
    const double* sourceColor = source->GetColor();
    const double* jobColor = job->GetColor();
    double r, g, b;
    double frac;

    int numSpheres = (int)fromSource.size() + (int)toJob.size() + 1;
    int thisNum = 0;

    for (int i = 0; i < (int)fromSource.size(); i++) {
        frac = (double)thisNum / (double)numSpheres;
        r = sourceColor[0] * (1.0 - frac) + jobColor[0] * frac;
        g = sourceColor[1] * (1.0 - frac) + jobColor[1] * frac;
        b = sourceColor[2] * (1.0 - frac) + jobColor[2] * frac;
        fromSource[i]->GetProperty()->SetColor(r, g, b);
        thisNum++;
    }    
    for (int i = 0; i < (int)toJob.size(); i++) {
        frac = (double)thisNum / (double)numSpheres;
        r = sourceColor[0] * (1.0 - frac) + jobColor[0] * frac;
        g = sourceColor[1] * (1.0 - frac) + jobColor[1] * frac;
        b = sourceColor[2] * (1.0 - frac) + jobColor[2] * frac;
        toJob[i]->GetProperty()->SetColor(r, g, b);
        thisNum++;
    }
    atJob->GetProperty()->SetColor(jobColor[0], jobColor[1], jobColor[2]);

    fromSourceLineActor->GetProperty()->SetColor(jobColor[0], jobColor[1], jobColor[2]);
    toJobLineActor->GetProperty()->SetColor(jobColor[0], jobColor[1], jobColor[2]);
}
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Job.h
//
// Author:      David Borland
//
// Description: Interface of Job class for MatchMaker.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "Job.h"

#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>

#include <wx/log.h>


// Initialize colors
double Job::matchingColor[] =   {1.0, 1.0, 1.0};
double Job::submittingColor[] = {0.0,  1.0, 1.0};
double Job::queuedColor[] =     {1.0, 1.0, 0.0};
double Job::runningColor[] =    {0.0,  1.0, 0.0};
double Job::doneColor[3] =      {1.0, 0.0,  1.0};
double Job::failedColor[3] =    {1.0, 0.0,  0.0};


Job::Job(const std::string& jobID, double radius, vtkRenderer* ren, 
         Site* startSite, double height, double jobVelocity, 
         ShowGlyphType showWhichGlyphs, bool showGhostJobs, bool fadeGhostJobs, bool showJobPath, bool showJobTrail, 
         double labelHeight, bool labelFaceCamera) 
         : Object(jobID, radius, ren), velocity(jobVelocity), showGhosts(showGhostJobs), fadeGhosts(fadeGhostJobs), showPath(showJobPath), showTrail(showJobTrail) {
    // Set the data transfer
    data = NULL;

    // Create the cylinder representing this job
    statusGlyph = vtkCylinderSource::New();
    statusGlyph->SetResolution(resolution);
    statusGlyph->SetHeight(height);
    vtkPolyDataMapper* statusGlyphMapper = vtkPolyDataMapper::New();
    statusGlyphMapper->SetInputConnection(statusGlyph->GetOutputPort());
    statusActor = vtkActor::New();
    statusActor->SetMapper(statusGlyphMapper);
    statusActor->GetProperty()->SetAmbient(0.0);
    statusActor->GetProperty()->SetDiffuse(1.0);
    statusActor->GetProperty()->SetSpecular(0.0);
    statusActor->RotateX(90.0);

    // Create a second cylinder representing this job at its new site
    ghostStatusGlyph = vtkCylinderSource::New();
    ghostStatusGlyph->SetResolution(resolution);
    ghostStatusGlyph->SetHeight(height);
    vtkPolyDataMapper* ghostStatusGlyphMapper = vtkPolyDataMapper::New();
    ghostStatusGlyphMapper->SetInputConnection(ghostStatusGlyph->GetOutputPort());
    ghostStatusActor = vtkActor::New();
    ghostStatusActor->SetMapper(ghostStatusGlyphMapper);
    ghostStatusActor->GetProperty()->SetAmbient(0.0);
    ghostStatusActor->GetProperty()->SetDiffuse(1.0);
    ghostStatusActor->GetProperty()->SetSpecular(0.0);
    ghostStatusActor->GetProperty()->SetOpacity(0.0);
    ghostStatusActor->RotateX(90.0);

    // Create a third cylinder representing this job at its old site
    oldGhostStatusGlyph = vtkCylinderSource::New();
    oldGhostStatusGlyph->SetResolution(resolution);
    oldGhostStatusGlyph->SetHeight(height);
    vtkPolyDataMapper* oldGhostStatusGlyphMapper = vtkPolyDataMapper::New();
    oldGhostStatusGlyphMapper->SetInputConnection(oldGhostStatusGlyph->GetOutputPort());
    oldGhostStatusActor = vtkActor::New();
    oldGhostStatusActor->SetMapper(oldGhostStatusGlyphMapper);
    oldGhostStatusActor->GetProperty()->SetAmbient(0.0);
    oldGhostStatusActor->GetProperty()->SetDiffuse(1.0);
    oldGhostStatusActor->GetProperty()->SetSpecular(0.0);
    oldGhostStatusActor->GetProperty()->SetOpacity(0.0);
    oldGhostStatusActor->RotateX(90.0);

    
    // Create the representation of the type of science of the job
    scienceGlyph = vtkCylinderSource::New();
    scienceGlyph->SetResolution(resolution);
    scienceGlyph->SetHeight(height * 0.5);
    vtkPolyDataMapper* scienceGlyphMapper = vtkPolyDataMapper::New();
    scienceGlyphMapper->SetInputConnection(scienceGlyph->GetOutputPort());
    scienceActor = vtkActor::New();
    scienceActor->SetMapper(scienceGlyphMapper);
    scienceActor->GetProperty()->SetAmbient(0.0);
    scienceActor->GetProperty()->SetDiffuse(1.0);
    scienceActor->GetProperty()->SetSpecular(0.0);
    scienceActor->RotateX(90.0);

    // Create a second cylinder representing the type of science at its new site
    ghostScienceGlyph = vtkCylinderSource::New();
    ghostScienceGlyph->SetResolution(resolution);
    ghostScienceGlyph->SetHeight(height * 0.5);
    vtkPolyDataMapper* ghostScienceGlyphMapper = vtkPolyDataMapper::New();
    ghostScienceGlyphMapper->SetInputConnection(ghostScienceGlyph->GetOutputPort());
    ghostScienceActor = vtkActor::New();
    ghostScienceActor->SetMapper(ghostScienceGlyphMapper);
    ghostScienceActor->GetProperty()->SetAmbient(0.0);
    ghostScienceActor->GetProperty()->SetDiffuse(1.0);
    ghostScienceActor->GetProperty()->SetSpecular(0.0);
    ghostScienceActor->GetProperty()->SetOpacity(0.0);
    ghostScienceActor->RotateX(90.0);

    // Create a third cylinder representing the type of science at its old site
    oldGhostScienceGlyph = vtkCylinderSource::New();
    oldGhostScienceGlyph->SetResolution(resolution);
    oldGhostScienceGlyph->SetHeight(height * 0.5);
    vtkPolyDataMapper* oldGhostScienceGlyphMapper = vtkPolyDataMapper::New();
    oldGhostScienceGlyphMapper->SetInputConnection(oldGhostScienceGlyph->GetOutputPort());
    oldGhostScienceActor = vtkActor::New();
    oldGhostScienceActor->SetMapper(oldGhostScienceGlyphMapper);
    oldGhostScienceActor->GetProperty()->SetAmbient(0.0);
    oldGhostScienceActor->GetProperty()->SetDiffuse(1.0);
    oldGhostScienceActor->GetProperty()->SetSpecular(0.0);
    oldGhostScienceActor->GetProperty()->SetOpacity(0.0);
    oldGhostScienceActor->RotateX(90.0);


    // Set the radius for all glyphs
    SetRadius(radius);


    // Create the representation of the job's path
    path = vtkLineSource::New();
    path->SetResolution(1);
    vtkPolyDataMapper* pathMapper = vtkPolyDataMapper::New();
    pathMapper->SetInputConnection(path->GetOutputPort());
    pathActor = vtkActor::New();
    pathActor->SetMapper(pathMapper);
    pathActor->GetProperty()->SetAmbient(0.0);
    pathActor->GetProperty()->SetDiffuse(1.0);
    pathActor->GetProperty()->SetSpecular(0.0);
    pathActor->GetProperty()->SetOpacity(0.75);
    pathActor->GetProperty()->SetLineWidth(2);


    // Create the representation of the job's trail
    trail = vtkLineSource::New();
    trail->SetResolution(1);
    vtkPolyDataMapper* trailMapper = vtkPolyDataMapper::New();
    trailMapper->SetInputConnection(trail->GetOutputPort());
    trailActor = vtkActor::New();
    trailActor->SetMapper(trailMapper);
    trailActor->GetProperty()->SetAmbient(0.0);
    trailActor->GetProperty()->SetDiffuse(1.0);
    trailActor->GetProperty()->SetSpecular(0.0);
    trailActor->GetProperty()->SetOpacity(0.25);
    trailActor->GetProperty()->SetLineWidth(2);


    // Create text label for the site
    if (labelFaceCamera) {
        text = vtkCaptionActor2D::New();
        text->SetCaption("default");
        text->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
        text->GetPositionCoordinate()->SetValue(0.0, 0.01 - labelHeight);
        text->SetWidth(1.0);
        text->SetHeight(labelHeight);
        text->BorderOff();
        text->ThreeDimensionalLeaderOff();
        text->GetCaptionTextProperty()->SetJustificationToLeft();
        text->GetCaptionTextProperty()->BoldOff();
        text->GetCaptionTextProperty()->ShadowOff();
        text->GetCaptionTextProperty()->ItalicOff();
        text->GetProperty()->SetLineStipplePattern(0x0000);

        text3D = NULL;
    }
    else {
        text3D = vtkTextActor3D::New();
        text3D->SetInput("default");
        text3D->GetTextProperty()->SetJustificationToCentered();
        text3D->GetTextProperty()->BoldOn();
        text3D->GetTextProperty()->ShadowOff();
        text3D->GetTextProperty()->ItalicOff();

        text = NULL;
    }
    showName = false;


    // Default state
    SetState("MATCHING");


    // Add the actor to the renderer
    ShowGlyphs(showWhichGlyphs);


    // Set the initial site
    moving = false;
    site = NULL;
    oldSite = NULL;
    startSite->AttachJob(this);
    statusActor->SetPosition(position.X(), position.Y(), position.Z());
    scienceActor->SetPosition(position.X(), position.Y(), position.Z());
    SetPosition(position);
    SetOldPosition(position);
    path->SetPoint1(position.X(), position.Y(), position.Z());
    path->SetPoint2(position.X(), position.Y(), position.Z());
    trail->SetPoint1(position.X(), position.Y(), position.Z());
    trail->SetPoint2(position.X(), position.Y(), position.Z());


    // Don't need these references any more
    statusGlyphMapper->Delete();
    ghostStatusGlyphMapper->Delete();
    oldGhostStatusGlyphMapper->Delete();

    scienceGlyphMapper->Delete();
    ghostScienceGlyphMapper->Delete();
    oldGhostScienceGlyphMapper->Delete();

    pathMapper->Delete();
    trailMapper->Delete();
}


Job::~Job() {
    // Remove from sites
    if (oldSite) oldSite->RemoveJob(id);
    site->RemoveJob(id);

    // Remove actors
    renderer->RemoveViewProp(statusActor);
    renderer->RemoveViewProp(ghostStatusActor);
    renderer->RemoveViewProp(oldGhostStatusActor);

    renderer->RemoveViewProp(scienceActor);
    renderer->RemoveViewProp(ghostScienceActor);
    renderer->RemoveViewProp(oldGhostScienceActor);

    renderer->RemoveViewProp(pathActor);
    renderer->RemoveViewProp(trailActor);

    if (text) renderer->RemoveViewProp(text);
    if (text3D) renderer->RemoveViewProp(text3D);

    // Clean up
    statusGlyph->Delete();
    statusActor->Delete();
    ghostStatusGlyph->Delete();
    ghostStatusActor->Delete();
    oldGhostStatusGlyph->Delete();
    oldGhostStatusActor->Delete();

    scienceGlyph->Delete();
    scienceActor->Delete();
    ghostScienceGlyph->Delete();
    ghostScienceActor->Delete();
    oldGhostScienceGlyph->Delete();
    oldGhostScienceActor->Delete();

    path->Delete();
    pathActor->Delete();
    trail->Delete();
    trailActor->Delete();

    if (text) text->Delete();
    if (text3D) text3D->Delete();

    if (data) delete data;
}


Site* Job::GetSite() {
    return site;
}


bool Job::SetState(const std::string& state) {
    isDone = false;

    // Set the color based on the state
    if (state == "MATCHING") {
        SetColor(matchingColor[0], matchingColor[1], matchingColor[2]);
    }
    else if (state == "SUBMITTING") {
        SetColor(submittingColor[0], submittingColor[1], submittingColor[2]);
    }
    else if (state == "QUEUED") {
        SetColor(queuedColor[0], queuedColor[1], queuedColor[2]);
    }
    else if (state == "RUNNING") {
        SetColor(runningColor[0], runningColor[1], runningColor[2]);
    }
    else if (state == "DONE") {
        SetColor(doneColor[0], doneColor[1], doneColor[2]);
        isDone = true;
    }
    else if (state == "FAILED") {
        SetColor(failedColor[0], failedColor[1], failedColor[2]);
    }
    else {
        // Don't know this state
        return false;
    }

    // If not matching and there is a data transfer, stop the data transfer
    if (state != "MATCHING") {
        if (data) {
            delete data;
            data = NULL;
        }
    }

    return true;
}


void Job::SetSite(Site* newSite) {
    if (oldSite) {
        oldSite->RemoveJob(id);
    }
    oldSite = site;
    site = newSite;

    moving = true;
    SetOldPosition(position);

    // Add these actors to the renderer
    if (showGhosts) {
        if (showGlyphs == ShowStatusOnly) {
            renderer->AddViewProp(ghostStatusActor);
            renderer->AddViewProp(oldGhostStatusActor);
        }
        else if (showGlyphs == ShowScienceOnly) {
            renderer->AddViewProp(ghostScienceActor);
            renderer->AddViewProp(oldGhostScienceActor);
        }
        else if (showGlyphs == ShowStatusAndScience) {
            renderer->AddViewProp(ghostScienceActor);
            renderer->AddViewProp(oldGhostScienceActor);

            renderer->AddViewProp(ghostStatusActor);
            renderer->AddViewProp(oldGhostStatusActor);
        }
    }
    if (showPath) {
        renderer->AddViewProp(pathActor);
    }
    if (showTrail) {
        renderer->AddViewProp(trailActor);
    }
}


void Job::SetScienceColor(double r, double g, double b) {
    scienceActor->GetProperty()->SetColor(r, g, b);
    ghostScienceActor->GetProperty()->SetColor(r, g, b);
    oldGhostScienceActor->GetProperty()->SetColor(r, g, b);

    if (showGlyphs == ShowScienceOnly) {
        pathActor->GetProperty()->SetColor(r, g, b);
        trailActor->GetProperty()->SetColor(r, g, b);
    }
}


void Job::StartDataTransfer(Site* dataSource, Site* dataSink, NetworkConnection* connection, double dataSize) {
    // Make sure there's not already a data transfer.  If so, kill it.
    if (data) delete data;

    data = new DataTransfer(dataSource, dataSink, connection, this, dataSize, renderer);
    data->SetOpacity(statusActor->GetProperty()->GetOpacity());
}


double Job::GetRadius() {
    return statusGlyph->GetRadius();
}

const double* Job::GetColor() {
    return statusActor->GetProperty()->GetColor();
}

void Job::SetRadius(double radius) {
    statusGlyph->SetRadius(radius);
    ghostStatusGlyph->SetRadius(radius);
    oldGhostStatusGlyph->SetRadius(radius);

    scienceGlyph->SetRadius(radius * 1.25);
    ghostScienceGlyph->SetRadius(radius * 1.25);
    oldGhostScienceGlyph->SetRadius(radius * 1.25);
}

void Job::SetColor(double r, double g, double b) {
    statusActor->GetProperty()->SetColor(r, g, b);
    ghostStatusActor->GetProperty()->SetColor(r, g, b);
    oldGhostStatusActor->GetProperty()->SetColor(r, g, b);

    if (showGlyphs != ShowScienceOnly) {
        pathActor->GetProperty()->SetColor(r, g, b);
        trailActor->GetProperty()->SetColor(r, g, b);
    }

    double colorScale = matchingColor[0] < 1.0 ? 0.0 : -0.75;

    if (text) text->GetProperty()->SetColor(r + colorScale < 0.0 ? 0.0 : r + colorScale, 
                                            g + colorScale < 0.0 ? 0.0 : g + colorScale,
                                            b + colorScale < 0.0 ? 0.0 : b + colorScale);
    if (text3D) text3D->GetTextProperty()->SetColor(r + colorScale < 0.0 ? 0.0 : r + colorScale, 
                                                    g + colorScale < 0.0 ? 0.0 : g + colorScale,
                                                    b + colorScale < 0.0 ? 0.0 : b + colorScale);
}

void Job::SetOpacity(double opacity) {
    statusActor->GetProperty()->SetOpacity(opacity);
    scienceActor->GetProperty()->SetOpacity(opacity);

    // Ghost actor opacities get set in UpdatePosition()

    pathActor->GetProperty()->SetOpacity(opacity * 0.75);
    trailActor->GetProperty()->SetOpacity(opacity * 0.25);

    if (data) data->SetOpacity(opacity);
}


double Job::GetHeight() {
    return statusGlyph->GetHeight();
}

void Job::SetHeight(double height) {
    statusGlyph->SetHeight(height);
    ghostStatusGlyph->SetHeight(height);
    oldGhostStatusGlyph->SetHeight(height);

    scienceGlyph->SetHeight(height * 0.5);
    ghostScienceGlyph->SetHeight(height * 0.5);
    oldGhostScienceGlyph->SetHeight(height * 0.5);
}


const std::string& Job::GetName() {
    return name;
}

void Job::SetName(const std::string& jobName) {
    name = jobName;

    // Set the caption text
    if (text) text->SetCaption(name.c_str());
    if (text3D) text3D->SetInput(name.c_str());

    ShowName(showName);
}


bool Job::IsDone() {
    return isDone;
}


void Job::ShowGlyphs(ShowGlyphType show) {
    showGlyphs = show;

    if (showGlyphs == ShowStatusOnly) {
        renderer->AddViewProp(statusActor);
        renderer->RemoveViewProp(scienceActor);

        if (moving) {            
            renderer->AddViewProp(ghostStatusActor);
            renderer->AddViewProp(oldGhostStatusActor);

            renderer->RemoveViewProp(ghostScienceActor);
            renderer->RemoveViewProp(oldGhostScienceActor);
        }

        pathActor->GetProperty()->SetColor(statusActor->GetProperty()->GetColor());
        trailActor->GetProperty()->SetColor(statusActor->GetProperty()->GetColor());
    }
    else if (showGlyphs == ShowScienceOnly) {
        renderer->RemoveViewProp(statusActor);
        renderer->AddViewProp(scienceActor);

        if (moving) {
            renderer->RemoveViewProp(ghostStatusActor);
            renderer->RemoveViewProp(oldGhostStatusActor);

            renderer->AddViewProp(ghostScienceActor);
            renderer->AddViewProp(oldGhostScienceActor);
        }

        pathActor->GetProperty()->SetColor(scienceActor->GetProperty()->GetColor());
        trailActor->GetProperty()->SetColor(scienceActor->GetProperty()->GetColor());
    }
    else if (showGlyphs == ShowStatusAndScience) {
        renderer->AddViewProp(statusActor);
        renderer->AddViewProp(scienceActor);

    if (moving) {
            renderer->AddViewProp(ghostScienceActor);
            renderer->AddViewProp(oldGhostScienceActor);

            renderer->AddViewProp(ghostStatusActor);
            renderer->AddViewProp(oldGhostStatusActor);
        }

        pathActor->GetProperty()->SetColor(statusActor->GetProperty()->GetColor());
        trailActor->GetProperty()->SetColor(statusActor->GetProperty()->GetColor());
    }
}


void Job::UpdatePosition() {
    // Update any data transfer
    if (data) data->Update();


    if (!moving) {
        // Make sure we are in the correct place
        statusActor->SetPosition(position.X(), position.Y(), position.Z());
        oldGhostStatusActor->SetPosition(position.X(), position.Y(), position.Z());
        scienceActor->SetPosition(position.X(), position.Y(), position.Z());
        oldGhostScienceActor->SetPosition(position.X(), position.Y(), position.Z());

        path->SetPoint1(position.X(), position.Y(), position.Z());
        path->SetPoint2(position.X(), position.Y(), position.Z());
        trail->SetPoint1(position.X(), position.Y(), position.Z());
        trail->SetPoint2(position.X(), position.Y(), position.Z());

        return;
    }


    // Get the current position
    Vec3 actorPos(statusActor->GetPosition()[0], statusActor->GetPosition()[1], statusActor->GetPosition()[2]);
    Vec3 diff = position - actorPos;
    double dist = diff.Magnitude();

    if (dist <= velocity) {
        // If close enough, set to end position
        statusActor->SetPosition(position.X(), position.Y(), position.Z());
        oldGhostStatusActor->SetPosition(position.X(), position.Y(), position.Z());
        scienceActor->SetPosition(position.X(), position.Y(), position.Z());        
        oldGhostScienceActor->SetPosition(position.X(), position.Y(), position.Z());

        path->SetPoint1(position.X(), position.Y(), position.Z());
        trail->SetPoint1(position.X(), position.Y(), position.Z());

        // Don't need these any more
        renderer->RemoveViewProp(ghostStatusActor);
        renderer->RemoveViewProp(oldGhostStatusActor);
        renderer->RemoveViewProp(ghostScienceActor);
        renderer->RemoveViewProp(oldGhostScienceActor);
        renderer->RemoveViewProp(pathActor);
        renderer->RemoveViewProp(trailActor);

        // Remove from old site
        if (oldSite) {
            oldSite->RemoveJob(id);
            oldSite = NULL;
        }

        // Done moving
        moving = false;
    }
    else {
        // Get the total distance
        Vec3 totalDiff = oldPosition - position;
        double totalDist = totalDiff.Magnitude();

        // Normalize the direction vector
        Vec3 norm = diff;
        norm.Normalize();

        // Scale 
        Vec3 offset = norm;
        offset *= velocity;

        // Move
        statusActor->AddPosition(offset.X(), offset.Y(), offset.Z());
        scienceActor->SetPosition(statusActor->GetPosition());

        // Update ghost opacities
        double maxOpacity = statusActor->GetProperty()->GetOpacity();
        if (fadeGhosts) {
            double frac = dist / totalDist;
            
            double minOpacity = maxOpacity * 0.25;
            double ghostOpacity = (maxOpacity - frac) * (maxOpacity - minOpacity) + minOpacity;
            double oldGhostOpacity = frac * (maxOpacity - minOpacity) + minOpacity;
            ghostStatusActor->GetProperty()->SetOpacity(ghostOpacity);
            oldGhostStatusActor->GetProperty()->SetOpacity(oldGhostOpacity);
            ghostScienceActor->GetProperty()->SetOpacity(ghostOpacity);
            oldGhostScienceActor->GetProperty()->SetOpacity(oldGhostOpacity);
        }
        else {                
            ghostStatusActor->GetProperty()->SetOpacity(maxOpacity * 0.5);
            oldGhostStatusActor->GetProperty()->SetOpacity(maxOpacity * 0.5);
            ghostScienceActor->GetProperty()->SetOpacity(maxOpacity * 0.5);
            oldGhostScienceActor->GetProperty()->SetOpacity(maxOpacity * 0.5);
        }

        // Update path
        double radius = statusGlyph->GetRadius();
        norm = diff;
        norm.Z() = 0.0;
        norm.Normalize();
        norm *= radius;
        path->SetPoint1(statusActor->GetPosition()[0] + norm.X(),            
                        statusActor->GetPosition()[1] + norm.Y(),
                        statusActor->GetPosition()[2]);
        path->SetPoint2(position.X() - norm.X(),
                        position.Y() - norm.Y(),
                        position.Z());

        norm.Set(actorPos.X() - oldPosition.X(), actorPos.Y() - oldPosition.Y(), 0.0);
        norm.Normalize();
        norm *= radius;
        trail->SetPoint1(oldPosition.X() + norm.X(),
                         oldPosition.Y() + norm.Y(),
                         oldPosition.Z());
        trail->SetPoint2(statusActor->GetPosition()[0] - norm.X(),
                         statusActor->GetPosition()[1] - norm.Y(),
                         statusActor->GetPosition()[2]);
    }
}


const Vec3& Job::GetPosition() {
    return position;
}

void Job::SetPosition(const Vec3& pos) {
    position = pos;

    ghostStatusActor->SetPosition(position.X(), position.Y(), position.Z());
    ghostScienceActor->SetPosition(position.X(), position.Y(), position.Z());

    // Set the attachment point for the text
    if (text) text->SetAttachmentPoint(position.X() + statusGlyph->GetRadius(), position.Y(), position.Z());
    if (text3D) text3D->SetPosition(position.X() + statusGlyph->GetRadius(), position.Y(), position.Z());
}


void Job::SetOldPosition(const Vec3& pos) {
    oldPosition = pos; 
    oldGhostStatusActor->SetPosition(oldPosition.X(), oldPosition.Y(), oldPosition.Z());
    oldGhostScienceActor->SetPosition(oldPosition.X(), oldPosition.Y(), oldPosition.Z());
}


void Job::SetVelocity(double v) {
    velocity = v;
}


void Job::ShowGhost(bool show) {
    showGhosts = show;

    if (showGhosts) {
        if (moving) {
            if (showGlyphs == ShowStatusOnly) {
                renderer->AddViewProp(ghostStatusActor);
                renderer->AddViewProp(oldGhostStatusActor);
            }
            else if (showGlyphs == ShowScienceOnly) {
                renderer->AddViewProp(ghostScienceActor);
                renderer->AddViewProp(oldGhostScienceActor);
            }   
            else {                
                renderer->AddViewProp(ghostStatusActor);
                renderer->AddViewProp(oldGhostStatusActor);

                renderer->AddViewProp(ghostScienceActor);
                renderer->AddViewProp(oldGhostScienceActor);
            }
        }
    }
    else {
        renderer->RemoveViewProp(ghostStatusActor);
        renderer->RemoveViewProp(oldGhostStatusActor);

        renderer->RemoveViewProp(ghostScienceActor);
        renderer->RemoveViewProp(oldGhostScienceActor);
    }
}


void Job::FadeGhost(bool fade) {
    fadeGhosts = fade;
}


void Job::ShowPath(bool show) {
    showPath = show;

    if (showPath) {
        if (moving) {
            renderer->AddViewProp(pathActor);
        }
    }
    else {
        renderer->RemoveViewProp(pathActor);
    }
}


void Job::ShowTrail(bool show) {
    showTrail = show;

    if (showTrail) {
        if (moving) {
            renderer->AddViewProp(trailActor);
        }
    }
    else {
        renderer->RemoveViewProp(trailActor);
    }
}


void Job::ShowName(bool show) {
    showName = show;

    if (showName) {
        // Add the actor to the renderer
        if (name.size() > 0) {
            if (text) renderer->AddViewProp(text);
            if (text3D) renderer->AddViewProp(text3D);
        }
    }
    else {
        // Remove the actor from the renderer
        if (text) renderer->RemoveViewProp(text);
        if (text3D) renderer->RemoveViewProp(text3D);
    }
}


void Job::ScaleColor(double scale) {    
    for (int i = 0; i < 3; i++) {
        matchingColor[i] *= scale;
        submittingColor[i] *= scale;
        queuedColor[i] *= scale;
        runningColor[i] *= scale;
        doneColor[i] *= scale;
        failedColor[i] *= scale;
    }
}
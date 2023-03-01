///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Site.cpp
//
// Author:      David Borland
//
// Description: Implementation of Site class for MatchMaker.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "Site.h"


#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>

#include <vtkTextActor3D.h>

#include "Projector.h"

#include <wx/log.h>


Site::Site(const std::string& siteID, double radius, vtkRenderer* ren, vtkColorTransferFunction* lookUpTable,
           double jobSpacingDistance, double siteSpacingDistance, int maxSiteStackSize, bool showSiteSpindle, 
           double* mapXYExtents, const Vec2& unknownPosition, const Vec2& offTheMapPosition, 
           double labelHeight, bool labelFaceCamera, bool darkBackground) 
: Object(siteID, radius, ren), lut(lookUpTable), 
         jobSpacing(jobSpacingDistance), siteSpacing(siteSpacingDistance), maxStackSize(maxSiteStackSize), showSpindle(showSiteSpindle), 
         mapExtents(mapXYExtents), unknownPos(unknownPosition), offTheMapPos(offTheMapPosition) {
    // Defaults
    innerRadius = radius;
    outerRadius = radius * 1.5;
    spindleRadius = radius * 0.5;
    anchorRadius = radius * 0.5;
    mostNumJobs = 0;
    siteZ = 0.1;
    anchorOffset = 0.05;
    jobOffset = 0.05;


    // Create the first stack
    stacks.push_back(new Stack(0.0, outerRadius, spindleRadius, resolution, showSpindle, renderer));


    // Create an anchor point
    anchor = vtkDiskSource::New();
    anchor->SetRadialResolution(1);
    anchor->SetCircumferentialResolution(resolution);
    anchor->SetInnerRadius(0.0);
    anchor->SetOuterRadius(anchorRadius);
    vtkPolyDataMapper* anchorMapper = vtkPolyDataMapper::New();
    anchorMapper->SetInputConnection(anchor->GetOutputPort());
    anchorActor = vtkActor::New();
    anchorActor->SetMapper(anchorMapper);
    anchorActor->GetProperty()->SetAmbient(1.0);
    anchorActor->GetProperty()->SetDiffuse(0.0);
    anchorActor->GetProperty()->SetSpecular(0.0);


    // Create an anchor line
    anchorLine = vtkLineSource::New();
    vtkPolyDataMapper* anchorLineMapper = vtkPolyDataMapper::New();
    anchorLineMapper->SetInputConnection(anchorLine->GetOutputPort());
    anchorLineActor = vtkActor::New();
    anchorLineActor->SetMapper(anchorLineMapper); 
    anchorLineActor->GetProperty()->SetLineWidth(2.0);


    // Default color
    double rgb[3];
    lut->GetColor(0.0, rgb);
    SetColor(rgb[0], rgb[1], rgb[2]);


    // Create text label for the site
    if (labelFaceCamera) {
        text = vtkCaptionActor2D::New();
        text->SetCaption(id.c_str());
        text->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
        text->GetPositionCoordinate()->SetValue(-0.5, -labelHeight);
        text->SetWidth(1.0);
        text->SetHeight(labelHeight);
        text->BorderOff();
        text->ThreeDimensionalLeaderOff();
        text->GetCaptionTextProperty()->SetJustificationToCentered();
        text->GetCaptionTextProperty()->BoldOff();
        text->GetCaptionTextProperty()->ShadowOff();
        text->GetCaptionTextProperty()->ItalicOff();
        text->GetProperty()->SetLineStipplePattern(0x0000);
        if (darkBackground) text->GetProperty()->SetColor(1.0, 1.0, 1.0);
        else text->GetProperty()->SetColor(0.0, 0.0, 0.0);

        renderer->AddViewProp(text);

        text3D = NULL;
    }
    else {
        text3D = vtkTextActor3D::New();
        text3D->SetInput(id.c_str());
        text3D->GetTextProperty()->SetJustificationToCentered();
        text3D->GetTextProperty()->BoldOn();
        text3D->GetTextProperty()->ShadowOff();
        text3D->GetTextProperty()->ItalicOff();
        if (darkBackground) text3D->GetTextProperty()->SetColor(1.0, 1.0, 1.0);
        else text3D->GetTextProperty()->SetColor(0.0, 0.0, 0.0);

        renderer->AddViewProp(text3D);

        text = NULL;
    }

    // Default position
    anchorActor->SetPosition(unknownPos.X(), unknownPos.Y(), siteZ - anchorOffset);
    anchorLine->SetPoint1(unknownPos.X(), unknownPos.Y(), siteZ - anchorOffset);
    SetPosition(Vec3(unknownPos.X(), unknownPos.Y(), siteZ));
    location.Set(unknownPos.X(), unknownPos.Y(), siteZ);


    // Don't need these references any more
    anchorLineMapper->Delete();
    anchorMapper->Delete();
}


Site::~Site() {
    // Delete stacks
    for (int i = 0; i < (int)stacks.size(); i++) delete stacks[i];

    // Remove actors
    renderer->RemoveViewProp(anchorActor);
    renderer->RemoveViewProp(anchorLineActor);
    if (text) renderer->RemoveViewProp(text);
    if (text3D) renderer->RemoveViewProp(text3D);

    // Clean up
    anchor->Delete();
    anchorActor->Delete();
    anchorLine->Delete();
    anchorLineActor->Delete();
    if (text) text->Delete();
    if (text3D) text3D->Delete();
}


void Site::SetLongLat(double longitude, double latitude) {
    if (Vec2(longitude, latitude) == longLat) return;

    longLat.Set(longitude, latitude);

    // Convert to x, y
    double x = Projector::ProjectLongitude(longitude);
    double y = Projector::ProjectLatitude(latitude);

    // Check against map extents
    if (x < mapExtents[0] || x > mapExtents[2] ||
        y < mapExtents[1] || y > mapExtents[3]) {
        x = offTheMapPos.X();
        y = offTheMapPos.Y();
    }

    // Remove here.  If needed, these will be added later.
    renderer->RemoveViewProp(anchorActor);
    renderer->RemoveViewProp(anchorLineActor);

    // This is the new anchor point
    anchorActor->SetPosition(x, y, siteZ + anchorOffset);
    anchorLine->SetPoint1(x, y, siteZ + anchorOffset);

    // Set this as the location
    Vec3 pos(x, y, siteZ);

    location.Set(pos);

    // Set this as the position
    SetPosition(pos);
}


void Site::SetPosition(const Vec3& pos) {
    position = pos;

    // Set the attachment point for the text
    if (text) text->SetAttachmentPoint(position.X(), position.Y() - outerRadius, position.Z());
    if (text3D) text3D->SetPosition(position.X() - outerRadius * 2, position.Y() - outerRadius * 2, position.Z() + anchorOffset);

    
    // Update the stack positions
    Update();

    // Update the network connection positions
    for (int i = 0; i < (int)connections.size(); i++) {
        connections[i]->Update();
    }

    // Update point 2 on the anchor line
    Vec2 anchorPos(anchorActor->GetPosition()[0], anchorActor->GetPosition()[1]);

    // Get the closest stack center
    Vec2 stackPos;
    GetClosestStackCenter(anchorPos, stackPos);

    Vec2 diff = anchorPos - stackPos;
    diff.Normalize();

    double offset = innerRadius + (outerRadius - innerRadius) / 2.0;
    anchorLine->SetPoint2(stackPos.X() + diff.X() * offset, stackPos.Y() + diff.Y() * offset, position.Z());


    // Show the anchor actors if necessary
    diff = stackPos - location;
    if (diff.Magnitude() > outerRadius + anchorRadius) {
        renderer->AddViewProp(anchorActor);
        renderer->AddViewProp(anchorLineActor);
    }
    else {
        renderer->RemoveViewProp(anchorActor);
        renderer->RemoveViewProp(anchorLineActor);
    }
}


void Site::ResetPosition() {
    SetPosition(location);
}


const Vec3& Site::GetPosition() {
    return position;
}


const Vec3& Site::GetLocation() {
    return location;
}


void Site::SetRadius(double radius) {
    innerRadius = radius;
    outerRadius = radius * 1.5;
    spindleRadius = radius * 0.5;
    anchorRadius = radius * 0.5;
    for (int i = 0; i < (int)stacks.size(); i++) {
        stacks[i]->SetRadii(0.0, outerRadius, spindleRadius);
    }
    anchor->SetOuterRadius(anchorRadius);
    
    SetPosition(position);
}


void Site::SetJobSpacing(double spacing) {
    jobSpacing = spacing;

    StackJobs();
}   


void Site::SetSiteSpacing(double spacing) {
    siteSpacing = spacing;
} 


void Site::SetMaxStackSize(int size) {
    maxStackSize = size;

    Update();
}


void Site::SetRank(std::string rank) {
    // Update the color
    double rgb[3];
    lut->GetColor(atof(rank.c_str()), rgb);
    SetColor(rgb[0], rgb[1], rgb[2]);
}


void Site::SetColor(double r, double g, double b) {
    color[0] = r;
    color[1] = g;
    color[2] = b;

    for (int i = 0; i < (int)stacks.size(); i++) {
        stacks[i]->SetColor(r, g, b);
    }

    double anchorScale = 1.0;
    anchorActor->GetProperty()->SetColor(r * anchorScale, g * anchorScale, b * anchorScale);
    anchorLineActor->GetProperty()->SetColor(r * anchorScale, g * anchorScale, b * anchorScale);
}

const double* Site::GetColor() {
    return anchorActor->GetProperty()->GetColor();
}


double Site::GetOuterRadius() {
    return outerRadius;
}


void Site::AttachJob(Job* job) {
    if (job->GetSite()) {
        if (job->GetSite()->GetID() == id) {
            return;
        }
    }

    job->SetSite(this);
    jobs.push_back(job);

    if ((int)jobs.size() > mostNumJobs) mostNumJobs = (int)jobs.size();

    Update();
}

void Site::RemoveJob(std::string jobId) {
    for (int i = 0; i < (int)jobs.size(); i++) {
        if (jobs[i]->GetID() == jobId) {
            jobs.erase(jobs.begin() + i);
            break;
        }
    }

    Update();
}


void Site::AddNetworkConnection(NetworkConnection* connection) {
    connections.push_back(connection);
}


void Site::ShowSpindle(bool show) {
    showSpindle = show;

    Update();

    for (int i = 0; i < (int)stacks.size(); i++) {
        stacks[i]->ShowSpindle(show);
    }
}


void Site::ResetSpindle() {
    mostNumJobs = (int)jobs.size();

    for (int i = 0; i < (int)stacks.size(); i++) {
        stacks[i]->SetSpindleHeight(0.0);
    }

    Update();
}

void Site::Update() {
    ArrangeStacks();
    StackJobs();
}


void Site::StackJobs() {
    int stackNum = 0;
    Vec3 pos = stacks[stackNum]->GetPosition();
    double z = pos.Z() + jobOffset;

    double jobHeight = 0.0;
    for (int i = 0; i < (int)jobs.size(); i++) {
        jobHeight = jobs[i]->GetHeight();
        z += jobHeight * 0.5;

        if (jobs[i]->GetSite()->GetID() == id) {
            // This site owns this job
            jobs[i]->SetPosition(Vec3(pos.X(), pos.Y(), z));
        }
        else {
            // This job is travelling from this site
            jobs[i]->SetOldPosition(Vec3(pos.X(), pos.Y(), z));
        }

        z += jobHeight * 0.5 + jobSpacing;

        if (((i + 1) % maxStackSize) == 0) {
            stackNum++;
            pos = stacks[stackNum]->GetPosition();
            z = pos.Z() + 0.2;
        }
    }

    // Set to maximum height
    double spindleHeight = maxStackSize * jobHeight + (maxStackSize - 1) * jobSpacing - jobHeight * 0.5;
    for (int i = 0; i < (int)stacks.size() - 1; i++) {
        stacks[i]->SetSpindleHeight(spindleHeight);
    }

    // Set to height based on number of jobs at last stack
    int remainder = (mostNumJobs - maxStackSize * ((int)stacks.size() - 1));
    spindleHeight = remainder * jobHeight + (remainder - 1) * jobSpacing - jobHeight * 0.5;
    spindleHeight = spindleHeight < 0.1 ? 0.1 : spindleHeight;
    stacks.back()->SetSpindleHeight(spindleHeight);
}


void Site::ArrangeStacks() {
    int numStacks;

    if (showSpindle) {
        numStacks = mostNumJobs / maxStackSize + 1;
    }
    else {
        numStacks = (int)jobs.size() / maxStackSize + 1;
    }

    if (numStacks > (int)stacks.size()) {
        int numToAdd = numStacks - (int)stacks.size();
        for (int i = 0; i < numToAdd; i++) {
            stacks.push_back(new Stack(0.0, outerRadius, spindleRadius, resolution, showSpindle, renderer));
            stacks.back()->SetColor(color[0], color[1], color[2]);
        }
    }
    else if (numStacks < (int)stacks.size()) {
        int numToDelete = (int)stacks.size() - numStacks;
        for (int i = 0; i < numToDelete; i++) {
            delete stacks.back();
            stacks.pop_back();
        }
    }

    double separation = (innerRadius + (outerRadius - innerRadius) * 0.5) * 2.0;
    Vec3 pos(position.X() - separation * (double)((int)stacks.size() - 1) * 0.5,
             position.Y(),
             position.Z());

    for (int i = 0; i < (int)stacks.size(); i++) {
        stacks[i]->SetPosition(pos);
        pos.X() += separation;
    }
}


void Site::GetClosestStackCenter(const Vec2& pos, Vec2& stackPos) {
    double closestDist;
    for (int i = 0; i < (int)stacks.size(); i++) {
        Vec2 tempPos = stacks[i]->GetPosition();

        Vec2 diff = pos - tempPos;
        double dist = diff.Magnitude();

        if (i == 0) {
            closestDist = dist;
            stackPos = tempPos;
        }
        else {
            if (dist < closestDist) {
                closestDist = dist;
                stackPos = tempPos;
            }
        }
    }
}


int Site::GetNumStacks() {
    return (int)stacks.size();
}


Vec3 Site::GetStackPosition(int i) {
    return stacks[i]->GetPosition();
}



//////////////////////////////////////////////////////////////////////////


SpecialSite::SpecialSite(const std::string& siteName, double radius, vtkRenderer* ren, vtkColorTransferFunction* lookUpTable,
                         double jobSpacingDistance, double siteSpacingDistance, int maxSiteStackSize, bool showSiteSpindle, 
                         double* mapXYExtents, const Vec2& unknownPosition, const Vec2& offTheMapPosition, 
                         double labelHeight, bool labelFaceCamera, bool darkBackground) 
: Site(siteName, radius, ren, lookUpTable, 
       jobSpacingDistance, siteSpacingDistance, maxSiteStackSize, showSiteSpindle, 
       mapXYExtents, unknownPosition, offTheMapPosition, labelHeight, labelFaceCamera, darkBackground) {
    // Default gap value
    gap = 20.0;
    border = 5.0;


    CreatePlane(strip, stripActor);
    CreatePlane(leftBorder, leftBorderActor);
    CreatePlane(rightBorder, rightBorderActor);
    CreatePlane(bottomBorder, bottomBorderActor);
    CreatePlane(topBorder, topBorderActor);

    
    if (darkBackground) {
        stripActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
        leftBorderActor->GetProperty()->SetColor(0.5, 0.5, 0.5);
        rightBorderActor->GetProperty()->SetColor(0.5, 0.5, 0.5);
        bottomBorderActor->GetProperty()->SetColor(0.5, 0.5, 0.5);
        topBorderActor->GetProperty()->SetColor(0.5, 0.5, 0.5);
    }
    else {
        stripActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
        leftBorderActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
        rightBorderActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
        bottomBorderActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
        topBorderActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
    }

    UpdateStrip();
    UpdatePosition();
}


SpecialSite::~SpecialSite() {
    // Remove the actor
    renderer->RemoveViewProp(stripActor);
    renderer->RemoveViewProp(leftBorderActor);
    renderer->RemoveViewProp(rightBorderActor);
    renderer->RemoveViewProp(bottomBorderActor);
    renderer->RemoveViewProp(topBorderActor);

    // Clean up
    strip->Delete();
    leftBorder->Delete();
    rightBorder->Delete();
    bottomBorder->Delete();
    topBorder->Delete();
}


void SpecialSite::SetRadius(double radius) {
    Site::SetRadius(radius);
    UpdateStrip();
    UpdatePosition();
}


void SpecialSite::UpdateStrip() {
    strip->SetOrigin(mapExtents[0] - gap - 2.0 * (border + outerRadius), mapExtents[1], 0.0);
    strip->SetPoint1(mapExtents[0] - gap, mapExtents[1], 0.0);
    strip->SetPoint2(mapExtents[0] - gap - 2.0 * (border + outerRadius), mapExtents[3], 0.0);

    UpdateBorder();
}

void SpecialSite::UpdateBorder() {
    double borderWidth = 2.0;
    leftBorder->SetOrigin(strip->GetOrigin()[0] - borderWidth, strip->GetOrigin()[1], strip->GetOrigin()[2]);
    leftBorder->SetPoint1(leftBorder->GetOrigin()[0] + borderWidth, leftBorder->GetOrigin()[1], leftBorder->GetOrigin()[2]);
    leftBorder->SetPoint2(leftBorder->GetOrigin()[0], strip->GetPoint2()[1], leftBorder->GetOrigin()[2]);

    rightBorder->SetOrigin(strip->GetPoint1()[0], strip->GetPoint1()[1], strip->GetPoint1()[2]);
    rightBorder->SetPoint1(rightBorder->GetOrigin()[0] + borderWidth, rightBorder->GetOrigin()[1], rightBorder->GetOrigin()[2]);
    rightBorder->SetPoint2(rightBorder->GetOrigin()[0], strip->GetPoint2()[1], rightBorder->GetOrigin()[2]);

    bottomBorder->SetOrigin(leftBorder->GetOrigin()[0], strip->GetOrigin()[1] - borderWidth, strip->GetOrigin()[2]);
    bottomBorder->SetPoint1(rightBorder->GetPoint1()[0], bottomBorder->GetOrigin()[1], bottomBorder->GetOrigin()[2]);
    bottomBorder->SetPoint2(bottomBorder->GetOrigin()[0], bottomBorder->GetOrigin()[1] + borderWidth, bottomBorder->GetOrigin()[2]);

    topBorder->SetOrigin(leftBorder->GetOrigin()[0], strip->GetPoint2()[1], strip->GetOrigin()[2]);
    topBorder->SetPoint1(rightBorder->GetPoint1()[0], topBorder->GetOrigin()[1], topBorder->GetOrigin()[2]);
    topBorder->SetPoint2(topBorder->GetOrigin()[0], topBorder->GetOrigin()[1] + borderWidth, topBorder->GetOrigin()[2]);
}

void SpecialSite::UpdatePosition() {
    anchorActor->SetPosition(mapExtents[0] - gap - (border + outerRadius), mapExtents[1] + border + outerRadius, siteZ + anchorOffset);
    anchorLine->SetPoint1(anchorActor->GetPosition());
    SetPosition(Vec3(anchorActor->GetPosition()[0], anchorActor->GetPosition()[1], siteZ));
}


void SpecialSite::ArrangeStacks() {
    int numStacks;

    if (showSpindle) {
        numStacks = mostNumJobs / maxStackSize + 1;
    }
    else {
        numStacks = (int)jobs.size() / maxStackSize + 1;
    }

    if (numStacks > (int)stacks.size()) {
        int numToAdd = numStacks - (int)stacks.size();
        for (int i = 0; i < numToAdd; i++) {
            stacks.push_back(new Stack(innerRadius, outerRadius, spindleRadius, resolution, showSpindle, renderer));
            stacks.back()->SetColor(color[0], color[1], color[2]);
        }
    }
    else if (numStacks < (int)stacks.size()) {
        int numToDelete = (int)stacks.size() - numStacks;
        for (int i = 0; i < numToDelete; i++) {
            delete stacks.back();
            stacks.pop_back();
        }
    }

    double separation = (innerRadius + (outerRadius - innerRadius) * 0.5) * 2.0;
    Vec3 pos(position.X(), 
             position.Y() + separation * (double)((int)stacks.size() - 1),
             position.Z());

    for (int i = 0; i < (int)stacks.size(); i++) {
        stacks[i]->SetPosition(pos);
        pos.Y() -= separation;
    }
}


void SpecialSite::CreatePlane(vtkPlaneSource*& plane, vtkActor*& actor) {
    plane = vtkPlaneSource::New();
    plane->SetXResolution(1);
    plane->SetYResolution(1);
    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->SetInputConnection(plane->GetOutputPort());
    actor = vtkActor::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetAmbient(1.0);
    actor->GetProperty()->SetDiffuse(0.0);
    actor->GetProperty()->SetSpecular(0.0);
    actor->GetProperty()->SetOpacity(0.5);

    renderer->AddViewProp(actor);

    mapper->Delete();
}


//////////////////////////////////////////////////////////////////////////


DoneSite::DoneSite(const std::string& siteName, double radius, vtkRenderer* ren, vtkColorTransferFunction* lookUpTable,
                   double jobSpacingDistance, double siteSpacingDistance, int maxSiteStackSize, bool showSiteSpindle, 
                   double* mapXYExtents, const Vec2& unknownPosition, const Vec2& offTheMapPosition, 
                   double labelHeight, bool labelFaceCamera, bool darkBackground) 
: SpecialSite(siteName, radius, ren, lookUpTable, jobSpacingDistance, siteSpacingDistance, maxSiteStackSize, showSiteSpindle, 
              mapXYExtents, unknownPosition, offTheMapPosition, labelHeight, labelFaceCamera, darkBackground) {
    // Initialize variables
    numJobs = numDone = 0;

    UpdateStrip();
    UpdatePosition();
}


DoneSite::~DoneSite() {
}


void DoneSite::SetNumJobs(int num) {
    numJobs = num;

    SetCaption();
}


void DoneSite::UpdateStrip() {
    strip->SetOrigin(mapExtents[2] + gap, mapExtents[1], 0.0);
    strip->SetPoint1(mapExtents[2] + gap + 2.0 * (border + outerRadius), mapExtents[1], 0.0);
    strip->SetPoint2(mapExtents[2] + gap, mapExtents[3], 0.0);

    UpdateBorder();
}

void DoneSite::UpdatePosition() {
    anchorActor->SetPosition(mapExtents[2] + gap + border + outerRadius, mapExtents[1] + border + outerRadius, siteZ + anchorOffset);
    anchorLine->SetPoint1(anchorActor->GetPosition());
    SetPosition(Vec3(anchorActor->GetPosition()[0], anchorActor->GetPosition()[1], siteZ));
}


void DoneSite::StackJobs() {
    int stackNum = 0;
    Vec3 pos = stacks[stackNum]->GetPosition();
    double z = pos.Z() + jobOffset;

    numDone = 0;
    double jobHeight = 0.0;
    for (int i = 0; i < (int)jobs.size(); i++) {
        jobHeight = jobs[i]->GetHeight();
        z += jobHeight * 0.5;

        if (jobs[i]->GetSite()->GetID() == id) {
            // This site owns this job
            jobs[i]->SetPosition(Vec3(pos.X(), pos.Y(), z));

            if (jobs[i]->IsDone()) {
                numDone++;
            }
        }
        else {
            // This job is travelling from this site
            jobs[i]->SetOldPosition(Vec3(pos.X(), pos.Y(), z));
        }

        z += jobHeight * 0.5 + jobSpacing;

        if (((i + 1) % maxStackSize) == 0) {
            stackNum++;
            pos = stacks[stackNum]->GetPosition();
            z = pos.Z() + 0.2;
        }
    }

    // Set to maximum height
    double spindleHeight = maxStackSize * jobHeight + (maxStackSize - 1) * jobSpacing - jobHeight * 0.5;
    for (int i = 0; i < (int)stacks.size() - 1; i++) {
        stacks[i]->SetSpindleHeight(spindleHeight);
    }

    // Set to height based on number of jobs at last stack
    int remainder = (mostNumJobs - maxStackSize * ((int)stacks.size() - 1));
    spindleHeight = remainder * jobHeight + (remainder - 1) * jobSpacing - jobHeight * 0.5;
    spindleHeight = spindleHeight < 0.1 ? 0.1 : spindleHeight;
    stacks.back()->SetSpindleHeight(spindleHeight);

    SetCaption();
}


void DoneSite::SetCaption() {
    char buffer[32];
    sprintf_s(buffer, sizeof(buffer), ": %d / %d", numDone, numJobs);
    if (text) text->SetCaption(std::string(id + buffer).c_str());
    if (text3D) text3D->SetInput(std::string(id + buffer).c_str());
}
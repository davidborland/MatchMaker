///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        SiteList.cpp
//
// Author:      David Borland
//
// Description: Implementation of SiteList class for MatchMaker.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "SiteList.h"

#include <vtkTextProperty.h>

#include <wx/log.h>


SiteList::SiteList(vtkRenderer* ren, vtkRenderer* legendRen, bool useDarkBackground) : renderer(ren), legendRenderer(legendRen), darkBackground(useDarkBackground) {
    siteRadius = 10.0;
    jobSpacing = 5.0;
    siteSpacing = 30.0;

    maxStackSize = 5;

    showSpindles = true;

    showSiteRankingLegend = true;

    labelHeight = 0.0;
    labelFaceCamera = true;

    // Create the lookup table and scalar bar
    double colorScale = darkBackground ? 0.4 : 0.75;
    lut = vtkColorTransferFunction::New();
// Red-Yellow-Green
/*
    lut->AddRGBPoint(0.0, colorScale, 0.0, 0.0);
    lut->AddRGBPoint(600.0, colorScale, colorScale, 0.0);
    lut->AddRGBPoint(1000.0, 0.0, colorScale, 0.0);
*/
// Blue-Orange
    lut->AddRGBPoint(0.0, 0.0, 0.0, 0.5);
    lut->AddRGBPoint(1000.0, 1.0, 0.5, 0.0);

    scalarBar = vtkScalarBarActor::New();
    scalarBar->SetLookupTable(lut);
    scalarBar->SetPosition(0.925, 0.75);
    scalarBar->SetPosition2(0.075, 0.25);
    scalarBar->SetTitle("Site Ranks:");
    scalarBar->GetTitleTextProperty()->SetJustificationToLeft();
    scalarBar->GetTitleTextProperty()->BoldOff();
    scalarBar->GetTitleTextProperty()->ShadowOff();
    scalarBar->GetTitleTextProperty()->ItalicOff();
    scalarBar->GetLabelTextProperty()->BoldOff();
    scalarBar->GetLabelTextProperty()->ShadowOff();
    scalarBar->GetLabelTextProperty()->ItalicOff();
    scalarBar->SetLabelFormat("%-12.0f");
    if (darkBackground) {
        scalarBar->GetTitleTextProperty()->SetColor(1.0, 1.0, 1.0);
        scalarBar->GetLabelTextProperty()->SetColor(1.0, 1.0, 1.0);
    }
    else {
        scalarBar->GetTitleTextProperty()->SetColor(0.0, 0.0, 0.0);
        scalarBar->GetLabelTextProperty()->SetColor(0.0, 0.0, 0.0);
    }

    legendRenderer->AddViewProp(scalarBar);
}


SiteList::~SiteList() {
    for (int i = 0; i < (int)sites.size(); i++) {
        delete sites[i];
    }

    lut->Delete();
    scalarBar->Delete();
}


Site* SiteList::Get(const std::string& siteID) {
    // Search for this id
    for (int i = 0; i < (int)sites.size(); i++) {
        if (sites[i]->GetID() == siteID) {
            return sites[i];
        }
    }

    // It's not there, so add it    
    if (siteID == "MATCHING") {
        sites.push_back(new SpecialSite(siteID, siteRadius, renderer, lut, jobSpacing, siteSpacing, maxStackSize, showSpindles, mapExtents, unknownPos, offTheMapPos, labelHeight, labelFaceCamera, darkBackground));
    }
    else if (siteID == "DONE") {
        sites.push_back(new DoneSite(siteID, siteRadius, renderer, lut, jobSpacing, siteSpacing, maxStackSize, showSpindles, mapExtents, unknownPos, offTheMapPos, labelHeight, labelFaceCamera, darkBackground));
        doneSite = sites.back();
    }
    else {
        sites.push_back(new Site(siteID, siteRadius, renderer, lut, jobSpacing, siteSpacing, maxStackSize, showSpindles, mapExtents, unknownPos, offTheMapPos, labelHeight, labelFaceCamera, darkBackground));
    }

    return sites.back();
}


void SiteList::Arrange() {
    // Loop over all sites
    for (int i = 0; i < (int)sites.size(); i++) {
        // Don't do for the matching and done sites
        if (sites[i]->GetID() == "MATCHING" ||
            sites[i]->GetID() == "DONE") {
            continue;
        }

        // The vector to move the site by
        Vec2 vec;

        // This site's current position
        Vec2 pos(sites[i]->GetPosition().X(), sites[i]->GetPosition().Y());

        // This site's ideal location
        Vec2 loc(sites[i]->GetLocation().X(), sites[i]->GetLocation().Y());

        // Vector from the position to the location
        Vec2 locVec = loc - pos;

        // Do for all stacks at this site
        double numStacks = sites[i]->GetNumStacks();
        for (int q = 0; q < sites[i]->GetNumStacks(); q++) {
            Vec2 stackPos(sites[i]->GetStackPosition(q).X(), sites[i]->GetStackPosition(q).Y());

            // Loop over all sites
            for (int j = 0; j < (int)sites.size(); j++) {
                // Ignore this site
                if (j == i) { 
                    continue;
                }


                // The force to be computed
                Vec2 force;


                // Do for all stacks at the site
                double numStacks2 = sites[j]->GetNumStacks();
                for (int k = 0; k < sites[j]->GetNumStacks(); k++) {
                    Vec2 pos2(sites[j]->GetStackPosition(k).X(), sites[j]->GetStackPosition(k).Y());

                    // Compute the force
                    ComputeForce(force, stackPos, pos2);

                    // Sum the vectors
                    vec += force;
                }


                // Now do for the location
                // XXX : Removed for now, as the fact that these locations are fixed can lead 
                //       to "jittering" of site positions
//                Vec2 loc2 = sites[j]->GetLocation();

                // Compute the force
//                ComputeForce(force, stackPos, loc2);

                // Sum the vectors
//                vec += force;
            }
        }

        // Add the attractive force back to the site
        vec += locVec;

        // Enforce a maximum displacement
        double maxVecMag = 10.0;
        double vecMag = vec.Magnitude();
        if (vecMag > maxVecMag) {
            vec *= 1.0 / vecMag;
            vec *= maxVecMag;
        }

        // Move a litte more smoothly
        vec *= 0.25;

        // Set the position
        sites[i]->SetPosition(Vec3(pos.X() + vec.X(), pos.Y() + vec.Y(), sites[i]->GetPosition().Z()));
    }
}


double SiteList::GetJobSpacing() {
    return jobSpacing;
}


double SiteList::GetSiteSpacing() {
    return siteSpacing;
}


void SiteList::SetSiteRadius(double radius) {
    siteRadius = radius;
    for (int i = 0; i < (int)sites.size(); i++) {
        sites[i]->SetRadius(siteRadius);
    }
}


void SiteList::SetJobSpacing(double spacing) {
    jobSpacing = spacing;
    for (int i = 0; i < (int)sites.size(); i++) {
        sites[i]->SetJobSpacing(jobSpacing);
    }
}


void SiteList::SetSiteSpacing(double spacing) {
    siteSpacing = spacing;
}


int SiteList::GetMaxStackSize() {
    return maxStackSize;
}


void SiteList::SetMaxStackSize(int size) {
    maxStackSize = size;
    for (int i = 0; i < (int)sites.size(); i++) {
        sites[i]->SetMaxStackSize(maxStackSize);
    }
}


bool SiteList::GetShowSpindles() {
    return showSpindles;
}


void SiteList::ShowSpindles(bool show) {
    showSpindles = show;
    for (int i = 0; i < (int)sites.size(); i++) {
        sites[i]->ShowSpindle(showSpindles);
    }
}


void SiteList::SetMapExtents(const double* extents) {
    mapExtents[0] = extents[0];  
    mapExtents[1] = extents[1];
    mapExtents[2] = extents[2];
    mapExtents[3] = extents[3];
}

void SiteList::SetUnknownPos(const Vec2& position) {
    unknownPos = position;
}

void SiteList::SetOffTheMapPos(const Vec2& position) {
    offTheMapPos = position;
}


bool SiteList::GetShowSiteRankingLegend() {
    return showSiteRankingLegend;
}

void SiteList::ShowSiteRankingLegend(bool show) {
    showSiteRankingLegend = show;
    if (show) legendRenderer->AddViewProp(scalarBar);
    else legendRenderer->RemoveViewProp(scalarBar);
}


void SiteList::SetLabelHeight(double height) {
    labelHeight = height;
}

void SiteList::LabelFaceCamera(bool faceCamera) {
    labelFaceCamera = faceCamera;
}


void SiteList::ResetSpindles() {
    for (int i = 0; i < (int)sites.size(); i++) {
        sites[i]->ResetSpindle();
    } 
}


void SiteList::Update() {
    for (int i = 0; i < (int)sites.size(); i++) {
        sites[i]->Update();
    }
}


void SiteList::ComputeForce(Vec2& force, Vec2& pos, const Vec2& pos2) {
    // If coincident, give a random nudge
    if (pos == pos2) {
        pos.X() += ((double)rand() / (double)RAND_MAX - 0.5) * 2.0;
        pos.Y() += ((double)rand() / (double)RAND_MAX - 0.5) * 2.0;
    }

    // Get the vector between the positions
    Vec2 diff = pos - pos2;

    // Compute the distance
    double dist = diff.Magnitude();
    dist = dist < 0.1 ? 0.1 : dist;

    // This amounts to a scaled inverse cube repulsive force.
    double scale = siteSpacing / dist;
    force = diff * scale * scale * scale * scale;
}


void SiteList::Reset() {
    for (int i = 0; i < (int)sites.size(); i++) {
        delete sites[i];
    }
    sites.clear();
}
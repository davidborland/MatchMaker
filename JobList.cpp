///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        JobList.cpp
//
// Author:      David Borland
//
// Description: Implementation of JobList class for MatchMaker.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "JobList.h"

#include <wx/log.h>


JobList::JobList(Site* matchingSiteIn, DoneSite* doneSiteIn, vtkRenderer* ren, bool useDarkBackground)
: matchingSite(matchingSiteIn), doneSite(doneSiteIn), renderer(ren), darkBackground(useDarkBackground) {
    jobRadius = 10.0;
    jobHeight = jobRadius * 0.5;
    jobVelocity = 20.0;

    showGlyphs = Job::ShowStatusOnly;
    showGhosts = true;
    fadeGhosts = true;
    showPaths = true;
    showTrails = true;

    labelHeight = 0.0;
    labelFaceCamera = true;

    scienceLegend = vtkLegendBoxActor::New();
    CreateScienceLegend();
    CreateScienceColors();
}


JobList::~JobList() {
    for (int i = 0; i < (int)jobs.size(); i++) {
        delete jobs[i];
    }

    scienceLegend->Delete();
}


void JobList::SetDefaultSites(Site* matchingSiteIn, DoneSite* doneSiteIn) {
    matchingSite = matchingSiteIn;
    doneSite = doneSiteIn;
}


Job* JobList::Get(const std::string& jobId, WorkflowList* workflowList) {
    // Search for this Id
    for (int i = 0; i < (int)jobs.size(); i++) {
        if (jobs[i]->GetID() == jobId) {
            return jobs[i];
        }
    }

    // It's not there, so add it
    jobs.push_back(new Job(jobId, jobRadius, renderer, matchingSite, jobHeight, jobVelocity, showGlyphs, showGhosts, fadeGhosts, showPaths, showTrails, labelHeight, labelFaceCamera));
    jobs.back()->SetScienceColor(scienceColors[0].r, scienceColors[0].g, scienceColors[0].b);

    // Change default if a workflow is currently highlighted
    if (workflowList->IsCurrent()) {
        jobs.back()->SetOpacity(workflowList->GetFadedOpacity());
    }

    // Update the number of sites
    if (doneSite) doneSite->SetNumJobs((int)jobs.size());

    return jobs.back();
}


void JobList::UpdatePositions() {
    for (int i = 0; i < (int)jobs.size(); i++) {
        jobs[i]->UpdatePosition();
    }
}


double JobList::GetJobRadius() {
    return jobRadius;
}


double JobList::GetJobHeight() {
    return jobHeight;
}


double JobList::GetJobVelocity() {
    return jobVelocity;
}


void JobList::SetJobRadius(double radius) {
    jobRadius = radius;
    for (int i = 0; i < (int)jobs.size(); i++) {
        jobs[i]->SetRadius(jobRadius);
    }
}


void JobList::SetJobHeight(double height) {
    jobHeight = height;
    for (int i = 0; i < (int)jobs.size(); i++) {
        jobs[i]->SetHeight(jobHeight);
    }
}


void JobList::SetJobVelocity(double velocity) {
    jobVelocity = velocity;
    for (int i = 0; i < (int)jobs.size(); i++) {
        jobs[i]->SetVelocity(jobVelocity);
    }
}


Job::ShowGlyphType JobList::GetShowGlyphs() {
    return showGlyphs;
}

bool JobList::GetShowGhosts() {
    return showGhosts;
}

bool JobList::GetFadeGhosts() {
    return fadeGhosts;
}

bool JobList::GetShowPaths() {
    return showPaths;
}

bool JobList::GetShowTrails() {
    return showTrails;
}


void JobList::ShowGlyphs(Job::ShowGlyphType show) {
    showGlyphs = show;
    for (int i = 0; i < (int)jobs.size(); i++) {
        jobs[i]->ShowGlyphs(showGlyphs);
    }
}

void JobList::ShowGhosts(bool show) {
    showGhosts = show;
    for (int i = 0; i < (int)jobs.size(); i++) {
        jobs[i]->ShowGhost(showGhosts);
    }
}

void JobList::FadeGhosts(bool fade) {
    fadeGhosts = fade;
    for (int i = 0; i < (int)jobs.size(); i++) {
        jobs[i]->FadeGhost(fadeGhosts);
    }
}

void JobList::ShowPaths(bool show) {
    showPaths = show;
    for (int i = 0; i < (int)jobs.size(); i++) {
        jobs[i]->ShowPath(showPaths);
    }
}

void JobList::ShowTrails(bool show) {
    showTrails = show;
    for (int i = 0; i < (int)jobs.size(); i++) {
        jobs[i]->ShowTrail(showTrails);
    }
}


void JobList::SetLabelHeight(double height) {
    labelHeight = height;
}

void JobList::LabelFaceCamera(bool faceCamera) {
    labelFaceCamera = faceCamera;
}


void JobList::RemoveDuplicates(std::vector<std::string> jobIDs) {
    for (int i = 0; i < (int)jobs.size(); i++) {
        for (int j = 0; j < (int)jobIDs.size(); j++) {
            if (jobs[i]->GetID() == jobIDs[j]) {
                delete jobs[i];
                jobs.erase(jobs.begin() + i);
                i--;
                break;
            }
        }
    }
}


const double* JobList::GetScienceColor(std::string& science) {
    for (int i = 0; i < (int)sciences.size(); i++) {
        if (science == sciences[i]) {
            scienceColor[0] = scienceColors[i].r;
            scienceColor[1] = scienceColors[i].g;
            scienceColor[2] = scienceColors[i].b;

            return scienceColor;
        }
    }

    // New science
    sciences.push_back(science);

    // If we've run out of colors, resort to random
    if (scienceColors.size() < sciences.size()) {
        Color color;
        color.r = (double)rand() / RAND_MAX;
        color.g = (double)rand() / RAND_MAX;
        color.b = (double)rand() / RAND_MAX;

        double intensity = color.r + color.g + color.b;
        if (darkBackground) {
            double minIntensity = 0.1 * 3.0;
            if (intensity < minIntensity) {
                double scale = (minIntensity - intensity) / 3.0;
                color.r += scale;
                color.g += scale;
                color.b += scale;
            }
        }
        else {
            double maxIntensity = 0.9 * 3.0;
            if (intensity > maxIntensity) {
                double scale = (intensity - maxIntensity) / 3.0;
                color.r -= scale;
                color.g -= scale;
                color.b -= scale;   
            }
        }
        scienceColors.push_back(color);
    }

    scienceColor[0] = scienceColors.back().r;
    scienceColor[1] = scienceColors.back().g;
    scienceColor[2] = scienceColors.back().b;

    UpdateScienceLegend();

    return scienceColor;
}

vtkLegendBoxActor* JobList::GetScienceLegend() {
    return scienceLegend;
}


void JobList::Reset() {
    for (int i = 0; i < (int)jobs.size(); i++) {
        delete jobs[i];
    }
    jobs.clear();

    sciences.clear();
    scienceColors.clear();
    CreateScienceLegend();
}


void JobList::CreateScienceLegend() {    
    srand(1);

    // Set an unknown science color
    sciences.push_back("Unknown");
    Color color;
    color.r = 0.5;
    color.g = 0.5;
    color.b = 0.5;
    scienceColors.push_back(color);

    scienceLegend->BorderOff();

    UpdateScienceLegend();
}

void JobList::UpdateScienceLegend() {
    double color[3];
    if (darkBackground) color[0] = color[1] = color[2] = 1.0;
    else color[0] = color[1] = color[2] = 0.0;

    scienceLegend->SetNumberOfEntries(sciences.size() + 1);
    scienceLegend->SetEntry(0, (vtkImageData*)NULL, "Science Types:", color);
    for (int i = 0; i < (int)sciences.size(); i++) {
        color[0] = scienceColors[i].r;
        color[1] = scienceColors[i].g;
        color[2] = scienceColors[i].b;
        std::string s = "\t";
        s += sciences[i];
        scienceLegend->SetEntry(i + 1, (vtkImageData*)NULL, s.c_str(), color);
    }

    scienceLegend->SetPosition(0.0, 0.0);

    double x = 0.075;
    double y = 0.025 * (sciences.size() + 1);
    scienceLegend->SetPosition2(x, y);
}

void JobList::CreateScienceColors() {
    // Create colors
    std::vector<Color> temp;

    double step = 0.3;
    double start = 0.0;
    double stop = 0.6;
    for (double r = start; r <= stop; r += step) {
        for (double g = start; g <= stop; g += step) {
            for (double b = start; b <= stop; b += step) {
                Color c;
                c.r = r;
                c.g = g;
                c.b = b;
                temp.push_back(c);
            }
        }
    }

    // Randomize order
    int numColors = temp.size();
    for (int i = 0; i < numColors; i++) {
        int index = rand() % temp.size();
        scienceColors.push_back(temp[index]);
        temp.erase(temp.begin() + index);
    }
}
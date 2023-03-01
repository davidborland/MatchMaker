///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Workflow.cpp
//
// Author:      David Borland
//
// Description: Implementation of Workflow class for MatchMaker.  Workflows have an ID and a
//              list of pointers to jobs in the workflow.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "Workflow.h"

#include <wx/log.h>


Workflow::Workflow(const std::string& workflowID, bool isFaded, double fadedOpacityValue) 
: id(workflowID), faded(isFaded), fadedOpacity(fadedOpacityValue) {
    showLabels = false;

    name = "name";
    username = "username";
}

Workflow::~Workflow() {
}


const std::string& Workflow::GetID() {
    return id;
}

const std::string& Workflow::GetName() {
    return name;
}

const std::string& Workflow::GetUsername() {
    return username;
}

void Workflow::SetName(const std::string& workflowName) {
    name = workflowName;
}

void Workflow::SetUsername(const std::string& workflowUsername) {
    username = workflowUsername;
}


void Workflow::InsertJob(Job* job) {
    jobs.push_back(job);

    if (faded) {
        jobs.back()->SetOpacity(fadedOpacity);
    }
    else {
        jobs.back()->SetOpacity(1.0);
    }
    jobs.back()->ShowName(showLabels);
}


double Workflow::GetFadedOpacity() {
    return fadedOpacity;
}

void Workflow::SetFadedOpacity(double opacity) {
    fadedOpacity = opacity;
    if (faded) Fade();
}


void Workflow::MakeOpaque() {
    faded = false;
    for (int i = 0; i < (int)jobs.size(); i++) {
        jobs[i]->SetOpacity(1.0);
    }
}

void Workflow::Fade() {
    faded = true;
    for (int i = 0; i < (int)jobs.size(); i++) {
        jobs[i]->SetOpacity(fadedOpacity);
    }
}


void Workflow::ShowLabels(bool show) {
    showLabels = show;
    for (int i = 0; i < (int)jobs.size(); i++) {
        jobs[i]->ShowName(showLabels);
    }
}


std::vector<std::string> Workflow::RemoveDuplicates(Job *job) {
    std::vector<std::string> jobIDs;

    // First pass to see if in this workflow
    bool here = false;
    for (int i = 0; i < (int)jobs.size(); i++) {
        if (jobs[i]->GetID() == job->GetID()) {
            here = true;
            break;
        }
    }

    if (!here) return jobIDs;

    // The job is in this workflow, find duplicates
    for (int i = 0; i < (int)jobs.size(); i++) {
        if (jobs[i]->GetID() != job->GetID()) {
            if (jobs[i]->GetName() == job->GetName()) {
                jobIDs.push_back(jobs[i]->GetID());
                jobs.erase(jobs.begin() + i);
                i--;
            }
        }
    }

    return jobIDs;
}
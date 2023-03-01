///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        WorkflowList.cpp
//
// Author:      David Borland
//
// Description: Implementation of WorkFlowList class for MatchMaker.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "WorkflowList.h"

#include <vtkTextProperty.h>

#include <wx/log.h>


WorkflowList::WorkflowList(vtkRenderer* legendRen, bool darkBackground) : legendRenderer(legendRen) {
    current = -1;

    // Create text label for the current workflow
    text = vtkLegendBoxActor::New();
    double color[3];
    if (darkBackground) color[0] = color[1] = color[2] = 1.0;
    else color[0] = color[1] = color[2] = 0.0;
    text->SetNumberOfEntries(1);
    text->SetEntry(0, (vtkImageData*)NULL, "", color);
    text->SetPosition(0.5, 0.08);
    text->SetWidth(1.0);
    text->SetHeight(0.03);
    text->GetEntryTextProperty()->SetJustificationToCentered();
    text->BorderOff();

    // Don't add to the renderer yet
}

WorkflowList::~WorkflowList() {
    for (int i = 0; i < (int)workflows.size(); i++) {
        delete workflows[i];
    }
    text->Delete();
}


Workflow* WorkflowList::Get(const std::string& workflowID) {
    // Search for this id
    for (int i = 0; i < (int)workflows.size(); i++) {
        if (workflows[i]->GetID() == workflowID) {
            return workflows[i];
        }
    }

    workflows.push_back(new Workflow(workflowID, IsCurrent(), fadedOpacity));

    return workflows.back();
}


bool WorkflowList::IsCurrent() {
    return current != -1;
}

void WorkflowList::ChangeCurrent() {
    // Increment and check against number of workflows
    current++;
    current = current >= (int)workflows.size() ? -1 : current;

    Update();
}

void WorkflowList::Update() {
    // Fade and set text as needed
    legendRenderer->RemoveViewProp(text);
    if (current == -1) {
        SetDefaultLabel(defaultLabel);
    }

    for (int i = 0; i < (int)workflows.size(); i++) {
        if (current == -1) {
            workflows[i]->MakeOpaque();
            workflows[i]->ShowLabels(false);
        }
        else {
            if (i == current) {
                workflows[i]->MakeOpaque();
                workflows[i]->ShowLabels(true);

                std::string s = workflows[i]->GetUsername() + ": " + workflows[i]->GetName();
                text->SetEntryString(0, s.c_str());        
                legendRenderer->AddViewProp(text);
            }
            else {
                workflows[i]->Fade();
                workflows[i]->ShowLabels(false);
            }
        }
    }
}


double WorkflowList::GetFadedOpacity() {
    return fadedOpacity;
}

void WorkflowList::SetFadedOpacity(double opacity) {
    fadedOpacity = opacity;
    for (int i = 0; i < (int)workflows.size(); i++) {
        workflows[i]->SetFadedOpacity(fadedOpacity);
    }
}


void WorkflowList::SetDefaultLabel(const std::string& defaultWorkflowLabel) {
    defaultLabel = defaultWorkflowLabel;

    std::string label = defaultLabel;

    if (label.size() > 0) {
        text->SetEntryString(0, label.c_str());
        legendRenderer->AddViewProp(text);
    }
}


std::vector<std::string> WorkflowList::RemoveDuplicates(Job* job) {
    std::vector<std::string> jobIDs;
    for (int i = 0; i < (int)workflows.size(); i++) {
        jobIDs = workflows[i]->RemoveDuplicates(job);
        if (jobIDs.size() > 0) break;
    }
    return jobIDs;
}


void WorkflowList::Reset() {  
    for (int i = 0; i < (int)workflows.size(); i++) {
        delete workflows[i];
    }
    workflows.clear();

    current = -1;
    Update();
}

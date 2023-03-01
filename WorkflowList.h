///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        WorkflowList.h
//
// Author:      David Borland
//
// Description: Interface of WorkFlowList class for MatchMaker.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef WORKFLOWLIST_H
#define WORKFLOWLIST_H


#include "Workflow.h"

#include <vtkLegendBoxActor.h>
#include <vtkRenderer.h>

#include <string>
#include <vector>

#include "Job.h"


class WorkflowList {
public:
    WorkflowList(vtkRenderer* legendRen, bool darkBackground);
    ~WorkflowList();

    Workflow* Get(const std::string& workflowID);

    bool IsCurrent();
    void ChangeCurrent();
    void Update();

    double GetFadedOpacity();
    void SetFadedOpacity(double opacity);

    void SetDefaultLabel(const std::string& defaultWorkflowLabel);

    std::vector<std::string> RemoveDuplicates(Job* job);

    // Reset the data
    void Reset();

private:
    std::vector<Workflow*> workflows;

    int current;

    double fadedOpacity;

    std::string defaultLabel;

    vtkRenderer* legendRenderer;
    vtkLegendBoxActor* text;
};


#endif
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Workflow.h
//
// Author:      David Borland
//
// Description: Interface of Workflow class for MatchMaker.  Workflows have an ID and a
//              list of pointers to jobs in the workflow.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef WORKFLOW_H
#define WORKFLOW_H


#include <string>
#include <vector>

#include "Job.h"


class Workflow {
public:
    Workflow(const std::string& workflowID, bool isFaded, double FadedOpacityValue);
    ~Workflow();

    const std::string& GetID();
    const std::string& GetName();
    const std::string& GetUsername();

    void SetName(const std::string& workflowName);
    void SetUsername(const std::string& workflowUsername);

    void InsertJob(Job* job);

    double GetFadedOpacity();
    void SetFadedOpacity(double opacity);

    void MakeOpaque();
    void Fade();

    void ShowLabels(bool show);

    std::vector<std::string> RemoveDuplicates(Job* job);

private:
    std::string id;
    std::string name;
    std::string username;

    // The jobs in this workflow.  These are pointers to Jobs in the Engine's Job vector.
    // They are neither created nor destroyed here.
    std::vector<Job*> jobs;

    bool faded;
    double fadedOpacity;

    bool showLabels;
};


#endif
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        JobList.h
//
// Author:      David Borland
//
// Description: Interface of JobList class for MatchMaker.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef JOBLIST_H
#define JOBLIST_H


#include <vector>

#include <vtkRenderer.h>
#include <vtkLegendBoxActor.h>

#include "Job.h"
#include "Site.h"
#include "WorkflowList.h"


class JobList {
public:
    JobList(Site* matchingSiteIn, DoneSite* doneSiteIn, vtkRenderer* ren, bool useDarkBackground);
    ~JobList();

    // Set the default sites
    void SetDefaultSites(Site* matchingSiteIn, DoneSite* doneSiteIn);

    // Get a job, creating it if necessary
    Job* Get(const std::string& jobId, WorkflowList* workflowList);

    // Animate the jobs
    void UpdatePositions();

    // Get/set job parameters
    double GetJobRadius();
    double GetJobHeight();
    double GetJobVelocity();

    void SetJobRadius(double radius);
    void SetJobHeight(double height);
    void SetJobVelocity(double velocity);

    // Get/set visualization effects
    Job::ShowGlyphType GetShowGlyphs();
    bool GetShowGhosts();
    bool GetFadeGhosts();
    bool GetShowPaths();
    bool GetShowTrails();

    void ShowGlyphs(Job::ShowGlyphType show);
    void ShowGhosts(bool show);
    void FadeGhosts(bool fade);
    void ShowPaths(bool show);
    void ShowTrails(bool show);

    // Text label height
    void SetLabelHeight(double height);
    void LabelFaceCamera(bool jobLabelFaceCamera);

    void RemoveDuplicates(std::vector<std::string> jobIDs);

    // Get the color for this science
    const double* GetScienceColor(std::string& science);

    // Get the science legend
    vtkLegendBoxActor* GetScienceLegend();

    // Reset the data
    void Reset();

private:
    // List of jobs
    std::vector<Job*> jobs;

    // Start site and done site
    Site* matchingSite;
    DoneSite* doneSite;

    // Job parameters
    double jobRadius;
    double jobHeight;
    double jobVelocity;

    // Visualization effects
    Job::ShowGlyphType showGlyphs;
    bool showGhosts;
    bool fadeGhosts;
    bool showPaths;
    bool showTrails;

    double labelHeight;
    bool labelFaceCamera;

    bool darkBackground;

    // Science names and colors
    std::vector<std::string> sciences;    
    typedef struct Color {
        double r;
        double g; 
        double b;
    };
    std::vector<Color> scienceColors;
    double scienceColor[3];

    vtkRenderer* renderer;

    vtkLegendBoxActor* scienceLegend;

    void CreateScienceLegend();
    void UpdateScienceLegend();

    void CreateScienceColors();
};


#endif
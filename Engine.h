///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Engine.h
//
// Author:      David Borland
//
// Description: Interface of Engine class for MatchMaker.  This class is a container for
//              the data structures and other classes that do the work for the application.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef ENGINE_H
#define ENGINE_H


#include <string>
#include <vector>

#include <vtkRenderWindowInteractor.h>

#include "Job.h"
#include "JobList.h"
#include "NetworkConnectionList.h"
#include "RenderPipeline.h"
#include "Site.h"
#include "SiteList.h"
#include "Socket.h"
#include "TextFileSocket.h"
#include "WorkflowList.h"


class Engine {
public:
    Engine();
    ~Engine();

    void UpdateSocket();
    void UpdateGraphics();

    int GetInitialSocketReadInterval();
    int GetInitialGraphicsUpdateInterval();
    int GetResetSeconds();

    Socket* GetSocket();

    JobList* GetJobList();
    SiteList* GetSiteList();
    WorkflowList* GetWorkflowList();

    RenderPipeline* GetRenderPipeline();

    const std::vector<std::string>& GetHostDescriptions();
    int GetNumHosts();

    const std::vector<std::string>& GetDataFileDescriptions();
    int GetNumDataFiles();

    void TogglePause();

    bool GetUseSocket();

    void Reset();
    void UseSocket();
    void UseFile();
    void UseHost(int host);
    void UseDataFile(int dataFile);

    void ChangeWorkflow();

private:
    // Socket for reading data
    Socket* socket;

    // Data sources
    std::vector<std::string> hostDescriptions;
    std::vector<std::string> hostNames;
    std::vector<int> ports;
    std::vector<std::string> dataFileDescriptions;
    std::vector<std::string> dataFileNames;

    // VTK render pipeline
    RenderPipeline* pipeline;

    // Lists of objects
    JobList* jobList;
    SiteList* siteList;
    WorkflowList* workflowList;
    NetworkConnectionList* networkConnectionList;

    // Keypress callback
    KeyPressCallback* keyPressCallback;

    // Done site or not
    bool useDoneSite;

    // Dark background or not
    bool darkBackground;

    // For pausing/unpausing
    bool pause;

    // Reading from the socket or not
    bool useSocket;
    int hostIndex;
    int dataFileIndex;

    // Timer intervals, in milliseconds
    int initialSocketReadInterval;
    int initialGraphicsUpdateInterval;
   
    // Timer interval, in seconds
    int resetSeconds;

    // Functions for parsing data read from the socket
    void ParseSocketData(std::string& s);
    std::vector<std::string> Tokenize(const std::string& s, const std::string& delimiters);

    // Create default sites
    void CreateDefaultSites(Site* & matching, Site* & done);

    // Reset data
    void ResetData();
};


#endif
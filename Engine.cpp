///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Engine.h
//
// Author:      David Borland
//
// Description: Implementation of Engine class for MatchMaker.  This class is a container for
//              the data structures and other classes that do the work for the application.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "Engine.h"

#include "ConfigFileParser.h"
#include "Projector.h"

#include <wx/log.h>


Engine::Engine() {    
    // Parse the configuration file
    std::string filename = "MatchMaker.cfg";
    ConfigFileParser* parser = new ConfigFileParser();
    if (!parser->Parse(filename)) {
        wxLogMessage("Parsing %s failed.  Using default values.", filename.c_str());
    }


    // Get timer Intervals
    initialSocketReadInterval = parser->GetSocketReadInterval();
    initialGraphicsUpdateInterval = parser->GetGraphicsUpdateInterval();
    resetSeconds = parser->GetResetSeconds();


    // Set the x and y scale
    Projector::SetXScale(parser->GetXScale());
    Projector::SetYScale(parser->GetYScale());


    // Dark or light background
    darkBackground = parser->DarkBackground();


    // Scale the job color based on background
    if (darkBackground) Job::ScaleColor(0.75);
    else Job::ScaleColor(1.0);


    // Create the VTK pipeline
    pipeline = new RenderPipeline(parser->GetLogoFileNames(), parser->GetMapFileName(),
                                  parser->GetLabelHeight(), parser->LabelFaceCamera(), darkBackground);
    int x, y;
    parser->GetFullSize(x, y);
    pipeline->SetFullSize(x, y);
    if (parser->FullScreen()) pipeline->ToggleFullScreen();
    if (parser->Stereo()) pipeline->ToggleStereo();
    if (parser->Dome()) pipeline->ToggleDome();
    pipeline->ShowMap(parser->ShowMap());
    pipeline->ShowLogos(parser->ShowLogos());
    pipeline->RotateLogos(parser->RotateLogos());


    // Create the list of sites
    siteList = new SiteList(pipeline->GetRenderer(), pipeline->GetLegendRenderer(), darkBackground);
    siteList->SetSiteRadius(parser->GetObjectRadius());
    siteList->SetJobSpacing(parser->GetJobSpacing());
    siteList->ShowSpindles(parser->ShowSiteSpindles());
    siteList->SetSiteSpacing(parser->GetSiteSpacing());
    siteList->SetMaxStackSize(parser->GetMaxStackSize());
    siteList->SetMapExtents(pipeline->GetMapExtents());
    siteList->SetUnknownPos(pipeline->GetUnknownPos());
    siteList->SetOffTheMapPos(pipeline->GetOffTheMapPos());
    siteList->ShowSiteRankingLegend(parser->ShowSiteRankingLegend());
    siteList->SetLabelHeight(parser->GetLabelHeight());
    siteList->LabelFaceCamera(parser->LabelFaceCamera());

    // Create start and done sites
    useDoneSite = parser->UseDoneSite();

    Site* matching = NULL;
    Site* done = NULL;
    CreateDefaultSites(matching, done);


    // Create the list of jobs
    jobList = new JobList(matching, static_cast<DoneSite*>(done), pipeline->GetRenderer(), darkBackground);
    jobList->SetJobRadius(parser->GetObjectRadius());
    jobList->SetJobHeight(parser->GetJobHeight());
    jobList->SetJobVelocity(parser->GetJobVelocity());
    jobList->ShowGlyphs(parser->GetShowGlyphs());
    jobList->ShowGhosts(parser->ShowGhostJobs());
    jobList->FadeGhosts(parser->FadeGhostJobs());
    jobList->ShowPaths(parser->ShowJobPaths());
    jobList->ShowTrails(parser->ShowJobTrails());
    jobList->SetLabelHeight(parser->GetLabelHeight());
    jobList->LabelFaceCamera(parser->LabelFaceCamera());
    
    // Set the science legend
    pipeline->SetScienceLegend(jobList->GetScienceLegend());

    // Which legends to show on startup
    if (jobList->GetShowGlyphs() == Job::ShowStatusOnly) {
        pipeline->ShowStatusLegend(true);
        pipeline->ShowScienceLegend(false);
    }
    else if (jobList->GetShowGlyphs() == Job::ShowScienceOnly) {
        pipeline->ShowStatusLegend(false);
        pipeline->ShowScienceLegend(true);
    }
    else if (jobList->GetShowGlyphs() == Job::ShowStatusAndScience) {
        pipeline->ShowStatusLegend(true);
        pipeline->ShowScienceLegend(true);
    }


    // Create the list of workflows
    workflowList = new WorkflowList(pipeline->GetLegendRenderer(), darkBackground);
    workflowList->SetFadedOpacity(parser->GetFadedOpacity());


    // Create the list of network connections
    networkConnectionList = new NetworkConnectionList(pipeline->GetRenderer(), darkBackground);

    
    // Keypress callback
    keyPressCallback = KeyPressCallback::New();
    keyPressCallback->SetEngineAndPipeline(this, pipeline);
    pipeline->GetInteractor()->AddObserver(vtkCommand::KeyPressEvent, keyPressCallback); 


    // Connect the socket
    hostDescriptions = parser->GetHostDescriptions();
    hostNames = parser->GetHostNames();
    ports = parser->GetPorts();
    dataFileDescriptions = parser->GetDataFileDescriptions();
    dataFileNames = parser->GetDataFileNames();
    useSocket = parser->UseSocket();
    hostIndex = 0;
    dataFileIndex = 0;

    if (hostNames.size() == 0) {
        hostDescriptions.push_back("Default : nantahala.renci.org:9001");
        hostNames.push_back("nantahala.renci.org");
        ports.push_back(9001);
    }
    if (dataFileNames.size() == 0) {
        dataFileDescriptions.push_back("Default : Data/OSG.data");
        dataFileNames.push_back("Data/OSG.data");
    }

    if (useSocket) {
        socket = new Socket(parser->GetSocketReadAll());
        socket->Init(hostNames[hostIndex].c_str(), ports[hostIndex]);
        workflowList->SetDefaultLabel(hostDescriptions[hostIndex]);
    }
    else {
        socket = new TextFileSocket(parser->GetSocketReadAll());
        socket->Init(dataFileNames[dataFileIndex].c_str());        
        workflowList->SetDefaultLabel(dataFileDescriptions[dataFileIndex]);
    }

    // No pause to start
    pause = false;
}


Engine::~Engine() {
    // Clean up
    keyPressCallback->Delete();
    delete socket;
    delete siteList;
    delete jobList;
    delete workflowList;
    delete pipeline;
}


void Engine::UpdateSocket() {
    if (pause) return;

    std::string s;

    socket->Read(s);

    if (!s.empty()) {
        ParseSocketData(s);
    }

    s.clear();
}

void Engine::UpdateGraphics() {
    if (pause) return;

    siteList->Arrange();
    jobList->UpdatePositions();
    pipeline->Update();

    pipeline->Render();
}


int Engine::GetInitialSocketReadInterval() {
    return initialSocketReadInterval;
}


int Engine::GetInitialGraphicsUpdateInterval() {
    return initialGraphicsUpdateInterval;
}


int Engine::GetResetSeconds() {
    return resetSeconds;
}


Socket* Engine::GetSocket() {
    return socket;
}


JobList* Engine::GetJobList() {
    return jobList;
}

SiteList* Engine::GetSiteList() {
    return siteList;
}

WorkflowList* Engine::GetWorkflowList() {
    return workflowList;
}


RenderPipeline* Engine::GetRenderPipeline() {
    return pipeline;
}


const std::vector<std::string>& Engine::GetHostDescriptions() {
    return hostDescriptions;
}

int Engine::GetNumHosts() {
    return hostNames.size();
}


const std::vector<std::string>& Engine::GetDataFileDescriptions() {
    return dataFileDescriptions;
}

int Engine::GetNumDataFiles() {
    return dataFileNames.size();
}


void Engine::TogglePause() {
    pause = !pause;
}


bool Engine::GetUseSocket() {
    return useSocket;
}


void Engine::Reset() {
    bool readAll = socket->GetReadAll();

    delete socket;

    if (useSocket) {
        socket = new Socket(readAll);
        socket->Init(hostNames[hostIndex].c_str(), ports[hostIndex]);
        workflowList->SetDefaultLabel(hostDescriptions[hostIndex]);
    }
    else {
        socket = new TextFileSocket(readAll);
        socket->Init(dataFileNames[dataFileIndex].c_str());
        workflowList->SetDefaultLabel(dataFileDescriptions[dataFileIndex]);
    }
    
    ResetData();
}


void Engine::UseSocket() {
    useSocket = true;

    Reset();
}

void Engine::UseFile() {
    useSocket = false;

    Reset();
}

void Engine::UseHost(int host) {
    hostIndex = host;

    if (useSocket) {
        Reset();
    }
}

void Engine::UseDataFile(int dataFile) {
    dataFileIndex = dataFile;

    if (!useSocket) {
        Reset();
    }
}


void Engine::ChangeWorkflow() {
    workflowList->ChangeCurrent();
}   


void Engine::ParseSocketData(std::string& s) {
//    wxLogMessage("%s", s.c_str());

    // Create a vector of the lines in the string
    std::vector<std::string> lines = Tokenize(s, "\n");

    // Parse each line
    for (int i = 0; i < (int)lines.size(); i++) {
        // If the line is "EOF", reset the data.
        if (lines[i] == "EOF") {
            ResetData();
            return;
        }

        std::vector<std::string> tokens = Tokenize(lines[i], " \n");

        // Check for something
        if (tokens.size() <= 0) {
            wxLogMessage("Empty line");
            continue;
        }

        // Validate
        if (tokens.size() == 1 && tokens[0] == "ping") {
            // Ignore
            continue;
        }

        if (tokens[2] == "longlat" && tokens.size() == 5) {
            // Okay, do nothing
        }        
        else if (tokens[0] == "workflow" && tokens.size() == 6) {
            // Okay, do nothing
        }
        else if (tokens[0] == "network_bandwidth" && tokens.size() == 6) {
            // Okay, do nothing
        }
        else if (tokens[2] == "data_source" && tokens.size() == 10) {
            // Okay, do nothing
        }
        else if (tokens.size() == 4 || tokens.size() == 6) {
            // Okay, do nothing
        }
        else {
            // Wrong number of tokens
            wxLogMessage("Invalid number of tokens: %s", lines[i].c_str());
            continue;
        }

        // Parse the line
        if (tokens[0] == "job") {
            std::string jobID = tokens[1];
            std::string command = tokens[2];

            // Get or create this job
            Job* job = jobList->Get(jobID, workflowList);

            if (command == "state") {
                std::string state = tokens[3];

                if (!job->SetState(state)) {
                    wxLogMessage("Invalid job state: %s", state.c_str());
                    continue;
                }

                // Check for science
                if (tokens.size() == 6) {
                    if (tokens[4] == "science") {
                        std::string science = tokens[5];

                        const double* color = jobList->GetScienceColor(science);

                        job->SetScienceColor(color[0], color[1], color[2]);
                    }
                }
/*
// For testing colors
else {
    std::vector<std::string> scienceNames;
/*
    for (int i = 0; i < 20; i++) {
        std::string name = "Science_";
        char number[16];
        sprintf(number, "%d", i);
        name += number;
        scienceNames.push_back(name);
    }
*/
/*
    scienceNames.push_back("Math");
    scienceNames.push_back("Biology");
    scienceNames.push_back("Chemistry");
    scienceNames.push_back("Physics");
    scienceNames.push_back("Geology");
    scienceNames.push_back("Astronomy");
    scienceNames.push_back("Bioinformatics");
    scienceNames.push_back("Medicine");
    scienceNames.push_back("Alchemy");
    scienceNames.push_back("Parapsychology");

    int index = rand() % scienceNames.size();

    const double* color = jobList->GetScienceColor(scienceNames[index]);

    job->SetScienceColor(color[0], color[1], color[2]);
}
*/

                if (job->IsDone() && job->GetName().size() > 0) {
                    // Check for duplicates
                    std::vector<std::string> jobIDs = workflowList->RemoveDuplicates(job);

                    if (jobIDs.size() > 0) jobList->RemoveDuplicates(jobIDs);
                }
            }
            else if (command == "tosite") {
                std::string siteID = tokens[3];

                // Get or create this site
                Site* site = siteList->Get(siteID);
 
                site->AttachJob(job);
            }
            else if (command == "workflow") {
                std::string workflowID = tokens[3];

                // Get or create this workflow
                Workflow* workflow = workflowList->Get(workflowID);

                workflow->InsertJob(job);
            }
            else if (command == "job_name") {
                std::string jobName = tokens[3];

                // Set the name
                job->SetName(jobName);

                // XXX : Check for duplicates
            }
            else if (command == "data_source") {
                std::string dataSourceID = tokens[3];
                std::string dataSinkID = tokens[5];
                double dataSize = atof(tokens[7].c_str());

                // Ignore if sourceID and destID are the same or dataSize <= 0.0
                if (dataSourceID == dataSinkID || dataSize <= 0.0) continue;

                // Get or create these sites
                Site* dataSource = siteList->Get(dataSourceID);
                Site* dataSink = siteList->Get(dataSinkID);

                // Get or create this network connection
                NetworkConnection* connection = networkConnectionList->Get(dataSource, dataSink);

                // Start the data transfer
                job->StartDataTransfer(dataSource, dataSink, connection, dataSize);
            }   
            else if (command == "localid") {
                // Ignore for now
                continue;
            }
            else {
                wxLogMessage("Invalid job command: %s", command.c_str());
                continue;
            }
        }
        else if (tokens[0] == "site") {
            std::string siteID = tokens[1];
            std::string command = tokens[2];

            // Get or create this site
            Site* site = siteList->Get(siteID);

            if (command == "rank") {
                std::string rank = tokens[3];               

                site->SetRank(rank);
            }
            else if (command == "longlat") {
                std::string longitude = tokens[3];
                std::string latitude = tokens[4];

                site->SetLongLat(atof(longitude.c_str()), atof(latitude.c_str()));
            }
            else {
                wxLogMessage("Invalid site command", command.c_str());
                continue;
            }
        }
        else if (tokens[0] == "workflow") {
            std::string workflowID = tokens[1];
            std::string username = tokens[3];
            std::string workflowName = tokens[5];

            // Get or create this workflow
            Workflow* workflow = workflowList->Get(workflowID);

            workflow->SetUsername(username);
            workflow->SetName(workflowName);
        }
        else if (tokens[0] == "network_bandwidth") {
            std::string sourceID = tokens[2];
            std::string destID = tokens[4];
            double bandwidth = atof(tokens[5].c_str());

            // Ignore if sourceID and destID are the same or bandwidth <= 0.0
            if (sourceID == destID || bandwidth <= 0.0) continue;

            // Get or create these sites
            Site* source = siteList->Get(sourceID);
            Site* dest = siteList->Get(destID);

            // Get or create this network connection
            NetworkConnection* connection = networkConnectionList->Get(source, dest);

            // Set the bandwidth
            connection->SetBandwidth(bandwidth);
        }
        else {
            wxLogMessage("Invalid command: %s", tokens[0].c_str());
            continue;
        }
    }

    pipeline->Render();
}
    

std::vector<std::string> Engine::Tokenize(const std::string& s, const std::string& delimiters) {
    std::vector<std::string> tokens;

    // Skip delimiters at the beginning
    std::string::size_type lastPos = s.find_first_not_of(delimiters, 0);

    // Look for first token
    std::string::size_type pos = s.find_first_of(delimiters, lastPos);

    while (pos != std::string::npos || lastPos != std::string::npos) {
        // Found a token, add it to the vector
        tokens.push_back(s.substr(lastPos, pos - lastPos));

        // Skip delimiters
        lastPos = s.find_first_not_of(delimiters, pos);

        // Look for next token
        pos = s.find_first_of(delimiters, lastPos);
    }

    return tokens;
}


void Engine::CreateDefaultSites(Site* & matching, Site* & done) {
    double colorScale = darkBackground ? 0.4 : 0.75;

    // Create a default site
    matching = siteList->Get("MATCHING");
    matching->SetColor(colorScale, colorScale, colorScale);

    // Create a done site
    if (useDoneSite) {
        done = siteList->Get("DONE");
        done->SetColor(colorScale, 0.0, colorScale);
    }
}


void Engine::ResetData() {
    // Reset lists
    jobList->Reset();
    siteList->Reset();
    workflowList->Reset();
    networkConnectionList->Reset();

    // Create default sites
    Site* matching = NULL;
    Site* done = NULL;
    CreateDefaultSites(matching, done);
    jobList->SetDefaultSites(matching, static_cast<DoneSite*>(done));
}   

///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        MatchMaker.cpp
//
// Author:      David Borland
//
// Description: Sets up wxWidgets for MatchMaker application
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "MatchMaker.h"

#include <string>

#include <wx/hyperlink.h>


// Create the event table for the main frame
BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_TIMER(SocketTimerId, MainFrame::OnTimer)
    EVT_TIMER(GraphicsTimerId, MainFrame::OnTimer)
    EVT_TIMER(ResetTimerId, MainFrame::OnTimer)

    EVT_BUTTON(ResetButtonId, MainFrame::OnButton)
    EVT_RADIOBUTTON(UseSocketButtonId, MainFrame::OnRadioButton)
    EVT_RADIOBUTTON(UseFileButtonId, MainFrame::OnRadioButton)
    EVT_RADIOBUTTON(HostButtonsId, MainFrame::OnRadioButton)
    EVT_RADIOBUTTON(DataFileButtonsId, MainFrame::OnRadioButton)

    EVT_BUTTON(SocketControlsButtonId, MainFrame::OnButton)
    EVT_BUTTON(GraphicsControlsButtonId, MainFrame::OnButton)
END_EVENT_TABLE()

// Create the event table for the MatchMaker control frame class
BEGIN_EVENT_TABLE(MatchMakerFrame, wxFrame)
    EVT_CLOSE(MatchMakerFrame::OnClose)
END_EVENT_TABLE()

// Create the event table for the Socket frame
BEGIN_EVENT_TABLE(SocketFrame, MatchMakerFrame)
    EVT_CHECKBOX(SocketReadAllCheckBoxId, SocketFrame::OnCheckBox)

    EVT_SCROLL_THUMBTRACK(SocketFrame::OnScrollThumbtrack)
    EVT_SCROLL_CHANGED(SocketFrame::OnScrollChanged)
END_EVENT_TABLE()

// Create the event table for the Graphics frame
BEGIN_EVENT_TABLE(GraphicsFrame, MatchMakerFrame)
    EVT_BUTTON(ResetSiteSpindlesButtonId, GraphicsFrame::OnButton)

    EVT_RADIOBUTTON(ShowStatusOnlyButtonId, GraphicsFrame::OnRadioButton)
    EVT_RADIOBUTTON(ShowScienceOnlyButtonId, GraphicsFrame::OnRadioButton)
    EVT_RADIOBUTTON(ShowStatusAndScienceButtonId, GraphicsFrame::OnRadioButton)

    EVT_CHECKBOX(ShowGhostJobsCheckBoxId, GraphicsFrame::OnCheckBox)
    EVT_CHECKBOX(FadeGhostJobsCheckBoxId, GraphicsFrame::OnCheckBox)
    EVT_CHECKBOX(ShowJobPathsCheckBoxId, GraphicsFrame::OnCheckBox)
    EVT_CHECKBOX(ShowJobTrailsCheckBoxId, GraphicsFrame::OnCheckBox)
    EVT_CHECKBOX(ShowSiteSpindlesCheckBoxId, GraphicsFrame::OnCheckBox)
    EVT_CHECKBOX(ShowSiteRankingLegendCheckBoxId, GraphicsFrame::OnCheckBox)
    EVT_CHECKBOX(ShowMapCheckBoxId, GraphicsFrame::OnCheckBox)
    EVT_CHECKBOX(ShowLogosCheckBoxId, GraphicsFrame::OnCheckBox)
    EVT_CHECKBOX(RotateLogosCheckBoxId, GraphicsFrame::OnCheckBox)

    EVT_SCROLL_THUMBTRACK(GraphicsFrame::OnScrollThumbtrack)
    EVT_SCROLL_CHANGED(GraphicsFrame::OnScrollChanged)
END_EVENT_TABLE()

// Create a new application object
IMPLEMENT_APP(MatchMakerApp)


/////////////////////////////////////////////////////////////////////////////////////////////
// MatchMakerApp
/////////////////////////////////////////////////////////////////////////////////////////////

// Program execution starts here
bool MatchMakerApp::OnInit() {
    // Create the main application window
    MainFrame* frame = new MainFrame("MatchMaker Controls", wxDefaultPosition, wxDefaultSize);

    // Show it.  Frames, unlike simple controls, are not shown initially when created.
    frame->Show();

    // Tell wxWidgets that this is a main frame
    SetTopWindow(frame);

    // Success:  wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// MainFrame
/////////////////////////////////////////////////////////////////////////////////////////////

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame((wxFrame*) NULL, wxID_ANY, title, pos, size) {
    // Initialize frames
    socketFrame = NULL;
    graphicsFrame = NULL;

    // Create a log window for printing messages
    log = new MatchMakerLogWindow(this, "Log Window", true, false);

    // Set the frame icon
//    SetIcon(wxICON(MatchMaker));

    
    // Create the main engine
    engine = new Engine();


    // Create a panel
    wxPanel* panel = new wxPanel(this);

    // Create buttons
    resetButton = new wxButton(panel, ResetButtonId, "Reset", wxDefaultPosition, wxSize(213, -1));

    useSocketButton = new wxRadioButton(panel, UseSocketButtonId, "Read from socket", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    useFileButton = new wxRadioButton(panel, UseFileButtonId, "Read from file");
    useSocketButton->SetValue(engine->GetUseSocket());
    useFileButton->SetValue(!engine->GetUseSocket());

    // Add host descriptions.  Only numHosts are active.
    std::vector<std::string> hostDescriptions = engine->GetHostDescriptions();
    int numHosts = engine->GetNumHosts();

    if (hostDescriptions.size() >= 1) {
        hostButtons.push_back(new wxRadioButton(panel, HostButtonsId, hostDescriptions[0].c_str(), wxDefaultPosition, wxDefaultSize, wxRB_GROUP));
    }
    for (int i = 1; i < (int)hostDescriptions.size(); i++) {
        hostButtons.push_back(new wxRadioButton(panel, HostButtonsId, hostDescriptions[i].c_str()));

        if (i >= numHosts) {
            hostButtons.back()->Disable();
        }
    }

    // Add data file descriptions.  Only numDataFiles are active though.
    std::vector<std::string> dataFileDescriptions = engine->GetDataFileDescriptions();
    int numDataFiles = engine->GetNumDataFiles();

    if (dataFileDescriptions.size() >= 1) {
        dataFileButtons.push_back(new wxRadioButton(panel, DataFileButtonsId, dataFileDescriptions[0].c_str(), wxDefaultPosition, wxDefaultSize, wxRB_GROUP));
    }
    for (int i = 1; i < (int)dataFileDescriptions.size(); i++) {
        dataFileButtons.push_back(new wxRadioButton(panel, DataFileButtonsId, dataFileDescriptions[i].c_str()));

        if (i >= numDataFiles) {
            dataFileButtons.back()->Disable();
        }
    }

    // Buttons to pop-up control windows
    socketControlsButton = new wxButton(panel, SocketControlsButtonId, "Socket controls");
    graphicsControlsButton = new wxButton(panel, GraphicsControlsButtonId, "Graphics controls");

    // A hyperlink to Mats' status webpage
    wxHyperlinkCtrl* hyperlink = new wxHyperlinkCtrl(panel, wxID_ANY, "Status Page", "http://www.renci.org/~rynge/osg/status/");
    hyperlink->SetVisitedColour(wxColour(200, 0, 200));

    
    // Put the widgets in a sizer
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(resetButton, 0, wxEXPAND | wxALL, 5);
    sizer->Add(useSocketButton, 0, wxEXPAND | wxALL, 5);
    for (int i = 0; i < (int)hostButtons.size(); i++) {
        sizer->Add(hostButtons[i], 0, wxEXPAND | wxLEFT, 20);
        sizer->AddSpacer(2);
    }    
    sizer->AddSpacer(5);
    sizer->Add(useFileButton, 0, wxEXPAND | wxALL, 5);    
    for (int i = 0; i < (int)dataFileButtons.size(); i++) {
        sizer->Add(dataFileButtons[i], 0, wxEXPAND | wxLEFT, 20);
        sizer->AddSpacer(2);
    }
    sizer->AddSpacer(10);
    sizer->Add(socketControlsButton, 0, wxEXPAND | wxALL, 5);
    sizer->Add(graphicsControlsButton, 0, wxEXPAND | wxALL, 5);
    sizer->Add(hyperlink, 0, wxEXPAND | wxALL, 5);

    panel->SetSizer(sizer);
    sizer->SetSizeHints(this);


    // Start the socket timer
    socketTimer = new wxTimer(this, SocketTimerId);
    socketTimer->Start(engine->GetInitialSocketReadInterval());

    // Start the graphics timer
    graphicsTimer = new wxTimer(this, GraphicsTimerId);
    graphicsTimer->Start(engine->GetInitialGraphicsUpdateInterval());

    // Start the reset timer
    resetTimer = new wxTimer(this, ResetTimerId);
    if (engine->GetResetSeconds() > 0) {
        resetTimer->Start(engine->GetResetSeconds() * 1000);
    }
}


MainFrame::~MainFrame() {
    // Clean up
    delete socketTimer;
    delete graphicsTimer;
    delete resetTimer;

    delete engine;
}


void MainFrame::OnTimer(wxTimerEvent& e) {
    if (e.GetId() == SocketTimerId) {
        engine->UpdateSocket();
    }
    else if (e.GetId() == GraphicsTimerId) {
        engine->UpdateGraphics();
    }
    else if (e.GetId() == ResetTimerId) {
        engine->Reset();
    }
}


void MainFrame::OnButton(wxCommandEvent& e) {
    if (e.GetId() == ResetButtonId) {
        engine->Reset();
    }
    else if (e.GetId() == SocketControlsButtonId) {
        if (!socketFrame) {
            socketFrame = new SocketFrame(this, "Socket Controls", wxDefaultPosition, wxDefaultSize, engine);
        }
        socketFrame->Show();
        socketFrame->Raise();
    }
    else if (e.GetId() == GraphicsControlsButtonId) {
        if (!graphicsFrame) {
            graphicsFrame = new GraphicsFrame(this, "Graphics Controls", wxDefaultPosition, wxDefaultSize, engine);
        }
        graphicsFrame->Show();
        graphicsFrame->Raise();
    }
}


void MainFrame::OnRadioButton(wxCommandEvent& e) {
    if (e.GetId() == UseSocketButtonId) {
        engine->UseSocket();
    }
    else if (e.GetId() == UseFileButtonId) {
        engine->UseFile();
    }
    else if (e.GetId() == HostButtonsId) {
        int i;
        for (i = 0; i < (int)hostButtons.size(); i++) {
            if (hostButtons[i]->GetValue()) {
                break;
            }
        }
        if (i < (int)hostButtons.size()) {
            engine->UseHost(i);
        }
    }
    else if (e.GetId() == DataFileButtonsId) {
        int i;
        for (i = 0; i < (int)dataFileButtons.size(); i++) {
            if (dataFileButtons[i]->GetValue()) {
                break;
            }
        }
        if (i < (int)dataFileButtons.size()) {
            engine->UseDataFile(i);
        }
    }
}


wxTimer* MainFrame::GetSocketTimer() {
    return socketTimer;
}


wxTimer* MainFrame::GetGraphicsTimer() {
    return graphicsTimer;
}



/////////////////////////////////////////////////////////////////////////////////////////////
// MatchMakerFrame
/////////////////////////////////////////////////////////////////////////////////////////////

MatchMakerFrame::MatchMakerFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, Engine* theEngine)
: wxFrame(parent, wxID_ANY, title, pos, size), engine(theEngine) {
    mainFrame = static_cast<MainFrame*>(parent);
}

MatchMakerFrame::~MatchMakerFrame(){
}

void MatchMakerFrame::OnClose(wxCloseEvent& e) {
    // Check to see if we can veto
    if (!e.CanVeto()) {
        // Can't veto, so go ahead and destroy
        Destroy();
    }
    else {
        // Can veto, so just hide it for now
        e.Veto();
        Hide();
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////
// SocketFrame
/////////////////////////////////////////////////////////////////////////////////////////////

SocketFrame::SocketFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, Engine* engine)
: MatchMakerFrame(parent, title, pos, size, engine) {
    wxPanel* panel = new wxPanel(this);

    socketReadAllCheckBox = new wxCheckBox(panel, SocketReadAllCheckBoxId, "Read all data");
    socketReadAllCheckBox->SetValue(engine->GetSocket()->GetReadAll());

    socketReadIntervalSlider = new wxSlider(panel, SocketReadIntervalSliderId, mainFrame->GetSocketTimer()->GetInterval(), 1, 5000, 
                                        wxDefaultPosition, wxSize(200, -1), wxSL_HORIZONTAL | wxSL_LABELS);
    wxStaticBoxSizer* socketReadIntervalSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Socket read interval (ms)");
    socketReadIntervalSizer->Add(socketReadIntervalSlider, 0, wxEXPAND, 0);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->Add(socketReadAllCheckBox, 0, wxEXPAND | wxALL, border);
    sizer->Add(socketReadIntervalSizer, 0, wxEXPAND | wxALL, border);

    panel->SetSizer(sizer);
    sizer->SetSizeHints(this);
}


void SocketFrame::OnCheckBox(wxCommandEvent& e) {
    if (e.GetId() == SocketReadAllCheckBoxId) {
        engine->GetSocket()->SetReadAll(e.IsChecked());
    }
}


void SocketFrame::OnScrollThumbtrack(wxScrollEvent& e) {
    if (e.GetId() == SocketReadIntervalSliderId) {
        mainFrame->GetSocketTimer()->Start(e.GetInt());
    }
}


void SocketFrame::OnScrollChanged(wxScrollEvent& e) {
    if (e.GetId() == SocketReadIntervalSliderId) {
        mainFrame->GetSocketTimer()->Start(e.GetInt());
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsFrame
/////////////////////////////////////////////////////////////////////////////////////////////

GraphicsFrame::GraphicsFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, Engine* engine)
: MatchMakerFrame(parent, title, pos, size, engine) {
    wxPanel* panel = new wxPanel(this);

    // Graphics update interval
    graphicsUpdateIntervalSlider = new wxSlider(panel, GraphicsUpdateIntervalSliderId, mainFrame->GetGraphicsTimer()->GetInterval(), 1, 100, 
                                     wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
    wxStaticBoxSizer* graphicsUpdateIntervalSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Graphics update interval (ms)");
    graphicsUpdateIntervalSizer->Add(graphicsUpdateIntervalSlider, 0, wxEXPAND, 0);

    // Object radius
    objectRadiusSlider = new wxSlider(panel, ObjectRadiusSliderId, engine->GetJobList()->GetJobRadius(), 1, 50, 
                                      wxDefaultPosition, wxSize(200, -1), wxSL_HORIZONTAL | wxSL_LABELS);
    wxStaticBoxSizer* objectRadiusSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Object radius");
    objectRadiusSizer->Add(objectRadiusSlider, 0, wxEXPAND, 0);

    // Job height
    jobHeightSlider = new wxSlider(panel, JobHeightSliderId, engine->GetJobList()->GetJobHeight(), 1, 10, 
                                   wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
    wxStaticBoxSizer* jobHeightSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Job height");
    jobHeightSizer->Add(jobHeightSlider, 0, wxEXPAND, 0);

    // Job spacing
    jobSpacingSlider = new wxSlider(panel, JobSpacingSliderId, engine->GetSiteList()->GetJobSpacing(), 1, 10, 
                                    wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
    wxStaticBoxSizer* jobSpacingSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Job spacing");
    jobSpacingSizer->Add(jobSpacingSlider, 0, wxEXPAND, 0);

    // Job velocity 
    jobVelocitySlider = new wxSlider(panel, JobVelocitySliderId, engine->GetJobList()->GetJobVelocity(), 1, 200, 
                                     wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
    wxStaticBoxSizer* jobVelocitySizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Job velocity");
    jobVelocitySizer->Add(jobVelocitySlider, 0, wxEXPAND, 0);

    // Site spacing
    siteSpacingSlider = new wxSlider(panel, SiteSpacingSliderId, engine->GetSiteList()->GetSiteSpacing(), 10, 150, 
                                     wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
    wxStaticBoxSizer* siteSpacingSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Site spacing");
    siteSpacingSizer->Add(siteSpacingSlider, 0, wxEXPAND, 0);

    // Maximum stack size
    maxStackSizeSlider = new wxSlider(panel, MaxStackSizeSliderId, engine->GetSiteList()->GetMaxStackSize(), 5, 50, 
                                      wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
    wxStaticBoxSizer* maxStackSizeSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Maximum stack size");
    maxStackSizeSizer->Add(maxStackSizeSlider, 0, wxEXPAND, 0);


    // Radio buttons for what to display
    showStatusOnlyButton = new wxRadioButton(panel, ShowStatusOnlyButtonId, "Show status only",
                                             wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    showScienceOnlyButton = new wxRadioButton(panel, ShowScienceOnlyButtonId, "Show science only");
    showStatusAndScienceButton = new wxRadioButton(panel, ShowStatusAndScienceButtonId, "Show status and science");

    if (engine->GetJobList()->GetShowGlyphs() == Job::ShowStatusOnly) {
        showStatusOnlyButton->SetValue(true);
    }
    else if (engine->GetJobList()->GetShowGlyphs() == Job::ShowScienceOnly) {
        showScienceOnlyButton->SetValue(true);
    }
    else if (engine->GetJobList()->GetShowGlyphs() == Job::ShowStatusAndScience) {
        showStatusAndScienceButton->SetValue(true);
    }
    

    // Show ghost jobs
    showGhostJobsCheckBox = new wxCheckBox(panel, ShowGhostJobsCheckBoxId, "Show ghost jobs");
    showGhostJobsCheckBox->SetValue(engine->GetJobList()->GetShowGhosts());

    // Fade ghost jobs
    fadeGhostJobsCheckBox = new wxCheckBox(panel, FadeGhostJobsCheckBoxId, "Fade ghost jobs");
    fadeGhostJobsCheckBox->SetValue(engine->GetJobList()->GetFadeGhosts());

    // Show job paths
    showJobPathsCheckBox = new wxCheckBox(panel, ShowJobPathsCheckBoxId, "Show job paths");
    showJobPathsCheckBox->SetValue(engine->GetJobList()->GetShowPaths());

    // Show job trails
    showJobTrailsCheckBox = new wxCheckBox(panel, ShowJobTrailsCheckBoxId, "Show job trails");
    showJobTrailsCheckBox->SetValue(engine->GetJobList()->GetShowTrails());

    // Show site spindles
    showSiteSpindlesCheckBox = new wxCheckBox(panel, ShowSiteSpindlesCheckBoxId, "Show site spindles");
    showSiteSpindlesCheckBox->SetValue(engine->GetSiteList()->GetShowSpindles());


    // Reset site spindles
    resetSiteSpindlesButton = new wxButton(panel, ResetSiteSpindlesButtonId, "Reset site spindles", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);


    // Faded opacity
    fadedOpacitySlider = new wxSlider(panel, FadedOpacitySliderId, engine->GetWorkflowList()->GetFadedOpacity() * 100, 0, 100, 
                                             wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
    wxStaticBoxSizer* fadedOpacitySizer = new wxStaticBoxSizer(wxVERTICAL, panel, "Faded opacity");
    fadedOpacitySizer->Add(fadedOpacitySlider, 0, wxEXPAND, 0);


    // Show site ranking legend
    showSiteRankingLegendCheckBox = new wxCheckBox(panel, ShowSiteRankingLegendCheckBoxId, "Show site ranking legend");
    showSiteRankingLegendCheckBox->SetValue(engine->GetSiteList()->GetShowSiteRankingLegend());


    // Show map
    showMapCheckBox = new wxCheckBox(panel, ShowMapCheckBoxId, "Show map");
    showMapCheckBox->SetValue(engine->GetRenderPipeline()->GetShowMap());


    // Show logos
    showLogosCheckBox = new wxCheckBox(panel, ShowLogosCheckBoxId, "Show logos");
    showLogosCheckBox->SetValue(engine->GetRenderPipeline()->GetShowLogos());

    // Rotate logos
    rotateLogosCheckBox = new wxCheckBox(panel, RotateLogosCheckBoxId, "Rotate logos");
    rotateLogosCheckBox->SetValue(engine->GetRenderPipeline()->GetRotateLogos());


    // Create the sizers
    wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* vSizer1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* vSizer2 = new wxBoxSizer(wxVERTICAL);

    // Add the widgets to the sizers
    vSizer1->Add(graphicsUpdateIntervalSizer, 0, wxEXPAND | wxALL, border);
    vSizer1->Add(objectRadiusSizer, 0, wxEXPAND | wxALL, border);
    vSizer1->Add(jobHeightSizer, 0, wxEXPAND | wxALL, border);
    vSizer1->Add(jobSpacingSizer, 0, wxEXPAND | wxALL, border);
    vSizer1->Add(jobVelocitySizer, 0, wxEXPAND | wxALL, border);
    vSizer1->Add(siteSpacingSizer, 0, wxEXPAND | wxALL, border);
    vSizer1->Add(maxStackSizeSizer, 0, wxEXPAND | wxALL, border);

    vSizer2->Add(showStatusOnlyButton, 0, wxEXPAND | wxALL, border);
    vSizer2->Add(showScienceOnlyButton, 0, wxEXPAND | wxALL, border);
    vSizer2->Add(showStatusAndScienceButton, 0, wxEXPAND | wxALL, border);
    vSizer2->AddSpacer(border);
    vSizer2->Add(showGhostJobsCheckBox, 0, wxEXPAND | wxALL, border);
    vSizer2->Add(fadeGhostJobsCheckBox, 0, wxEXPAND | wxALL, border);
    vSizer2->Add(showJobPathsCheckBox, 0, wxEXPAND | wxALL, border);
    vSizer2->Add(showJobTrailsCheckBox, 0, wxEXPAND | wxALL, border);
    vSizer2->Add(showSiteSpindlesCheckBox, 0, wxEXPAND | wxALL, border);
    vSizer2->Add(resetSiteSpindlesButton, 0, wxEXPAND | wxALL, border);
    vSizer2->Add(fadedOpacitySizer, 0, wxEXPAND | wxALL, border);
    vSizer2->Add(showSiteRankingLegendCheckBox, 0, wxEXPAND | wxALL, border);
    vSizer2->Add(showMapCheckBox, 0, wxEXPAND | wxALL, border);
    vSizer2->Add(showLogosCheckBox, 0, wxEXPAND | wxALL, border);
    vSizer2->Add(rotateLogosCheckBox, 0, wxEXPAND | wxALL, border);

    hSizer->Add(vSizer1, 0, wxEXPAND | wxALL, 0);
    hSizer->Add(vSizer2, 0, wxEXPAND | wxALL, 0);

    // Make the sizer and the panel work well together
    panel->SetSizer(hSizer);
    hSizer->SetSizeHints(this);
}


void GraphicsFrame::OnButton(wxCommandEvent& e) {
    if (e.GetId() == ResetSiteSpindlesButtonId) {
        engine->GetSiteList()->ResetSpindles();
    }
}

void GraphicsFrame::OnRadioButton(wxCommandEvent& e) {
    if (e.GetId() == ShowStatusOnlyButtonId) {
        engine->GetJobList()->ShowGlyphs(Job::ShowStatusOnly);
        engine->GetRenderPipeline()->ShowStatusLegend(true);
        engine->GetRenderPipeline()->ShowScienceLegend(false);
    }
    else if (e.GetId() == ShowScienceOnlyButtonId) {        
        engine->GetJobList()->ShowGlyphs(Job::ShowScienceOnly);
        engine->GetRenderPipeline()->ShowStatusLegend(false);
        engine->GetRenderPipeline()->ShowScienceLegend(true);
    }
    else if (e.GetId() == ShowStatusAndScienceButtonId) {
        engine->GetJobList()->ShowGlyphs(Job::ShowStatusAndScience);
        engine->GetRenderPipeline()->ShowStatusLegend(true);
        engine->GetRenderPipeline()->ShowScienceLegend(true);
    }
}

void GraphicsFrame::OnCheckBox(wxCommandEvent& e) {
    if (e.GetId() == ShowGhostJobsCheckBoxId) {
        engine->GetJobList()->ShowGhosts(e.IsChecked());
    }
    else if (e.GetId() == FadeGhostJobsCheckBoxId) {
        engine->GetJobList()->FadeGhosts(e.IsChecked());
    }
    else if (e.GetId() == ShowJobPathsCheckBoxId) {
        engine->GetJobList()->ShowPaths(e.IsChecked());
    }
    else if (e.GetId() == ShowJobTrailsCheckBoxId) {
        engine->GetJobList()->ShowTrails(e.IsChecked());
    }
    else if (e.GetId() == ShowSiteSpindlesCheckBoxId) {
        engine->GetSiteList()->ShowSpindles(e.IsChecked());
    }       
    else if (e.GetId() == ShowSiteRankingLegendCheckBoxId) {
        engine->GetSiteList()->ShowSiteRankingLegend(e.IsChecked());
    }
    else if (e.GetId() == ShowMapCheckBoxId) {
        engine->GetRenderPipeline()->ShowMap(e.IsChecked());
    }
    else if (e.GetId() == ShowLogosCheckBoxId) {
        engine->GetRenderPipeline()->ShowLogos(e.IsChecked());
    }
    else if (e.GetId() == RotateLogosCheckBoxId) {
        engine->GetRenderPipeline()->RotateLogos(e.IsChecked());
    }
}


void GraphicsFrame::OnScrollThumbtrack(wxScrollEvent& e) {
    if (e.GetId() == GraphicsUpdateIntervalSliderId) {
        mainFrame->GetGraphicsTimer()->Start(e.GetInt());
    }
    else if (e.GetId() == ObjectRadiusSliderId) {
        engine->GetJobList()->SetJobRadius(e.GetInt());
        engine->GetSiteList()->SetSiteRadius(e.GetInt());
    }
    else if (e.GetId() == JobHeightSliderId) {
        engine->GetJobList()->SetJobHeight(e.GetInt());
        engine->GetSiteList()->Update();
    }
    else if (e.GetId() == JobSpacingSliderId) {
        engine->GetSiteList()->SetJobSpacing(e.GetInt());
    }
    else if (e.GetId() == JobVelocitySliderId) {
        engine->GetJobList()->SetJobVelocity(e.GetInt());
    }
    else if (e.GetId() == SiteSpacingSliderId) {
        engine->GetSiteList()->SetSiteSpacing(e.GetInt());
    }
    else if (e.GetId() == MaxStackSizeSliderId) {
        engine->GetSiteList()->SetMaxStackSize(e.GetInt());
    }
    else if (e.GetId() == FadedOpacitySliderId) {
        engine->GetWorkflowList()->SetFadedOpacity((double)e.GetInt() / 100.0);
    }
}


void GraphicsFrame::OnScrollChanged(wxScrollEvent& e) {
    if (e.GetId() == GraphicsUpdateIntervalSliderId) {
        mainFrame->GetGraphicsTimer()->Start(e.GetInt());
    }
    else if (e.GetId() == ObjectRadiusSliderId) {
        engine->GetJobList()->SetJobRadius(e.GetInt());
        engine->GetSiteList()->SetSiteRadius(e.GetInt());
    }
    else if (e.GetId() == JobHeightSliderId) {
        engine->GetJobList()->SetJobHeight(e.GetInt());
        engine->GetSiteList()->Update();
    }
    else if (e.GetId() == JobSpacingSliderId) {
        engine->GetSiteList()->SetJobSpacing(e.GetInt());
    }
    else if (e.GetId() == JobVelocitySliderId) {
        engine->GetJobList()->SetJobVelocity(e.GetInt());
    }   
    else if (e.GetId() == MaxStackSizeSliderId) {
        engine->GetSiteList()->SetMaxStackSize(e.GetInt());
    }    
    else if (e.GetId() == FadedOpacitySliderId) {
        engine->GetWorkflowList()->SetFadedOpacity((double)e.GetInt() / 100.0);
    }
}

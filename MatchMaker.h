///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        MatchMaker.h
//
// Author:      David Borland
//
// Description: Sets up wxWidgets for MatchMaker application
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef MATCHMAKER_H
#define MATCHMAKER_H


#include <wx/wx.h>

#include "Engine.h"


// Forward declarations
class MatchMakerLogWindow;
class SocketFrame;
class GraphicsFrame;


// Event IDs
enum {
    // Timer
    SocketTimerId,
    GraphicsTimerId,
    ResetTimerId,

    // Widgets
    ResetButtonId,
    UseSocketButtonId,
    UseFileButtonId,
    HostButtonsId,
    DataFileButtonsId,
    SocketControlsButtonId,
    GraphicsControlsButtonId,

    SocketReadAllCheckBoxId,
    SocketReadIntervalSliderId,

    GraphicsUpdateIntervalSliderId,
    ObjectRadiusSliderId,
    JobHeightSliderId,
    JobSpacingSliderId,
    JobVelocitySliderId,
    SiteSpacingSliderId,
    MaxStackSizeSliderId,

    ShowStatusOnlyButtonId,
    ShowScienceOnlyButtonId,
    ShowStatusAndScienceButtonId,

    ShowGhostJobsCheckBoxId,
    FadeGhostJobsCheckBoxId,
    ShowJobPathsCheckBoxId,
    ShowJobTrailsCheckBoxId,
    ShowSiteSpindlesCheckBoxId,
    ResetSiteSpindlesButtonId,

    FadedOpacitySliderId,

    ShowSiteRankingLegendCheckBoxId,
        
    ShowMapCheckBoxId,
    
    ShowLogosCheckBoxId,
    RotateLogosCheckBoxId
};


/////////////////////////////////////////////////////////////////////////////////////////////
// MatchMakerApp
/////////////////////////////////////////////////////////////////////////////////////////////

class MatchMakerApp : public wxApp {
public:
    // Program execution starts here
    bool OnInit();
};


/////////////////////////////////////////////////////////////////////////////////////////////
// MainFrame
/////////////////////////////////////////////////////////////////////////////////////////////

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    virtual ~MainFrame();

    // Handle timer events
    void OnTimer(wxTimerEvent& e);

    // Menu events
    void OnButton(wxCommandEvent& e);
    void OnRadioButton(wxCommandEvent& e);

    // Get Timers
    wxTimer* GetSocketTimer();
    wxTimer* GetGraphicsTimer();

private:
    // Timers
    wxTimer* socketTimer;
    wxTimer* graphicsTimer;
    wxTimer* resetTimer;

    // Button for resetting the visualization
    wxButton* resetButton;
    
    // Radio buttons for switching between a socket and a file
    wxRadioButton* useSocketButton;
    wxRadioButton* useFileButton;

    // Radio buttons for different hosts
    std::vector<wxRadioButton*> hostButtons;

    // Radio buttons for different data files
    std::vector<wxRadioButton*> dataFileButtons;

    // Buttons for popping up frames
    wxButton* socketControlsButton;
    wxButton* graphicsControlsButton;

    // Frames
    SocketFrame* socketFrame;
    GraphicsFrame* graphicsFrame;

    // The main engine
    Engine* engine;

    // Log window
    MatchMakerLogWindow* log;

    // Any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};


/////////////////////////////////////////////////////////////////////////////////////////////
// MatchMakerLogWindow
/////////////////////////////////////////////////////////////////////////////////////////////

class MatchMakerLogWindow : public wxLogWindow {
public:
    MatchMakerLogWindow(wxWindow *parent, const wxChar *title, 
                        bool show = true, bool passToOld = true) : wxLogWindow(parent, title, show, passToOld) {}
/*
    virtual bool OnFrameClose(wxFrame* frame) {
        // Don't allow the user to close
        return false;
    }
*/
};


/////////////////////////////////////////////////////////////////////////////////////////////
// Other frames
/////////////////////////////////////////////////////////////////////////////////////////////

// Base frame class all other frames are derived from
class MatchMakerFrame : public wxFrame {
public:
    MatchMakerFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, Engine* theEngine);
    virtual ~MatchMakerFrame();

    virtual void OnClose(wxCloseEvent& event);

protected:
    Engine* engine;
    MainFrame* mainFrame;

    // Border
    static const int border = 5;

    DECLARE_EVENT_TABLE()
};


// Frame to hold the Socket controls
class SocketFrame : public MatchMakerFrame {
public:
    SocketFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, Engine* engine);

    void OnCheckBox(wxCommandEvent& e);
    void OnScrollThumbtrack(wxScrollEvent& e);
    void OnScrollChanged(wxScrollEvent& e);

private:
    wxCheckBox* socketReadAllCheckBox;
    wxSlider* socketReadIntervalSlider;

    DECLARE_EVENT_TABLE()
};


// Frame to hold the Graphics controls
class GraphicsFrame : public MatchMakerFrame {
public:
    GraphicsFrame(wxWindow* parent, const wxString& title, const wxPoint& pos, const wxSize& size, Engine* engine);
    
    void OnButton(wxCommandEvent& e);
    void OnRadioButton(wxCommandEvent& e);
    void OnCheckBox(wxCommandEvent& e);
    void OnScrollThumbtrack(wxScrollEvent& e);
    void OnScrollChanged(wxScrollEvent& e);

private:
    wxSlider* graphicsUpdateIntervalSlider;
    wxSlider* objectRadiusSlider;
    wxSlider* jobHeightSlider;
    wxSlider* jobSpacingSlider;
    wxSlider* jobVelocitySlider;
    wxSlider* siteSpacingSlider;
    wxSlider* maxStackSizeSlider;

    wxRadioButton* showStatusOnlyButton;
    wxRadioButton* showScienceOnlyButton;
    wxRadioButton* showStatusAndScienceButton;

    wxCheckBox* showGhostJobsCheckBox;
    wxCheckBox* fadeGhostJobsCheckBox;
    wxCheckBox* showJobPathsCheckBox;
    wxCheckBox* showJobTrailsCheckBox;
    wxCheckBox* showSiteSpindlesCheckBox;

    wxButton* resetSiteSpindlesButton;

    wxSlider* fadedOpacitySlider;

    wxCheckBox* showSiteRankingLegendCheckBox;

    wxCheckBox* showMapCheckBox;

    wxCheckBox* showLogosCheckBox;
    wxCheckBox* rotateLogosCheckBox;

    DECLARE_EVENT_TABLE()
};


#endif
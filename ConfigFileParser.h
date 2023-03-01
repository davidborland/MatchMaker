///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        ConfigFileParser.h
//
// Author:      David Borland
//
// Description: Interface of ConfigFileParser for reading MatchMaker configuration file.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef CONFIGFILEPARSER_H
#define CONFIGFILEPARSER_H


#include <string>
#include <vector>

#include "Job.h"


class ConfigFileParser {
public:
    ConfigFileParser();
    ~ConfigFileParser();

    bool Parse(const std::string& fileName);

    bool FullScreen();
    void GetFullSize(int& x, int& y);
    bool Stereo();
    bool Dome();

    double GetXScale();
    double GetYScale();

    bool UseSocket();
    const std::vector<std::string>& GetHostDescriptions();
    const std::vector<std::string>& GetHostNames();
    const std::vector<int>& GetPorts();
    const std::vector<std::string>& GetDataFileDescriptions();
    const std::vector<std::string>& GetDataFileNames();

    bool GetSocketReadAll();
    int GetSocketReadInterval();
    bool LoopFile();

    int GetGraphicsUpdateInterval();

    int GetResetSeconds();

    double GetObjectRadius();
    double GetLabelHeight();
    bool LabelFaceCamera();
    
    double GetJobHeight();
    double GetJobSpacing();
    double GetJobVelocity();

    double GetSiteSpacing();
    int GetMaxStackSize();

    Job::ShowGlyphType GetShowGlyphs();
    bool ShowGhostJobs();
    bool FadeGhostJobs();
    bool ShowJobPaths();
    bool ShowJobTrails();
    bool ShowSiteSpindles();

    bool UseDoneSite();

    double GetFadedOpacity();

    bool ShowSiteRankingLegend();

    const std::vector<std::string>& GetLogoFileNames();
    bool ShowLogos();
    bool RotateLogos();

    const std::string& GetMapFileName();
    bool ShowMap();

    bool DarkBackground();

private:
    bool fullScreen;
    int fullSize[2];
    bool stereo;
    bool dome;
    
    double xScale;
    double yScale;

    bool useSocket;
    std::vector<std::string> hostDescriptions;
    std::vector<std::string> hostNames;
    std::vector<int> ports;
    std::vector<std::string> dataFileDescriptions;
    std::vector<std::string> dataFileNames;

    bool socketReadAll;
    int socketReadInterval;
    bool loopFile;

    int graphicsUpdateInterval;

    int resetSeconds;

    double objectRadius;
    double labelHeight;
    bool labelFaceCamera;

    double jobHeight;
    double jobSpacing;
    double jobVelocity;

    double siteSpacing;
    int maxStackSize;

    Job::ShowGlyphType showGlyphs; 
    bool showGhostJobs;
    bool fadeGhostJobs;
    bool showJobPaths;
    bool showJobTrails;
    bool showSiteSpindles;

    bool useDoneSite;

    double fadedOpacity;

    bool showSiteRankingLegend;

    std::vector<std::string> logoFileNames;
    bool showLogos;
    bool rotateLogos;

    std::string mapFileName;
    bool showMap;

    bool darkBackground;

    std::vector<std::string> Tokenize(const std::string& s, const std::string& delimiters);
};


#endif
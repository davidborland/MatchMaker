///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        ConfigFileParser.cpp
//
// Author:      David Borland
//
// Description: Implementation of ConfigFileParser for reading MatchMaker configuration file.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "ConfigFileParser.h"

#include <fstream>


#include <wx/log.h>


ConfigFileParser::ConfigFileParser() {
    // Defaults
    fullScreen = false;
    fullSize[0] = -1;
    fullSize[1] = -1;
    stereo = false;
    dome = false;

    xScale = 1.0;
    yScale = 1.0;

    useSocket = true;

    socketReadAll = true;
    socketReadInterval = 100;
    bool loopFile = true;

    graphicsUpdateInterval = 10;

    resetSeconds = -1;

    objectRadius = 10.0;   
    labelHeight = 0.0;
    labelFaceCamera = true;

    jobHeight = 5.0;
    jobSpacing = 5.0;
    jobVelocity = 2.0;

    siteSpacing = 30.0;
    maxStackSize = 30;

    showGlyphs = Job::ShowStatusOnly;
    showGhostJobs = true;
    fadeGhostJobs = true;
    showJobPaths = true;
    showSiteSpindles = true;

    useDoneSite = true;

    fadedOpacity = 0.25;

    showSiteRankingLegend = true;

    showLogos = true;
    rotateLogos = true;

    mapFileName = "Data/usnsn_map_crop_alpha_dark.tif";
    showMap = true;

    darkBackground = true; 
}


ConfigFileParser::~ConfigFileParser() {
}


bool ConfigFileParser::Parse(const std::string& fileName) {
    std::fstream file(fileName.c_str(), std::fstream::in);
    if (file.fail()) {
        wxLogMessage("Couldn't open %s", fileName.c_str());
        return false;
    }

    std::string s;
    std::vector<std::string> tokens;

    wxLogMessage("**********Parsing %s**********", fileName.c_str());
    wxLogMessage("");

    while (!file.eof()) {
        getline(file, s);

        tokens = Tokenize(s, " \n");

        if (tokens.size() == 0) {
            continue;
        }
        else {
            if (tokens[0] == "//") continue;
        }

        if (tokens.size() >= 2) {
            if (tokens[0] == "FullSize") {
                if (tokens.size() != 3) {
                    wxLogMessage("Error parsing : %s", s.c_str());
                    continue;
                }
                fullSize[0] = atoi(tokens[1].c_str());
                fullSize[1] = atoi(tokens[2].c_str());

                wxLogMessage("fullSize = %d %d", fullSize[0], fullSize[1]);
                continue;
            }
            else if (tokens[0] == "LogoFileNames") {
                std::string printString;
                for (int i = 1; i < (int)tokens.size(); i++) {
                    logoFileNames.push_back(tokens[i]);
                    printString += logoFileNames.back();
                    printString += " ";
                }
                wxLogMessage("LogoFileNames = %s", printString.c_str());
                continue;
            } 
            else if (tokens[0] == "HostDescription") {
                std::string hostDescription;
                bool hasHostName = false;
                int i = 1;
                while (1) {
                    hostDescription += tokens[i];
                    i++;

                    if (i == tokens.size()) break;
                    if (tokens[i] == "HostName") {
                        hasHostName = true;
                        break;
                    }

                    hostDescription += " ";
                }
                hostDescriptions.push_back(hostDescription);

                if (hasHostName) {
                    if (i + 3 > (int)tokens.size()) {
                        wxLogMessage("Error parsing : %s", s.c_str());
                        continue;
                    }

                    hostNames.push_back(tokens[i + 1]);
                    ports.push_back(atoi(tokens[i + 2].c_str()));
                   
                    wxLogMessage("hostDescription = %s, hostName = %s, port = %d", 
                                  hostDescriptions.back().c_str(), hostNames.back().c_str(), ports.back());
                }
                else {
                    wxLogMessage("hostDescription = %s", hostDescriptions.back().c_str());
                }
                continue;
            }
            else if (tokens[0] == "DataFileDescription") {
                std::string dataFileDescription;
                bool hasDataFileName = false;
                int i = 1;
                while (1) {
                    dataFileDescription += tokens[i];
                    i++;

                    if (i == tokens.size()) break;
                    if (tokens[i] == "DataFileName") {
                        hasDataFileName = true;
                        break;
                    }

                    dataFileDescription += " ";
                }
                dataFileDescriptions.push_back(dataFileDescription);

                if (hasDataFileName) {
                    if (i + 2 > (int)tokens.size()) {
                        wxLogMessage("Error parsing : %s", s.c_str());
                        continue;
                    }

                    dataFileNames.push_back(tokens[i + 1]);
                   
                    wxLogMessage("dataFileDescription = %s, dataFileName = %s",
                                  hostDescriptions.back().c_str(), hostNames.back().c_str());
                }
                else {
                    wxLogMessage("dataFileDescription = %s", dataFileDescriptions.back().c_str());
                }
                continue;
            }
        }

        if (tokens.size() == 2) {
            if (tokens[0] == "FullScreen") {
                fullScreen = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("fullScreen = %d", fullScreen);
            }
            else if (tokens[0] == "Stereo") {
                stereo = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("stereo = %d", stereo);
            }          
            else if (tokens[0] == "Dome") {
                dome = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("dome = %d", dome);
            }
            else if (tokens[0] == "XScale") {
                xScale = atof(tokens[1].c_str());
                wxLogMessage("xScale = %f", xScale);
            }
            else if (tokens[0] == "YScale") {
                yScale = atof(tokens[1].c_str());
                wxLogMessage("yScale = %f", yScale);
            }
            else if (tokens[0] == "UseSocket") {
                useSocket = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("useSocket = %d", useSocket);
            }
            else if (tokens[0] == "SocketReadAll") {
                socketReadAll = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("socketReadAll = %d", socketReadAll);
            }
            else if (tokens[0] == "SocketReadInterval") {
                socketReadInterval = atoi(tokens[1].c_str());
                wxLogMessage("socketReadInterval = %d", socketReadInterval);
            }
            else if (tokens[0] == "LoopFile") {
                loopFile = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("loopFile = %d", loopFile);
            }
            else if (tokens[0] == "GraphicsUpdateInterval") {
                graphicsUpdateInterval = atoi(tokens[1].c_str());
                wxLogMessage("graphicsUpdateInterval = %d", graphicsUpdateInterval);
            }
            else if (tokens[0] == "ResetSeconds") {
                resetSeconds = atoi(tokens[1].c_str());
                wxLogMessage("resetSeconds = %d", resetSeconds);
            }
            else if (tokens[0] == "ObjectRadius") {
                objectRadius = atof(tokens[1].c_str());
                wxLogMessage("objectRadius = %f", objectRadius);
            }           
            else if (tokens[0] == "LabelHeight") {
                labelHeight = atof(tokens[1].c_str());
                wxLogMessage("labelHeight = %f", labelHeight);
            }
            else if (tokens[0] == "LabelFaceCamera") {
                labelFaceCamera = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("labelFaceCamera = %d", labelFaceCamera);
            }
            else if (tokens[0] == "JobHeight") {
                jobHeight = atof(tokens[1].c_str());
                wxLogMessage("jobHeight = %f", jobHeight);
            }
            else if (tokens[0] == "JobSpacing") {
                jobSpacing = atof(tokens[1].c_str());
                wxLogMessage("jobSpacing = %f", jobSpacing);
            }
            else if (tokens[0] == "JobVelocity") {
                jobVelocity = atof(tokens[1].c_str());
                wxLogMessage("jobVelocity = %f", jobVelocity);
            }
            else if (tokens[0] == "SiteSpacing") {
                siteSpacing = atof(tokens[1].c_str());
                wxLogMessage("siteSpacing = %f", siteSpacing);
            }
            else if (tokens[0] == "MaxStackSize") {
                maxStackSize = atoi(tokens[1].c_str());
                wxLogMessage("maxStackSize = %d", maxStackSize);
            }
            else if (tokens[0] == "ShowGlyphs") {
                int type = atoi(tokens[1].c_str());
                if (type == 0) {
                    showGlyphs = Job::ShowStatusOnly;
                    wxLogMessage("showGlyphType = ShowStatusOnly");
                }
                else if (type == 1) {
                    showGlyphs = Job::ShowScienceOnly;
                    wxLogMessage("showGlyphType = ShowScienceOnly");
                }
                else {
                    showGlyphs = Job::ShowStatusAndScience;
                    wxLogMessage("showGlyphType = ShowStatusAndScience");
                }
            }
            else if (tokens[0] == "ShowGhostJobs") {
                showGhostJobs = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("showGhostJobs = %d", showGhostJobs);
            }
            else if (tokens[0] == "FadeGhostJobs") {
                fadeGhostJobs = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("fadeGhostJobs = %d", fadeGhostJobs);
            }
            else if (tokens[0] == "ShowJobPaths") {
                showJobPaths = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("showJobPaths = %d", showJobPaths);
            }
            else if (tokens[0] == "ShowJobTrails") {
                showJobTrails = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("showJobTrails = %d", showJobTrails);
            }
            else if (tokens[0] == "ShowSiteSpindles") {
                showSiteSpindles = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("showSiteSpindles = %d", showSiteSpindles);
            }   
            else if (tokens[0] == "UseDoneSite") {
                useDoneSite = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("useDoneSite = %d", useDoneSite);
            } 
            else if (tokens[0] == "FadedOpacity") {
                fadedOpacity = atof(tokens[1].c_str());
                wxLogMessage("fadedOpacity = %f", fadedOpacity);
            }
            else if (tokens[0] == "ShowSiteRankingLegend") {
                showSiteRankingLegend = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("showSiteRankingLegend = %d", showSiteRankingLegend);
            }
            else if (tokens[0] == "ShowLogos") {
                showLogos = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("showLogos = %d", showLogos);
            }
            else if (tokens[0] == "RotateLogos") {
                rotateLogos = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("rotateLogos = %d", rotateLogos);
            }
            else if (tokens[0] == "MapFileName") {
                mapFileName = tokens[1];
                wxLogMessage("MapFileName = %s", mapFileName.c_str());
            }            
            else if (tokens[0] == "ShowMap") {
                showMap = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("showMap = %d", showMap);
            }
            else if (tokens[0] == "DarkBackground") {
                darkBackground = atoi(tokens[1].c_str()) != 0;
                wxLogMessage("darkBackground = %d", darkBackground);
            }
        }
        else {
            wxLogMessage("Error parsing : %s", s.c_str());
        }
    }

    wxLogMessage("");
    wxLogMessage("***********End %s************\n", fileName.c_str());

    file.close();

    return true;
}


bool ConfigFileParser::FullScreen() {
    return fullScreen;
}

void ConfigFileParser::GetFullSize(int& x, int& y) {
    x = fullSize[0];
    y = fullSize[1];
}

bool ConfigFileParser::Stereo() {
    return stereo;
}

bool ConfigFileParser::Dome() {
    return dome;
}


double ConfigFileParser::GetXScale() {
    return xScale;
}

double ConfigFileParser::GetYScale() {
    return yScale;
}


bool ConfigFileParser::UseSocket() {
    return useSocket;
}

const std::vector<std::string>& ConfigFileParser::GetHostDescriptions() {
    return hostDescriptions;
}

const std::vector<std::string>& ConfigFileParser::GetHostNames() {
    return hostNames;
}

const std::vector<int>& ConfigFileParser::GetPorts() {
    return ports;
}

const std::vector<std::string>& ConfigFileParser::GetDataFileDescriptions() {
    return dataFileDescriptions;
}

const std::vector<std::string>& ConfigFileParser::GetDataFileNames() {
    return dataFileNames;
}


bool ConfigFileParser::GetSocketReadAll() {
    return socketReadAll;
}

int ConfigFileParser::GetSocketReadInterval() {
    return socketReadInterval;
}

bool ConfigFileParser::LoopFile() {
    return loopFile;
}


int ConfigFileParser::GetGraphicsUpdateInterval() {
    return graphicsUpdateInterval;
}


int ConfigFileParser::GetResetSeconds() {
    return resetSeconds;
}


double ConfigFileParser::GetObjectRadius() {
    return objectRadius;
}

double ConfigFileParser::GetLabelHeight() {
    return labelHeight;
}

bool ConfigFileParser::LabelFaceCamera() {
    return labelFaceCamera;
}


double ConfigFileParser::GetJobHeight() {
    return jobHeight;
}

double ConfigFileParser::GetJobSpacing() {
    return jobSpacing;
}

double ConfigFileParser::GetJobVelocity() {
    return jobVelocity;
}


double ConfigFileParser::GetSiteSpacing() {
    return siteSpacing;
}

int ConfigFileParser::GetMaxStackSize() {
    return maxStackSize;
}


Job::ShowGlyphType ConfigFileParser::GetShowGlyphs() {
    return showGlyphs;
}

bool ConfigFileParser::ShowGhostJobs() {
    return showGhostJobs;
}

bool ConfigFileParser::FadeGhostJobs() {
    return fadeGhostJobs;
}

bool ConfigFileParser::ShowJobPaths() {
    return showJobPaths;
}

bool ConfigFileParser::ShowJobTrails() {
    return showJobTrails;
}

bool ConfigFileParser::ShowSiteSpindles() {
    return showSiteSpindles;
}


bool ConfigFileParser::UseDoneSite() {
    return useDoneSite;
}


double ConfigFileParser::GetFadedOpacity() {
    return fadedOpacity;
}


bool ConfigFileParser::ShowSiteRankingLegend() {
    return showSiteRankingLegend;
}


const std::vector<std::string>& ConfigFileParser::GetLogoFileNames() {
    return logoFileNames;
}

bool ConfigFileParser::ShowLogos() {
    return showLogos;
}

bool ConfigFileParser::RotateLogos() {
    return rotateLogos;
}


const std::string& ConfigFileParser::GetMapFileName() {
    return mapFileName;
}

bool ConfigFileParser::ShowMap() {
    return showMap;
}


bool ConfigFileParser::DarkBackground() {
    return darkBackground;
}


std::vector<std::string> ConfigFileParser::Tokenize(const std::string& s, const std::string& delimiters) {
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
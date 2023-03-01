///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        SiteList.h
//
// Author:      David Borland
//
// Description: Interface of SiteList class for MatchMaker.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef SITELIST_H
#define SITELIST_H


#include "Site.h"

#include <vtkColorTransferFunction.h>
#include <vtkScalarBarActor.h>

#include <vector>

#include <Vec2.h>


class SiteList {
public:
    SiteList(vtkRenderer* ren, vtkRenderer* legendRen, bool useDarkBackground);
    ~SiteList();

    Site* Get(const std::string& siteID);
    void Arrange();

    // Get/set spacing
    double GetJobSpacing();
    double GetSiteSpacing();
    void SetSiteRadius(double radius);
    void SetJobSpacing(double spacing);
    void SetSiteSpacing(double spacing);

    // Get/set max stack size
    int GetMaxStackSize();
    void SetMaxStackSize(int size);

    // Get/set show spindles
    bool GetShowSpindles();
    void ShowSpindles(bool show);

    // Set map extents
    void SetMapExtents(const double* extents);
    void SetUnknownPos(const Vec2& position);
    void SetOffTheMapPos(const Vec2& position);

    // Show the site ranking legend or not
    bool GetShowSiteRankingLegend();
    void ShowSiteRankingLegend(bool show);

    // Site label height
    void SetLabelHeight(double height);
    void LabelFaceCamera(bool faceCamera);

    // Reset spindles to current stack size
    void ResetSpindles();

    // Force update
    void Update();

    // Reset the data
    void Reset();

private:
    std::vector<Site*> sites;
    Site* doneSite;

    double siteRadius;
    double jobSpacing;
    double siteSpacing;

    int maxStackSize;

    bool showSpindles;

    bool showSiteRankingLegend;

    double labelHeight;
    bool labelFaceCamera;

    double mapExtents[4];
    Vec2 unknownPos;
    Vec2 offTheMapPos;

    bool darkBackground;

    vtkRenderer* renderer;
    vtkRenderer* legendRenderer;
    vtkColorTransferFunction* lut;
    vtkScalarBarActor* scalarBar;

    void ComputeForce(Vec2& force, Vec2& pos, const Vec2& pos2);
};


#endif
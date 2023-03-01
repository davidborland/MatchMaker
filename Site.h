///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Site.h
//
// Author:      David Borland
//
// Description: Interface of Site class for MatchMaker.  DoneSite is derived from Site,
//              and is used for the Done site, which stacks Done jobs below the site.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef SITE_H
#define SITE_H


#include "Object.h"

#include <string>
#include <vector>

#include <vtkActor.h>
#include <vtkCaptionActor2D.h>
#include <vtkColorTransferFunction.h>
#include <vtkCylinderSource.h>
#include <vtkDiskSource.h>
#include <vtkLineSource.h>
#include <vtkPlaneSource.h>
#include <vtkTextActor3D.h>

#include <Vec2.h>
#include <Vec3.h>

#include "Job.h"
#include "NetworkConnection.h"
#include "Stack.h"


class Job;
class NetworkConnection;


class Site : public Object {
public:
    Site(const std::string& siteID, double radius, vtkRenderer* ren, vtkColorTransferFunction* lookUpTable,
         double jobSpacingDistance, double siteSpacingDistance, int maxSiteStackSize, bool showSiteSpindle, 
         double* mapXYExtents, const Vec2& unknownPosition, const Vec2& offTheMapPosition, 
         double labelHeight, bool labelFaceCamera, bool darkBackground);
    virtual ~Site();

    // Position and location
    const Vec3& GetPosition();
    const Vec3& GetLocation();
    void SetLongLat(double longitude, double latitude);
    void SetPosition(const Vec3& pos);
    void ResetPosition();

    // Site appearance
    const double* GetColor();
    virtual void SetColor(double r, double g, double b);
    virtual void SetRadius(double radius);

    double GetOuterRadius();

    // Spacing
    void SetJobSpacing(double spacing);
    void SetSiteSpacing(double spacing);

    // Max number of jobs per stack
    void SetMaxStackSize(int size);
    
    // Set the rank
    void SetRank(std::string rank);

    // Add and remove jobs
    void AttachJob(Job* job);
    void RemoveJob(std::string jobId);

    // Add network connections
    void AddNetworkConnection(NetworkConnection* connection);

    // Show spindles or not
    void ShowSpindle(bool show);
    void ResetSpindle();

    // Update the site
    void Update();

    // Stack info
    int GetNumStacks();
    Vec3 GetStackPosition(int i);
    void GetClosestStackCenter(const Vec2& pos, Vec2& stackPos);

protected:
    // The jobs at this site.  These are pointers to Jobs in the Engine's JobList.
    // They are neither created nor destroyed here.
    std::vector<Job*> jobs;

    // The network connections at this site.  These are pointers to NetworkConnections in the Engine's NetworkConnectionList.
    // They are neither created nor destroyed here.
    std::vector<NetworkConnection*> connections;

    // Each site can have multiple stacks, to reduce the overall height.
    std::vector<Stack*> stacks;

    // An anchor from the current site position to the actual site position.  
    // This is necessary when site are moved to keep space between them.
    vtkDiskSource* anchor;
    vtkActor* anchorActor;
    vtkLineSource* anchorLine;
    vtkActor* anchorLineActor;

    // The site name
    vtkCaptionActor2D* text;
    vtkTextActor3D* text3D;

    // Lookup table for the site color
    vtkColorTransferFunction* lut;

    // Radii of the site geometry
    double innerRadius;
    double outerRadius;
    double spindleRadius;
    double anchorRadius;

    // Minimum amount of spacing between sites
    double siteSpacing;

    // Maximum number of jobs per stack
    int maxStackSize;

    // The ideal position of the site
    Vec2 longLat;

    // The position of the site 
    Vec3 position;

    // The physical location of the site
    Vec3 location;

    // The site color
    double color[3];

    // Used in calculating spindle heights
    int mostNumJobs;

    // Vertical spacing between stacked jobs
    double jobSpacing;

    // Offsets to limit z-fighting
    double siteZ;
    double anchorOffset;
    double jobOffset;

    // Show spindles or not
    bool showSpindle;

    // Map extents
    double* mapExtents;
    Vec2 unknownPos;
    Vec2 offTheMapPos;

    // Arrange the stacks
    virtual void ArrangeStacks();
    
    // Stack the jobs
    virtual void StackJobs();
};


//////////////////////////////////////////////////////////////////////////


class SpecialSite : public Site {
public:
    SpecialSite(const std::string& siteName, double radius, vtkRenderer* ren, vtkColorTransferFunction* lookUpTable,
                double jobSpacingDistance, double siteSpacingDistance, int maxSiteStackSize, bool showSiteSpindle, 
                double* mapXYExtents, const Vec2& unknownPosition, const Vec2& offTheMapPosition, 
                double labelHeight, bool labelFaceCamera, bool darkBackground);
    virtual ~SpecialSite();

    virtual void SetRadius(double radius);

protected:
    vtkPlaneSource* strip;
    vtkActor* stripActor;

    vtkPlaneSource* leftBorder;
    vtkPlaneSource* rightBorder;
    vtkPlaneSource* bottomBorder;
    vtkPlaneSource* topBorder;
    vtkActor* leftBorderActor;
    vtkActor* rightBorderActor;
    vtkActor* bottomBorderActor;
    vtkActor* topBorderActor;

    double gap;
    double border;
        
    virtual void UpdateStrip();
    void UpdateBorder();
    virtual void UpdatePosition();

    virtual void ArrangeStacks();

    void CreatePlane(vtkPlaneSource*& plane, vtkActor*& actor);
};


//////////////////////////////////////////////////////////////////////////


class DoneSite : public SpecialSite {
public:
    DoneSite(const std::string& siteName, double radius, vtkRenderer* ren, vtkColorTransferFunction* lookUpTable,
             double jobSpacingDistance, double siteSpacingDistance, int maxSiteStackSize, bool showSiteSpindle, 
             double* mapXYExtents, const Vec2& unknownPosition, const Vec2& offTheMapPosition, 
             double labelHeight, bool labelFaceCamera, bool darkBackground);
    virtual ~DoneSite();

    void SetNumJobs(int num);

protected:
    int numJobs;
    int numDone;

    void SetCaption();

    virtual void UpdateStrip();
    virtual void UpdatePosition();

    virtual void StackJobs();
};


#endif
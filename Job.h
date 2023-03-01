///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Job.h
//
// Author:      David Borland
//
// Description: Interface of Job class for MatchMaker.                
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef JOB_H
#define JOB_H


#include "Object.h"

#include <string>

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkCaptionActor2D.h>
#include <vtkCylinderSource.h>
#include <vtkLineSource.h>
#include <vtkTextActor3D.h>

#include <Vec3.h>

#include "Site.h"
#include "DataTransfer.h"
#include "NetworkConnection.h"


class DataTransfer;
class NetworkConnection;
class Site;


class Job : public Object {
public:    
    // Show glyphs or not
    enum ShowGlyphType {
        ShowStatusOnly,
        ShowScienceOnly,
        ShowStatusAndScience
    };
    void ShowGlyphs(ShowGlyphType show);

    Job(const std::string& jobID, double radius, vtkRenderer* renderer, Site* startSite, 
        double height, double jobVelocity, 
        ShowGlyphType showWhichGlyphs, bool showGhostJobs, bool fadeGhostJobs, bool showJobPath, bool showJobTrail, 
        double labelHeight, bool labelFaceCamera);
    virtual ~Job();

    Site* GetSite();

    bool SetState(const std::string& state);
    void SetSite(Site* newSite);
    void SetScienceColor(double r, double g, double b);

    void StartDataTransfer(Site* dataSource, Site* dataSink, NetworkConnection* connection, double dataSize);

    double GetRadius();
    const double* GetColor();
    virtual void SetRadius(double radius);
    virtual void SetColor(double r, double g, double b);
    void SetOpacity(double opacity);

    // Get/set height
    double GetHeight();
    void SetHeight(double height);

    // Get/set name
    const std::string& GetName();
    void SetName(const std::string& jobName);

    bool IsDone();


    // Colors
    static double matchingColor[3];
    static double submittingColor[3];
    static double queuedColor[3];
    static double runningColor[3];
    static double doneColor[3];
    static double failedColor[3];

    // For animating 
    const Vec3& GetPosition();
    void SetPosition(const Vec3& pos);
    void SetOldPosition(const Vec3& pos);
    void UpdatePosition();
    void SetVelocity(double v);

    void ShowGhost(bool show);
    void FadeGhost(bool fade);
    void ShowPath(bool show);
    void ShowTrail(bool show);

    // Text caption
    void ShowName(bool show);

    // Scale color
    static void ScaleColor(double scale);

private:
    Site* site;
    Site* oldSite;

    DataTransfer* data;

    vtkCylinderSource* statusGlyph;
    vtkActor* statusActor;
    vtkCylinderSource* ghostStatusGlyph;
    vtkActor* ghostStatusActor;
    vtkCylinderSource* oldGhostStatusGlyph;
    vtkActor* oldGhostStatusActor;

    vtkCylinderSource* scienceGlyph;
    vtkActor* scienceActor;
    vtkCylinderSource* ghostScienceGlyph;
    vtkActor* ghostScienceActor;
    vtkCylinderSource* oldGhostScienceGlyph;
    vtkActor* oldGhostScienceActor;

    vtkLineSource* path;
    vtkActor* pathActor;
    vtkLineSource* trail;
    vtkActor* trailActor;    
    
    // For displaying the name
    vtkCaptionActor2D* text;
    vtkTextActor3D* text3D;

    std::string name;

    bool moving;
    Vec3 position;
    Vec3 oldPosition;
    double velocity;

    ShowGlyphType showGlyphs;
    bool showGhosts;
    bool fadeGhosts;
    bool showPath;
    bool showTrail;

    bool showName;

    bool isDone;

    void UpdateGhostOpacities(double fraction);
};


#endif
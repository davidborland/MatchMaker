///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        RenderPipeline.h
//
// Author:      David Borland
//
// Description: Interface of the VTK rendering pipeline for MatchMaker.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H


#include <vtkAVIWriter.h>
#include <vtkCaptionActor2D.h>
#include <vtkImageActor.h>
#include <vtkLegendBoxActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextActor3D.h>
#include <vtkWindowToImageFilter.h>

#include <Vec2.h>

#include <string>
#include <vector>

#include "VTKCallbacks.h"


class RenderPipeline {
public:
    RenderPipeline(const std::vector<std::string>& logoFileNames, const std::string& mapFileName,
                   double labelHeight, bool labelFaceCamera, bool useDarkBackground);
    ~RenderPipeline();

    vtkRenderWindowInteractor* GetInteractor();

    void Render();
    void Update();

    vtkRenderer* GetRenderer();
    vtkRenderer* GetLegendRenderer();

    void ToggleFullScreen();
    void ToggleStereo();
    void ToggleDome();
    
    void SetFullSize(int x, int y);

    void HandleRender();
    void ToggleMovie();

    const double* GetMapExtents();
    const Vec2& GetUnknownPos();
    const Vec2& GetOffTheMapPos();

    bool GetShowMap();
    void ShowMap(bool show);

    bool GetShowLogos();
    bool GetRotateLogos();
    void ShowLogos(bool show);
    void RotateLogos(bool rotate);

    void SetScienceLegend(vtkLegendBoxActor* scienceLegendActor);

    void ShowStatusLegend(bool show);
    void ShowScienceLegend(bool show);

private:
    // Basic rendering stuff
    vtkRenderer* renderer;
    vtkRenderWindow* window;
    vtkRenderWindowInteractor* interactor;
    vtkRenderer* legendRenderer;

    // Save window information when switching to fullscreen
    int savedPosition[2];
    int savedSize[2];
    int useFullSize;
    int fullSize[2];

    // Map extents.  {xMin, yMin, xMax, yMax}
    vtkActor* mapActor;
    vtkActor* offTheMapActor;
    vtkActor* offTheMapBorderActor;
    vtkCaptionActor2D* offTheMapLabel;
    vtkTextActor3D* offTheMapLabel3D;
    double mapExtents[4];
    Vec2 unknownPos;
    Vec2 offTheMapPos;
    bool showMap;

    bool darkBackground;

    // Legend
    vtkLegendBoxActor* statusLegend;
    vtkLegendBoxActor* scienceLegend;

    // Logos
    std::vector<vtkImageActor*> logoActors;
    std::vector<vtkRenderer*> logoRenderers;
    bool showLogos;
    bool rotateLogos;

    // For making a movie
    vtkWindowToImageFilter* windowCapture;
    vtkAVIWriter* aviWriter;
    RendererCallback* rendererCallback;
    bool recordingMovie;
    int movieNumber;

    void CreateRenderers();
    void CreateMap(const std::string& mapFileName);
    void CreateStatusLegend();
    void CreateLogos(const std::vector<std::string>& logoFileName);
    void SetInitialView();
    void CreateZones(double labelHeight, bool labelFaceCamera);
    void CreateMovieMaker();
};


#endif
///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        RenderPipeline.cpp
//
// Author:      David Borland
//
// Description: Implemention of the VTK rendering pipeline for MatchMaker.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "RenderPipeline.h"

#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkDiskSource.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkPlaneSource.h>
#include <vtkPNGReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>
#include <vtkTexture.h>

#include "Job.h"
#include "Projector.h"
#include "vtkMyInteractorStyleTrackballCamera.h"

#include <wx/log.h>


RenderPipeline::RenderPipeline(const std::vector<std::string>& logoFileNames, const std::string& mapFileName,
                               double labelHeight, bool labelFaceCamera, bool useDarkBackground) : darkBackground(useDarkBackground) {
    // Default
    savedPosition[0] = 0;
    savedPosition[1] = 0;
    savedSize[0] = 1024;
    savedSize[1] = 512;
    fullSize[0] = -1;
    fullSize[1] = -1;

    // Create objects
    CreateRenderers();
    CreateMap(mapFileName);
    CreateStatusLegend();
    CreateLogos(logoFileNames);
    SetInitialView();
    CreateZones(labelHeight, labelFaceCamera);
    CreateMovieMaker();

    // Start interaction
    interactor->Initialize();
}


RenderPipeline::~RenderPipeline() {
    // Clean up
    renderer->Delete();
    window->Delete();
    interactor->Delete();

    legendRenderer->Delete();

    mapActor->Delete();
    offTheMapActor->Delete();
    offTheMapBorderActor->Delete();
    if (offTheMapLabel) offTheMapLabel->Delete();
    if (offTheMapLabel3D) offTheMapLabel3D->Delete();

    statusLegend->Delete();

    for (int i = 0; i < (int)logoActors.size(); i++) logoActors[i]->Delete();
    for (int i = 0; i < (int)logoRenderers.size(); i++) logoRenderers[i]->Delete();

    windowCapture->Delete();
    aviWriter->End();
    aviWriter->Delete();
    rendererCallback->Delete();
}


vtkRenderWindowInteractor* RenderPipeline::GetInteractor() {
    return interactor;
}


void RenderPipeline::Render() {  
    renderer->ResetCameraClippingRange();
    interactor->Render();
}


void RenderPipeline::Update() {
    if (showLogos && rotateLogos) {
        for (int i = 0; i < (int)logoActors.size(); i++) {
            if (logoActors[i]->GetOrientation()[1] > 90.0) logoActors[i]->SetOrientation(0.0, 270.0, 0.0);
            else logoActors[i]->RotateY(1.0);

            logoRenderers[i]->ResetCameraClippingRange();
        }
    }
}


vtkRenderer* RenderPipeline::GetRenderer() {
    return renderer;
}


vtkRenderer* RenderPipeline::GetLegendRenderer() {
    return legendRenderer;
}


void RenderPipeline::ToggleFullScreen() {
    if (window->GetFullScreen() == 0) {
        savedPosition[0] = window->GetPosition()[0];
        savedPosition[1] = window->GetPosition()[1];
        savedSize[0] = window->GetSize()[0];
        savedSize[1] = window->GetSize()[1];

        window->FullScreenOn();

        if (fullSize[0] > 0 && fullSize[1] > 0) {
            window->SetSize(fullSize);
        }
    }
    else {
        window->FullScreenOff();

        window->SetPosition(savedPosition);
        window->SetSize(savedSize);
    }
    interactor->ReInitialize();
}

void RenderPipeline::ToggleStereo() {
    window->SetStereoRender(!window->GetStereoRender());
}

void RenderPipeline::ToggleDome() {    
//    window->SetDomeRender(!window->GetDomeRender());
}


void RenderPipeline::SetFullSize(int x, int y) {
    fullSize[0] = x;
    fullSize[1] = y;
}


void RenderPipeline::HandleRender() {
    if (recordingMovie) {
        windowCapture->Modified();
        aviWriter->Write();
    }
}


void RenderPipeline::ToggleMovie() {
    if (recordingMovie) {
        aviWriter->End();
        recordingMovie = false;

        wxLogMessage("Movie saved");
    }
    else {
        char filename[32];
        sprintf_s(filename, sizeof(filename), "Data/MatchMakerMovie%d.avi", movieNumber++);
        aviWriter->SetFileName(filename);
        aviWriter->Start();
        recordingMovie = true;

        wxLogMessage("Starting movie");
    }
}


const double* RenderPipeline::GetMapExtents() {
    return mapExtents;
}


const Vec2& RenderPipeline::GetUnknownPos() {
    return unknownPos;
}


const Vec2& RenderPipeline::GetOffTheMapPos() {
    return offTheMapPos;
}


bool RenderPipeline::GetShowLogos() {
    return showLogos;
}

bool RenderPipeline::GetRotateLogos() {
    return rotateLogos;
}

void RenderPipeline::ShowLogos(bool show) {
    showLogos = show;

    if (showLogos) {
        for (int i = 0; i < (int)logoActors.size(); i++) {
            logoRenderers[i]->AddViewProp(logoActors[i]);
        }
    }        
    else {
        for (int i = 0; i < (int)logoActors.size(); i++) {
            logoRenderers[i]->RemoveViewProp(logoActors[i]);
        }
    }

    for (int i = 0; i < (int)logoActors.size(); i++) {
        logoActors[i]->SetOrientation(0.0, 0.0, 0.0);
        logoRenderers[i]->ResetCamera();
    }
}

void RenderPipeline::RotateLogos(bool rotate) {
    rotateLogos = rotate;

    if (!rotateLogos) {
        for (int i = 0; i < (int)logoActors.size(); i++) {
            logoActors[i]->SetOrientation(0.0, 0.0, 0.0);
        }
    }
}


void RenderPipeline::SetScienceLegend(vtkLegendBoxActor* scienceLegendActor) {
    scienceLegend = scienceLegendActor;
}


void RenderPipeline::ShowStatusLegend(bool show) {
    if (show) {
        legendRenderer->AddViewProp(statusLegend);
    }
    else {
        legendRenderer->RemoveViewProp(statusLegend);
    }
}

void RenderPipeline::ShowScienceLegend(bool show) {
    if (show) {
        legendRenderer->AddViewProp(scienceLegend);
    }
    else {
        legendRenderer->RemoveViewProp(scienceLegend);
    }
}


bool RenderPipeline::GetShowMap() {
    return showMap;
}

void RenderPipeline::ShowMap(bool show) {
    showMap = show;

    if (showMap) {
        renderer->AddViewProp(mapActor);
        renderer->AddViewProp(offTheMapActor);
        renderer->AddViewProp(offTheMapBorderActor);
        if (offTheMapLabel) renderer->AddViewProp(offTheMapLabel);
        if (offTheMapLabel3D) renderer->AddViewProp(offTheMapLabel3D);
    }
    else {
        renderer->RemoveViewProp(mapActor);
        renderer->RemoveViewProp(offTheMapActor);
        renderer->RemoveViewProp(offTheMapBorderActor);
        if (offTheMapLabel) renderer->RemoveViewProp(offTheMapLabel);
        if (offTheMapLabel3D) renderer->RemoveViewProp(offTheMapLabel3D);
    }
}


void RenderPipeline::CreateRenderers() {
    // Create a renderer
    renderer = vtkRenderer::New();
    legendRenderer = vtkRenderer::New();
    legendRenderer->InteractiveOff();

    // Create the window
    window = vtkRenderWindow::New();
    window->SetWindowName("MatchMaker");
//    window->SetStereoTypeToLeftRight();
    window->StereoCapableWindowOn();
    window->AddRenderer(renderer);
    window->AddRenderer(legendRenderer);
    window->SetSize(savedSize[0], savedSize[1]);

//    window->BordersOff();

    // Create the interactor
    interactor = vtkRenderWindowInteractor::New();
    interactor->SetRenderWindow(window);

    // Use a trackball camera style, with different keypress events
    // Note:  Turns out this is not actually necessary, as the 
    vtkMyInteractorStyleTrackballCamera* trackballStyle = vtkMyInteractorStyleTrackballCamera::New();
    interactor->SetInteractorStyle(trackballStyle);
}


void RenderPipeline::CreateMap(const std::string& mapFileName) {
    // Load the map
    vtkPNGReader* mapReader = vtkPNGReader::New();
    mapReader->SetFileName(mapFileName.c_str());
    mapReader->Update();

    // Create a texture for the map
    vtkTexture* mapTexture = vtkTexture::New();
    mapTexture->SetInputConnection(mapReader->GetOutputPort());
    mapTexture->InterpolateOn();

    // Create the map
    vtkPlaneSource* map = vtkPlaneSource::New();
    map->SetXResolution(1);
    map->SetYResolution(1);

    // Map settings
    double longMin = -125.0;
    double longMax = -66.5;
    double latMin = 23.5;
    double latMax = 50.0;
    mapExtents[0] = Projector::ProjectLongitude(longMin);
    mapExtents[1] = Projector::ProjectLatitude(latMin);
    mapExtents[2] = Projector::ProjectLongitude(longMax);
    mapExtents[3] = Projector::ProjectLatitude(latMax);

    // Set up the map
    map->SetOrigin(mapExtents[0], mapExtents[1], 0.0);
    map->SetPoint1(mapExtents[2], mapExtents[1], 0.0);
    map->SetPoint2(mapExtents[0], mapExtents[3], 0.0);
    vtkPolyDataMapper* mapMapper = vtkPolyDataMapper::New();
    mapMapper->SetInputConnection(map->GetOutputPort());
    mapActor = vtkActor::New();
    mapActor->SetMapper(mapMapper);
    mapActor->SetTexture(mapTexture);

    // Turn off lighting for the map
    mapActor->GetProperty()->SetAmbient(1.0);
    mapActor->GetProperty()->SetDiffuse(0.0);
    mapActor->GetProperty()->SetSpecular(0.0);
    if (darkBackground) mapActor->GetProperty()->SetOpacity(0.5);
    else mapActor->GetProperty()->SetOpacity(0.9);

    // Add to the renderer
    renderer->AddViewProp(mapActor);    

    // Don't need these references any more
    mapReader->Delete();
    mapTexture->Delete();
    map->Delete();
    mapMapper->Delete();

    showMap = true;
}


void RenderPipeline::CreateStatusLegend() {
    // Create a legend
    statusLegend = vtkLegendBoxActor::New();
    double color[3];
    if (darkBackground) color[0] = color[1] = color[2] = 1.0;
    else color[0] = color[1] = color[2] = 0.0;
    statusLegend->SetNumberOfEntries(8);
    statusLegend->SetEntry(0, (vtkImageData*)NULL, "Job States:", color);
    statusLegend->SetEntry(1, (vtkImageData*)NULL, "\tMatching", Job::matchingColor);
    statusLegend->SetEntry(2, (vtkImageData*)NULL, "\tSubmitting", Job::submittingColor);
    statusLegend->SetEntry(3, (vtkImageData*)NULL, "\tQueued", Job::queuedColor);
    statusLegend->SetEntry(4, (vtkImageData*)NULL, "\tRunning", Job::runningColor);
    statusLegend->SetEntry(5, (vtkImageData*)NULL, "\tDone", Job::doneColor);
    statusLegend->SetEntry(6, (vtkImageData*)NULL, "\tFailed", Job::failedColor);
    statusLegend->SetPosition(0.0, 0.75);
    statusLegend->SetPosition2(0.075, 0.25);
    statusLegend->BorderOff();
}


void RenderPipeline::CreateLogos(const std::vector<std::string>& logoFileNames) {
    // Create logos
    for (int i = 0; i < (int)logoFileNames.size(); i++) {
        vtkPNGReader* logoReader = vtkPNGReader::New();
        logoReader->SetFileName(logoFileNames[i].c_str());
        logoReader->Update();

        int x = logoReader->GetOutput()->GetDimensions()[0];
        int y = logoReader->GetOutput()->GetDimensions()[1];
        double aspect = (double)x / (double)y;

        logoActors.push_back(vtkImageActor::New());
        logoActors.back()->SetInput(logoReader->GetOutput());
        logoActors.back()->InterpolateOn();
        logoActors.back()->SetOrigin((double)x / 2.0, (double)y / 2.0, 0);
        logoActors.back()->SetOpacity(0.5);

        logoRenderers.push_back(vtkRenderer::New());
        logoRenderers.back()->AddViewProp(logoActors.back());
        logoRenderers.back()->InteractiveOff();
        double startX = 0.75;
        double width = (0.99 - startX) / (double)logoFileNames.size();
        double startY = 0.0;
        double endY = 0.15;
        logoRenderers.back()->SetViewport(startX + i * width, startY, 
                                          startX + (i + 1) * width, endY);

        window->AddRenderer(logoRenderers.back());

        logoReader->Delete();
    }

    window->SetNumberOfLayers(2);
    renderer->SetLayer(0);
    legendRenderer->SetLayer(1);
    for (int i = 0; i < (int)logoRenderers.size(); i++) {
        logoRenderers[i]->SetLayer(1);
    }

    showLogos = true;
    rotateLogos = true;
}   


void RenderPipeline::SetInitialView() {
    // Set up camera for a better view
    renderer->GetActiveCamera()->Elevation(-45.0);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Dolly(1.4);
    renderer->ResetCameraClippingRange();


    // Prime the interactor
    vtkInteractorStyle* style = static_cast<vtkInteractorStyle*>(interactor->GetInteractorStyle());
    style->OnLeftButtonDown();
    style->OnMouseMove();
    style->OnLeftButtonUp();


    if (darkBackground) renderer->SetBackground(0.1, 0.1, 0.1);
    else renderer->SetBackground(0.75, 0.75, 0.75);
}


void RenderPipeline::CreateZones(double labelHeight, bool labelFaceCamera) {
    double gap = 40.0;
    double radius = (mapExtents[3] - mapExtents[1]) * 0.5 * 0.5;
    double y = mapExtents[3] + radius + gap;


    // Create zone for unknown positions
    unknownPos.Set(mapExtents[0] + (mapExtents[2] - mapExtents[0]) * 0.25, y);
    vtkDiskSource* unknown = vtkDiskSource::New();
    unknown->SetCircumferentialResolution(32);
    unknown->SetRadialResolution(1);
    unknown->SetInnerRadius(0.0);
    unknown->SetOuterRadius(radius);
    vtkPolyDataMapper* unknownMapper = vtkPolyDataMapper::New();
    unknownMapper->SetInputConnection(unknown->GetOutputPort());
    vtkActor* unknownActor = vtkActor::New();
    unknownActor->SetMapper(unknownMapper);
    unknownActor->SetPosition(unknownPos.X(), unknownPos.Y(), 0.0);
    unknownActor->SetScale(1.5, 1.0, 1.0);

    // Create a border
    vtkDiskSource* unknownBorder = vtkDiskSource::New();
    unknownBorder->SetCircumferentialResolution(32);
    unknownBorder->SetRadialResolution(1);
    unknownBorder->SetInnerRadius(radius);
    unknownBorder->SetOuterRadius(radius + 2.0);
    vtkPolyDataMapper* unknownBorderMapper = vtkPolyDataMapper::New();
    unknownBorderMapper->SetInputConnection(unknownBorder->GetOutputPort());
    vtkActor* unknownBorderActor = vtkActor::New();
    unknownBorderActor->SetMapper(unknownBorderMapper);
    unknownBorderActor->SetPosition(unknownPos.X(), unknownPos.Y(), 0.0);
    unknownBorderActor->SetScale(1.5, 1.0, 1.0);

    // Turn off lighting for the unknown zone
    if (darkBackground) {
        unknownActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
        unknownBorderActor->GetProperty()->SetColor(0.5, 0.5, 0.5);
    }
    else {
        unknownActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
        unknownBorderActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
    }
    unknownActor->GetProperty()->SetAmbient(1.0);
    unknownActor->GetProperty()->SetDiffuse(0.0);
    unknownActor->GetProperty()->SetSpecular(0.0);
    unknownActor->GetProperty()->SetOpacity(0.5);
    unknownBorderActor->GetProperty()->SetAmbient(1.0);
    unknownBorderActor->GetProperty()->SetDiffuse(0.0);
    unknownBorderActor->GetProperty()->SetSpecular(0.0);
    unknownBorderActor->GetProperty()->SetOpacity(0.5);

    // Add a label for the unknown zone
    if (labelFaceCamera) {
        vtkCaptionActor2D* unknownLabel = vtkCaptionActor2D::New();
        unknownLabel->SetCaption("Unknown Positions");
        unknownLabel->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
        unknownLabel->GetPositionCoordinate()->SetValue(-0.5, -labelHeight);
        unknownLabel->SetAttachmentPoint(mapExtents[0] + (mapExtents[2] - mapExtents[0]) * 0.25, y - radius, 0.0);
        unknownLabel->SetWidth(1.0);
        unknownLabel->SetHeight(labelHeight);
        unknownLabel->BorderOff();
        unknownLabel->ThreeDimensionalLeaderOff();
        unknownLabel->GetCaptionTextProperty()->SetJustificationToCentered();
        unknownLabel->GetCaptionTextProperty()->BoldOff();
        unknownLabel->GetCaptionTextProperty()->ShadowOff();
        unknownLabel->GetCaptionTextProperty()->ItalicOff();
        unknownLabel->GetProperty()->SetLineStipplePattern(0x0000);
        if (darkBackground) unknownLabel->GetProperty()->SetColor(1.0, 1.0, 1.0);
        else unknownLabel->GetProperty()->SetColor(0.0, 0.0, 0.0);
        
        renderer->AddViewProp(unknownLabel);

        unknownLabel->Delete();
    }
    else {
        vtkTextActor3D* unknownLabel = vtkTextActor3D::New();
        unknownLabel->SetInput("Unknown Positions");
        unknownLabel->SetPosition(mapExtents[0] + (mapExtents[2] - mapExtents[0]) * 0.2, y - radius * 1.1, 0.0);
        unknownLabel->GetTextProperty()->SetJustificationToCentered();
        unknownLabel->GetTextProperty()->BoldOn();
        unknownLabel->GetTextProperty()->ShadowOff();
        unknownLabel->GetTextProperty()->ItalicOff();
        if (darkBackground) unknownLabel->GetTextProperty()->SetColor(1.0, 1.0, 1.0);
        else unknownLabel->GetTextProperty()->SetColor(0.0, 0.0, 0.0);

        renderer->AddViewProp(unknownLabel);

        unknownLabel->Delete();
    }


    // Create zone for off-the-map positions
    offTheMapPos.Set(mapExtents[0] + (mapExtents[2] - mapExtents[0]) * 0.75, y);
    vtkDiskSource* offTheMap = vtkDiskSource::New();
    offTheMap->SetCircumferentialResolution(32);
    offTheMap->SetRadialResolution(1);
    offTheMap->SetInnerRadius(0.0);
    offTheMap->SetOuterRadius(radius);
    vtkPolyDataMapper* offTheMapMapper = vtkPolyDataMapper::New();
    offTheMapMapper->SetInputConnection(offTheMap->GetOutputPort());
    offTheMapActor = vtkActor::New();
    offTheMapActor->SetMapper(offTheMapMapper);
    offTheMapActor->SetPosition(offTheMapPos.X(), offTheMapPos.Y(), 0.0);
    offTheMapActor->SetScale(1.5, 1.0, 1.0);

    // Create a border
    vtkDiskSource* offTheMapBorder = vtkDiskSource::New();
    offTheMapBorder->SetCircumferentialResolution(32);
    offTheMapBorder->SetRadialResolution(1);
    offTheMapBorder->SetInnerRadius(radius);
    offTheMapBorder->SetOuterRadius(radius + 2.0);
    vtkPolyDataMapper* offTheMapBorderMapper = vtkPolyDataMapper::New();
    offTheMapBorderMapper->SetInputConnection(offTheMapBorder->GetOutputPort());
    offTheMapBorderActor = vtkActor::New();
    offTheMapBorderActor->SetMapper(offTheMapBorderMapper);
    offTheMapBorderActor->SetPosition(offTheMapPos.X(), offTheMapPos.Y(), 0.0);
    offTheMapBorderActor->SetScale(1.5, 1.0, 1.0);

    // Turn off lighting for the off-the-map zone
    if (darkBackground) {
        offTheMapActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
        offTheMapBorderActor->GetProperty()->SetColor(0.5, 0.5, 0.5);
    }
    else {
        offTheMapActor->GetProperty()->SetColor(1.0, 1.0, 1.0);
        offTheMapBorderActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
    }
    offTheMapActor->GetProperty()->SetAmbient(1.0);
    offTheMapActor->GetProperty()->SetDiffuse(0.0);
    offTheMapActor->GetProperty()->SetSpecular(0.0);
    offTheMapActor->GetProperty()->SetOpacity(0.5);    
    offTheMapBorderActor->GetProperty()->SetAmbient(1.0);
    offTheMapBorderActor->GetProperty()->SetDiffuse(0.0);
    offTheMapBorderActor->GetProperty()->SetSpecular(0.0);
    offTheMapBorderActor->GetProperty()->SetOpacity(0.5);

    // Add a label for the off-the-map zone
    if (labelFaceCamera) {
        offTheMapLabel = vtkCaptionActor2D::New();
        offTheMapLabel->SetCaption("Off-the-map Locations");
        offTheMapLabel->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
        offTheMapLabel->GetPositionCoordinate()->SetValue(-0.5, -labelHeight);
        offTheMapLabel->SetAttachmentPoint(mapExtents[0] + (mapExtents[2] - mapExtents[0]) * 0.75, y - radius, 0.0);
        offTheMapLabel->SetWidth(1.0);
        offTheMapLabel->SetHeight(labelHeight);
        offTheMapLabel->BorderOff();
        offTheMapLabel->ThreeDimensionalLeaderOff();
        offTheMapLabel->GetCaptionTextProperty()->SetJustificationToCentered();
        offTheMapLabel->GetCaptionTextProperty()->BoldOff();
        offTheMapLabel->GetCaptionTextProperty()->ShadowOff();
        offTheMapLabel->GetCaptionTextProperty()->ItalicOff();
        offTheMapLabel->GetProperty()->SetLineStipplePattern(0x0000);
        if (darkBackground) offTheMapLabel->GetProperty()->SetColor(1.0, 1.0, 1.0);
        else offTheMapLabel->GetProperty()->SetColor(0.0, 0.0, 0.0);

        renderer->AddViewProp(offTheMapLabel);

        offTheMapLabel3D = NULL;
    }
    else {
        offTheMapLabel3D = vtkTextActor3D::New();
        offTheMapLabel3D->SetInput("Off-the-map Locations");
        offTheMapLabel3D->SetPosition(mapExtents[0] + (mapExtents[2] - mapExtents[0]) * 0.7, y - radius * 1.1, 0.0);
        offTheMapLabel3D->GetTextProperty()->SetJustificationToCentered();
        offTheMapLabel3D->GetTextProperty()->BoldOn();
        offTheMapLabel3D->GetTextProperty()->ShadowOff();
        offTheMapLabel3D->GetTextProperty()->ItalicOff();
        if (darkBackground) offTheMapLabel3D->GetTextProperty()->SetColor(1.0, 1.0, 1.0);
        else offTheMapLabel3D->GetTextProperty()->SetColor(0.0, 0.0, 0.0);

        renderer->AddViewProp(offTheMapLabel3D);

        offTheMapLabel = NULL;
    }


    // Add to the renderer
    renderer->AddViewProp(unknownActor);
    renderer->AddViewProp(unknownBorderActor);
    renderer->AddViewProp(offTheMapActor);
    renderer->AddViewProp(offTheMapBorderActor);


    // Don't need these references any more
    unknown->Delete();
    unknownMapper->Delete();
    unknownActor->Delete();
    unknownBorder->Delete();
    unknownBorderMapper->Delete();
    unknownBorderActor->Delete();
    offTheMap->Delete();
    offTheMapMapper->Delete();
    offTheMapBorder->Delete();
    offTheMapBorderMapper->Delete();    
}


void RenderPipeline::CreateMovieMaker() {
    // Connect to an avi writer
    windowCapture = vtkWindowToImageFilter::New();
    windowCapture->SetInput(renderer->GetRenderWindow());
    aviWriter = vtkAVIWriter::New();
    aviWriter->SetInputConnection(windowCapture->GetOutputPort());
    recordingMovie = false;
    movieNumber = 1;

    // Callback for each frame
    rendererCallback = RendererCallback::New();
    rendererCallback->SetPipeline(this);
    renderer->AddObserver(vtkCommand::EndEvent, rendererCallback);
}
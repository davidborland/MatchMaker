///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        VTKCallbacks.cpp
//
// Author:      David Borland
//
// Description: Classes derived from vtkCommand to handle interaction events in MatchMaker.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "VTKCallbacks.h"

#include "Engine.h"
#include "RenderPipeline.h"

#include <wx/log.h>


RendererCallback::RendererCallback() {
    pipeline = NULL;
}


RendererCallback* RendererCallback::New() {
    return new RendererCallback;
}

void RendererCallback::Execute(vtkObject* caller, unsigned long eventId, void* callData) {
    if (pipeline) {
        if (eventId == EndEvent) {
            pipeline->HandleRender();
        }
    }
}

void RendererCallback::SetPipeline(RenderPipeline* pipe) {
    pipeline = pipe;
}


///////////////////////////////////////////////////////////////////////////////////


KeyPressCallback::KeyPressCallback() {
    pipeline = NULL;
}


KeyPressCallback* KeyPressCallback::New() {
    return new KeyPressCallback;
}

void KeyPressCallback::Execute(vtkObject* caller, unsigned long eventId, void* callData) {
    vtkRenderWindowInteractor* interactor = vtkRenderWindowInteractor::SafeDownCast(caller);
    if (pipeline) {
        if (eventId == KeyPressEvent) {
            char c = interactor->GetKeyCode();

            if (c == 'd') {
                pipeline->ToggleDome();
            }
            else if (c == 'f') {
                pipeline->ToggleFullScreen();
            }
            else if (c == 'm') {
                pipeline->ToggleMovie();
            }
            else if (c == 'p') {
                engine->TogglePause();
            }
            else if (c == 'q') {
                exit(0);
            }
            else if (c == 's') {
                pipeline->ToggleStereo();
            }
            else if (c == 'w') {
                engine->ChangeWorkflow();
            }
        }
    }
}

void KeyPressCallback::SetEngineAndPipeline(Engine* eng, RenderPipeline* pipe) {
    engine = eng;
    pipeline = pipe;
}
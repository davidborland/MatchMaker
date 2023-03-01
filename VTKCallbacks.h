///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        VTKCallbacks.h
//
// Author:      David Borland
//
// Description: Classes derived from vtkCommand to handle interaction events in MatchMaker.
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef VTKCALLBACKS_H
#define VTKCALLBACKS_H


#include <vtkCommand.h>


class Engine;
class RenderPipeline;


class RendererCallback : public vtkCommand {
public:
    RendererCallback();
    static RendererCallback* New();

    virtual void Execute(vtkObject* caller, unsigned long eventId, void* callData);

    void SetPipeline(RenderPipeline* pipe);

private:
    RenderPipeline* pipeline;
};


///////////////////////////////////////////////////////////////////////////////////


class KeyPressCallback : public vtkCommand {
public:
    KeyPressCallback();
    static KeyPressCallback* New();

    virtual void Execute(vtkObject* caller, unsigned long eventId, void* callData);

    void SetEngineAndPipeline(Engine* eng, RenderPipeline* pipe);

private:    
    Engine* engine;
    RenderPipeline* pipeline;
};


#endif
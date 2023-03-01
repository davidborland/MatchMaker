///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        vtkMyInteractorStyleTrackballCamera.cxx
//
// Author:      David Borland
//
// Description: Override keypresses.
//
///////////////////////////////////////////////////////////////////////////////////////////////

#include "vtkMyInteractorStyleTrackballCamera.h"

#include <vtkAbstractPicker.h>
#include <vtkAbstractPropPicker.h>
#include <vtkAssemblyPath.h>
#include <vtkObjectFactory.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include <wx/log.h>

vtkCxxRevisionMacro(vtkMyInteractorStyleTrackballCamera, "$Revision: 1.3 $");
vtkStandardNewMacro(vtkMyInteractorStyleTrackballCamera);

//----------------------------------------------------------------------------
void vtkMyInteractorStyleTrackballCamera::OnChar()
{
  // Don't want default vtk behavior for keypresses
  vtkRenderWindowInteractor *rwi = this->Interactor;

  // XXX: Why no keypress events???
  wxLogMessage("Keypress: %d", rwi->GetKeyCode());
/*
  switch (rwi->GetKeyCode()) 
    {
    case 'm' :
    case 'M' :
      if (this->AnimState == VTKIS_ANIM_OFF) 
        {
        this->StartAnimate();
        }
      else 
        {
        this->StopAnimate();
        }
      break;

    case 'Q' :
    case 'q' :
    case 'e' :
    case 'E' :
      rwi->ExitCallback();
      break;

    case 'f' :      
    case 'F' :
      {
      this->AnimState = VTKIS_ANIM_ON;
      vtkAssemblyPath *path = NULL;
      this->FindPokedRenderer(rwi->GetEventPosition()[0],
                              rwi->GetEventPosition()[1]);
      rwi->GetPicker()->Pick(rwi->GetEventPosition()[0],
                             rwi->GetEventPosition()[1], 
                             0.0, 
                             this->CurrentRenderer);
      vtkAbstractPropPicker *picker;
      if ((picker=vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker())))
        {
        path = picker->GetPath();
        }
      if (path != NULL)
        {
        rwi->FlyTo(this->CurrentRenderer, picker->GetPickPosition());
        }
      this->AnimState = VTKIS_ANIM_OFF;
      }
      break;

    case 'u' :
    case 'U' :
      rwi->UserCallback();
      break;

    case 'r' :
    case 'R' :
      this->FindPokedRenderer(rwi->GetEventPosition()[0], 
                              rwi->GetEventPosition()[1]);
      this->CurrentRenderer->ResetCamera();
      rwi->Render();
      break;

    case 'w' :
    case 'W' :
      {
      vtkActorCollection *ac;
      vtkActor *anActor, *aPart;
      vtkAssemblyPath *path;
      this->FindPokedRenderer(rwi->GetEventPosition()[0],
                              rwi->GetEventPosition()[1]);
      ac = this->CurrentRenderer->GetActors();
      vtkCollectionSimpleIterator ait;
      for (ac->InitTraversal(ait); (anActor = ac->GetNextActor(ait)); ) 
        {
        for (anActor->InitPathTraversal(); (path=anActor->GetNextPath()); ) 
          {
          aPart=(vtkActor *)path->GetLastNode()->GetViewProp();
          aPart->GetProperty()->SetRepresentationToWireframe();
          }
        }
      rwi->Render();
      }
      break;

    case 's' :
    case 'S' :
      {
      vtkActorCollection *ac;
      vtkActor *anActor, *aPart;
      vtkAssemblyPath *path;
      this->FindPokedRenderer(rwi->GetEventPosition()[0],
                              rwi->GetEventPosition()[1]);
      ac = this->CurrentRenderer->GetActors();
      vtkCollectionSimpleIterator ait;
      for (ac->InitTraversal(ait); (anActor = ac->GetNextActor(ait)); ) 
        {
        for (anActor->InitPathTraversal(); (path=anActor->GetNextPath()); ) 
          {
          aPart=(vtkActor *)path->GetLastNode()->GetViewProp();
          aPart->GetProperty()->SetRepresentationToSurface();
          }
        }
      rwi->Render();
      }
      break;

    case '3' :
      if (rwi->GetRenderWindow()->GetStereoRender()) 
        {
        rwi->GetRenderWindow()->StereoRenderOff();
        }
      else 
        {
        rwi->GetRenderWindow()->StereoRenderOn();
        }
      rwi->Render();
      break;

    case 'p' :
    case 'P' :
      if (this->State == VTKIS_NONE) 
        {
        vtkAssemblyPath *path = NULL;
        int *eventPos = rwi->GetEventPosition();
        this->FindPokedRenderer(eventPos[0], eventPos[1]);
        rwi->StartPickCallback();
        vtkAbstractPropPicker *picker = 
          vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker());
        if ( picker != NULL )
          {
          picker->Pick(eventPos[0], eventPos[1], 
                       0.0, this->CurrentRenderer);
          path = picker->GetPath();
          }
        if ( path == NULL )
          {
          this->HighlightProp(NULL);
          this->PropPicked = 0;
          }
        else
          {
          this->HighlightProp(path->GetFirstNode()->GetViewProp());
          this->PropPicked = 1;
          }
        rwi->EndPickCallback();
        }
      break;
    }
*/
}
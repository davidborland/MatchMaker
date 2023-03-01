CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

PROJECT( MatchMaker )

SET( EXECUTABLE_OUTPUT_PATH "${MatchMaker_BINARY_DIR}/bin" )
SET( LIBRARY_OUTPUT_PATH "${MatchMaker_BINARY_DIR}/lib" )

OPTION( CMAKE_VERBOSE_MAKEFILE  "Enable/Disable verbose compiler output" ON )
OPTION( CMAKE_COLOR_MAKEFILE "Enable/Disable color cues when building" ON )
MARK_AS_ADVANCED( CLEAR CMAKE_VERBOSE_MAKEFILE CMAKE_COLOR_MAKEFILE )


#######################################
# Include VTK
#######################################

FIND_PACKAGE( VTK )
IF( VTK_FOUND )
  INCLUDE( ${VTK_USE_FILE} )
ELSE( VTK_FOUND )
  MESSAGE( FATAL_ERROR "Cannot build without VTK.  Please set VTK_DIR." )
ENDIF( VTK_FOUND )

INCLUDE_DIRECTORIES ( ${VTK_INCLUDE_DIRS} )
LINK_DIRECTORIES ( ${VTK_LIBRARY_DIRS} )

SET( VTK_LIBS vtkCommon
              vtkexpat
              vtkFiltering
              vtkfreetype
              vtkftgl
              vtkGenericFiltering
              vtkGraphics
              vtkHybrid
              vtkImaging
              vtkIO
              vtkjpeg
              vtklibxml2.lib
              vtkpng.lib
              vtkRendering
              vtksys
              vtkWidgets
              vtkzlib )                    


#######################################
# Include wxWidgets
#######################################

INCLUDE( LocalUsewxWidgets.cmake )
INCLUDE( ${CMAKE_ROOT}/Modules/UsewxWidgets.cmake )
INCLUDE_DIRECTORIES( ${wxWidgets_INCLUDES} )
LINK_DIRECTORIES( ${wxWidgets_LIBRARY_DIRS} )


#######################################
# Include wxVTK
#######################################

FIND_PATH( wxVTK_SRC_DIR wxVTKRenderWindowInteractor.h )
INCLUDE_DIRECTORIES( ${wxVTK_SRC_DIR} )
SET( wxVTK_SRC ${wxVTK_SRC_DIR}/wxVTKRenderWindowInteractor.h ${wxVTK_SRC_DIR}/wxVTKRenderWindowInteractor.cxx )


#######################################
# Include Haggis
#######################################

FIND_PATH( HAGGIS_SRC_DIR SCR/SCRFrame.h )
FIND_PATH( HAGGIS_BIN_DIR Haggis.sln )

INCLUDE_DIRECTORIES( ${HAGGIS_SRC_DIR}/Quat)
LINK_DIRECTORIES( ${HAGGIS_BIN_DIR}/Quat )

SET( HAGGIS_LIBS Quat.lib )



#######################################
# Include MatchMaker code
#######################################

SET( SRC ConfigFileParser.h ConfigFileParser.cpp
         DataTransfer.h DataTransfer.cpp
         Engine.h Engine.cpp
         Job.h Job.cpp
         JobList.h JobList.cpp
         MatchMaker.h MatchMaker.cpp
         NetworkConnection.h NetworkConnection.cpp
         NetworkConnectionList.h NetworkConnectionList.cpp
         Object.h Object.cpp
         Projector.h Projector.cpp
         RenderPipeline.h RenderPipeline.cpp
         Site.h Site.cpp
         SiteList.h SiteList.cpp
         Socket.h Socket.cpp
         Stack.h Stack.cpp
         TextFileSocket.h TextFileSocket.cpp
         VTKCallbacks.h VTKCallbacks.cpp
         vtkMyInteractorStyleTrackballCamera.h vtkMyInteractorStyleTrackballCamera.cxx
         Workflow.h WorkFlow.cpp
         WorkflowList.h WorkflowList.cpp )
ADD_EXECUTABLE( MatchMaker WIN32 MACOSX_BUNDLE ${SRC} ${wxVTK_SRC} )
TARGET_LINK_LIBRARIES( MatchMaker ${VTK_LIBS} ${HAGGIS_LIBS} )  
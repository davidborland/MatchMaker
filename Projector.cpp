///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Projector.cpp
//
// Author:      David Borland
//
// Description: Implementation of Projector for projecting longitudes and latitudes to xy.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#include "Projector.h"

#include <math.h>

#include <vtkMath.h>

#include <wx/log.h>


double Projector::scale = 1000.0;

double Projector::xScale = 1.0;
double Projector::yScale = 1.0;


Projector::Projector() {
}

Projector::~Projector() {
}


void Projector::SetXScale(double scale) {
    xScale = scale;
}

void Projector::SetYScale(double scale) {
    yScale = scale;
}


double Projector::ProjectLongitude(double longitude) {
    return vtkMath::RadiansFromDegrees(longitude) * scale * xScale;
}

double Projector::ProjectLatitude(double latitude) {
    double lat = latitude >= 90.0 ? 89.9 : latitude;
    double rad = vtkMath::RadiansFromDegrees(lat);;

    return log(tan(rad) + 1.0 / cos(rad)) * scale * yScale;
}
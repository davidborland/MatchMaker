///////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:        Projector.h
//
// Author:      David Borland
//
// Description: Interface of Projector for projecting longitudes and latitudes to xy.  
//
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef PROJECTOR_H
#define PROJECTOR_H


class Projector {
public:
    Projector();
    ~Projector();

    static void SetXScale(double scale);
    static void SetYScale(double scale);

    static double ProjectLongitude(double longitude);
    static double ProjectLatitude(double latitude);

private:
    static double scale;

    static double xScale;
    static double yScale;
};


#endif
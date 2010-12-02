#ifndef SCENE_H
#define SCENE_H

#include <qgl.h>
#include "3dobject.h"

//Constants
#define INF_DIST 100
#define MAX_OBJECTS 10
#define NO_SELECTION -1

/* global variables */
struct Scene{
    Scene() {
        selectedProp = NO_SELECTION;
    }

    MyObject obj[MAX_OBJECTS];			//objects
    MyObject prop[MAX_OBJECTS];                 //props
    int objcount;				//number of loaded objects
    int propcount;				//number of loaded props
    int selectedProp;				//index of selected prop

    GLUquadricObj	*q;			// Quadratic For Drawing Light Spheres

    float CameraPos;				// Camera Distance to Origin
    float CameraRot[4];				// Camera Rotation

    float LightPos1[4];			        // Light 1 Position
    float LightAmb1[4];				// Ambient Light Values
    float LightDif1[4];				// Diffuse Light Values
    float LightSpc1[4];				// Specular Light Values

    float LightPos2[4];				// Light 2 Position
    float LightAmb2[4];	    			// Ambient Light Values
    float LightDif2[4];				// Diffuse Light Values
    float LightSpc2[4];				// Specular Light Values
};

#endif // SCENE_H

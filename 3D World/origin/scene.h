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
};

#endif // SCENE_H

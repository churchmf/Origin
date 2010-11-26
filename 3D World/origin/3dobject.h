/*
This file is borrowed from Assignment3 of CMPUT411 at the University of Alberta.
*/
#ifndef _3DOBJECT_H_
#define _3DOBJECT_H_

#include <stdio.h>
#include <qgl.h>

//Constants - Do not touch these
#define MAX_OBJECT_POINTS 800
#define MAX_OBJECT_PLANES 800
#define MAX_PLANE_POINTS 10

typedef float GLvector4f[4];		// Typedef's For VMatMult Procedure
typedef float GLmatrix16f[16];		// Typedef's For VMatMult Procedure

//matrix multiplication
//(we have to do this on CPU because OpenGL does not provide a suitable interface for
//multiplying a vector by a matrix
void VMatMult(GLmatrix16f M, GLvector4f v);

// point/vector in 3d-coordinate system
struct MyPoint{
    //fields
	float x, y, z;
    //methods
        MyPoint cross(MyPoint op2);
        MyPoint minus(MyPoint op2);
        MyPoint normalize();
        float dot(MyPoint op2);
        MyPoint times(float c);
};

// structure describing an object's face
struct MyPlane{
        MyPoint color;					//plane color
        unsigned int nPoints;				//vertices in plane
        unsigned int pids[MAX_PLANE_POINTS];		//id of points of this plane
        unsigned int neigh[MAX_PLANE_POINTS];		//neighbor planes
        MyPoint normal;					//normal vector
        bool islit;					//is it facing the light?
};

// object structure
struct MyObject{
    //fields
        unsigned int nPlanes;				//number of surfaces that make up this object
        unsigned int nPoints;				//number of vertices that make up this object
        MyPoint points[MAX_OBJECT_POINTS];		//points
        MyPlane planes[MAX_OBJECT_PLANES];		//surfaces

        MyPoint position;				//location
        MyPoint rotation;				//rotation around (x,y,z) axes

        bool castsShadow;				//does it cast a shadow?
    //methods
        static int ReadObject(char *st, MyObject *o);	//Loads an object from file
        void assessVisibility(float *lp);		//decide which planes of the object are lit
        void castShadow(float *lp);			//generate shadow quads
        void renderShadow(float *lp);			//draw shadows using stencil buffer and castShadow
        void draw();					//draw object
        void setConnectivity();				//find neighbor planes for each plane of the object
        void print();					//for debug
        void calcNormals();				//calculate normals for all planes
};


#endif // _3DOBJECT_H_

/*
This file is borrowed from Assignment3 of CMPUT411 at the University of Alberta.
*/
#ifndef _3DOBJECT_H_
#define _3DOBJECT_H_

#include <stdio.h>
#include <qgl.h>

//Constants
#define MAX_OBJECT_POINTS 100
#define MAX_OBJECT_PLANES 100
#define MAX_OBJECT_NORMALS 100
#define MAX_PLANE_POINTS 10
#define PROP_TRANSFORM_STEP 0.01f
#define PROP_ROTATE_STEP 0.01f
#define PROP_SCALE_STEP 0.01f

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
    float length(MyPoint p2);
    bool equals(MyPoint p2);
    bool isNear(MyPoint p2);
    MyPoint plus(MyPoint p2);
    MyPoint scale(MyPoint p2);
    MyPoint divide(float c);
};

struct MyUV {
    float u, v;
};

// structure describing an object's face
struct MyPlane{
    MyPoint color;					//plane color
    unsigned int nPoints;				//vertices in plane
    unsigned int pids[MAX_PLANE_POINTS];		//id of points of this plane
    unsigned int tids[MAX_PLANE_POINTS];		//id of textureCoords of this plane
    unsigned int nids[MAX_PLANE_POINTS];		//id of normals of this plane
    unsigned int neigh[MAX_PLANE_POINTS];		//neighbor planes
    bool islit;                                         //is it facing the light?

    MyPoint getNormal();
};

struct MyMaterial{
    //fields
    QColor ka; //ambient color of the material
    QColor kd; //diffuse color of the material
    QColor ks; //specular color of the material
    float alpha; //transparency of the material to be alpha
    float s; //shininess of the material
    GLuint texture; //material's texture
};

// object structure
struct MyObject{
    MyObject()
    {
        scale.x = 1;
        scale.y = 1;
        scale.z = 1;
        goalScale.x = 1;
        goalScale.y = 1;
        goalScale.z = 1;

        position.x = 0;
        position.y = 0;
        position.z = 0;
        goalPosition.x = 0;
        goalPosition.y = 0;
        goalPosition.z = 0;

        rotation.x = 0;
        rotation.y = 0;
        rotation.z = 0;
        goalRotation.x = 0;
        goalRotation.y = 0;
        goalRotation.z = 0;
    }

    //fields
    unsigned int nPlanes;				//number of surfaces that make up this object
    unsigned int nPoints;				//number of vertices that make up this object
    MyPoint points[MAX_OBJECT_POINTS];                  //points
    MyPlane planes[MAX_OBJECT_PLANES];                  //surfaces
    MyPoint normals[MAX_OBJECT_NORMALS];                //normals
    MyUV    textureCoords[MAX_OBJECT_POINTS];           //textureCoords

    MyPoint position;                                  //location
    MyPoint rotation;                                  //rotation around (x,y,z) axes
    MyPoint scale;                                     //scale
    MyPoint goalPosition;                              //goal position
    MyPoint goalScale;                                 //goal scale
    MyPoint goalRotation;                              //goal rotation


    MyMaterial material;                               //object's material

    //methods
    void draw();					//draw object
    //void scale(MyPoint k);                            //scale an object(x,y,z) by the point(kx,ky,kz)
};


#endif // _3DOBJECT_H_

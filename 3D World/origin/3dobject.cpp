#include <math.h>
#include "scene.h"						// Header File For 3D Object Handling

void VMatMult(GLmatrix16f M, GLvector4f v)
{
    GLfloat res[4];										// Hold Calculated Results
    res[0]=M[ 0]*v[0]+M[ 4]*v[1]+M[ 8]*v[2]+M[12]*v[3];
    res[1]=M[ 1]*v[0]+M[ 5]*v[1]+M[ 9]*v[2]+M[13]*v[3];
    res[2]=M[ 2]*v[0]+M[ 6]*v[1]+M[10]*v[2]+M[14]*v[3];
    res[3]=M[ 3]*v[0]+M[ 7]*v[1]+M[11]*v[2]+M[15]*v[3];
    v[0]=res[0];										// Results Are Stored Back In v[]
    v[1]=res[1];
    v[2]=res[2];
    v[3]=res[3];										// Homogenous Coordinate
}
/*Begin ********* MyPoint methods*/
// subtraction of two vectors/points
MyPoint MyPoint::minus(MyPoint op2)
{
    MyPoint res;
    res.x = x - op2.x;
    res.y = y - op2.y;
    res.z = z - op2.z;
    return res;
}
// cross product of two vectors
MyPoint MyPoint::cross(MyPoint op2)
{
    MyPoint res;
    res.x = y * op2.z - z * op2.y;
    res.y = z * op2.x - x * op2.z;
    res.z = x * op2.y - y * op2.x;
    return res;
}
//dot product of two vectors
float MyPoint::dot(MyPoint op2){	
    MyPoint res;
    res.x = x * op2.x;
    res.y = y * op2.y;
    res.z = z * op2.z;
    return res.x+ res.y+ res.z;
}

//scalar multiplication
MyPoint MyPoint::times(float c){
    MyPoint res;
    res.x= x*c;
    res.y= y*c;
    res.z= z*c;
    return res;
}
//matrix scaling
MyPoint MyPoint::scale(MyPoint p2){
    MyPoint res;
    res.x= x*p2.x;
    res.y= y*p2.y;
    res.z= z*p2.z;
    return res;
}

MyPoint MyPoint::divide(float c)
{
    MyPoint res;
    res.x= x/c;
    res.y= y/c;
    res.z= z/c;
    return res;
}

float MyPoint::length(MyPoint p2)
{
    float diffX = p2.x - x;
    float diffY = p2.y - y;
    float diffZ = p2.z - z;
    return sqrt (diffX * diffX + diffY * diffY + diffZ * diffZ);
}

bool MyPoint::equals(MyPoint p2)
{
    return x == p2.x && y == p2.y && z == p2.z;
}

MyPoint MyPoint::plus(MyPoint p2)
{
    MyPoint res;
    res.x= x+p2.x;
    res.y= y+p2.y;
    res.z= z+p2.z;
    return res;
}

//return a vector with length 1 in the same direction
MyPoint MyPoint::normalize(){
    float len= sqrt(x*x+y*y+z*z);
    MyPoint res= times(1.0f/len);
    return res;
}

/*END *********** MyPoint methods*/
void MyObject::draw()
{
    //Bind the texture
    glBindTexture(GL_TEXTURE_2D, this->material.texture);

    // For each plane,
    for(unsigned int i=0; i<nPlanes; i++){
        // Get the plane object.
        MyPlane plane = planes[i];

        // Start adding vertices to the polygon.
        glBegin(GL_POLYGON);
        // For each vertex on the plane,
        for(unsigned int j=0; j<plane.nPoints; j++){
            // Get the vertex, normal and textureCoord.
            MyPoint point = points[plane.pids[j]];
            MyPoint normal = normals[plane.nids[j]];
            MyUV textureCoord = textureCoords[plane.tids[j]];

            // Set the normal
            glNormal3f(normal.x, normal.y, normal.z);

            // Make the texture coordinates
            glTexCoord2f(textureCoord.u,textureCoord.v);

            // Add it to the polygon.
            glVertex3f(point.x, point.y, point.z);
        }
        glEnd();
    }
}

//scale an object(x,y,z) by the point(kx,ky,kz)
/*
void MyObject::scale(MyPoint k)
{
    for (unsigned int i=0;i<nPoints;i++){
        MyPoint& point = points[i];
        point.x *= k.x;
        point.y *= k.y;
        point.z *= k.z;
    }
}
*/

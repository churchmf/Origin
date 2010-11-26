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
//return a vector with length 1 in the same direction
MyPoint MyPoint::normalize(){
    float len= sqrt(x*x+y*y+z*z);
    MyPoint res= times(1/len);
    return res;
}
/*END *********** MyPoint methods*/
//prints out object - for debugging purposes only
void MyObject::print(){

    unsigned int i;
    printf("object:%d %d\n", nPoints, nPlanes);

    //planes
    for (i=0;i<nPlanes;i++){
	printf( "Plane: \n%d, %f %f %f: \n", planes[i].nPoints, planes[i].normal.x, planes[i].normal.y, planes[i].normal.z);
	printf(" color {%f %f %f}\n", planes[i].color.x, planes[i].color.y, planes[i].color.z);
	//print vertices
	for(int j=0; j<planes[i].nPoints; j++){
	    printf("\t(%f %f %f)\n", points[planes[i].pids[j]].x, points[planes[i].pids[j]].y, points[planes[i].pids[j]].z );
	}
    }
}

// load object from file
int MyObject::ReadObject(char *st, MyObject *o){
    FILE *file;
    unsigned int i,j;

    file = fopen(st, "r");
    if (!file) return false;
    //points
    fscanf(file, "%d", &(o->nPoints));
    for (i=1;i<=o->nPoints;i++){
	fscanf(file, "%f", &(o->points[i].x));
	fscanf(file, "%f", &(o->points[i].y));
	fscanf(file, "%f", &(o->points[i].z));
    }
    //planes
    fscanf(file, "%d", &(o->nPlanes));
    for (i=0;i<o->nPlanes;i++){
	fscanf(file, "%d", &(o->planes[i].nPoints));
	//read vertices
	for(j=0; j<o->planes[i].nPoints; j++){
	    fscanf(file, "%d", &(o->planes[i].pids[j]));
	}
	fscanf(file, "%f", &(o->planes[i].color.x));
	fscanf(file, "%f", &(o->planes[i].color.y));
	fscanf(file, "%f", &(o->planes[i].color.z));
    }
    o->castsShadow=1;
    //o->print();
    return true;
}

// set neighbors of each plane
// planes[i].neigh[j] should be set to index of neighbor with which plane i shares the edge
// between its jth th and j+1th vertices.
void MyObject::setConnectivity(){
    /*
	ENTER CODE HERE (for problem 3)
	set neighbor array for each plane of the object

	for all planes p1 in obj
		for all planes p2 in obj (p2!=p1)
			for all edges e1 in p1
				for all edges e2 in p2
					if e1==e2
						set neighbor of e1 to e2
    */
}

// procedure for drawing the object - very simple
void MyObject::draw(){
    /*
	ENTER CODE HERE (for problems 1,2,3)
	draw the object using some GL_POLYGON s
	Don't forget to use glColor3f and glNormalxx before drawing each polygon
	consult MyObject struct (in 3dobject.h) to see how objects are structured
    */
    // For each plane,
    for(int i=0; i<nPlanes; i++){
        // Get the plane object.
        MyPlane plane = planes[i];

        // Set the colour of the plane to the MyPoint called Color...
        glColor3f(plane.color.x, plane.color.y, plane.color.z);

        // Set the normal?
        glNormal3f(plane.normal.x, plane.normal.y, plane.normal.z);

        // Start adding vertices to the polygon.
        glBegin(GL_POLYGON);
        // For each vertex on the plane,
        for(int j=0; j<plane.nPoints; j++){
            // Get the vertex.
            MyPoint point = points[plane.pids[j]];
            // Add it to the polygon.
            glVertex3f(point.x, point.y, point.z);
        }
        glEnd();
    }

}

//calculates normal vector for all planes of the object using 3 of its vertices
void MyObject::calcNormals(){
    for (unsigned int i=0;i<nPlanes;i++){
	planes[i].normal = points[planes[i].pids[1]].minus( points[planes[i].pids[0]] ).cross( points[planes[i].pids[2]].minus( points[planes[i].pids[1]] ) ).normalize();;
    }
}

//calculate which planes of the object are lit
//lp is in object coordinate system
//set planes[i].islit for all planes of the object
void MyObject::assessVisibility(float *lp){
    /*
	ENTER CODE HERE (for problem 3)
	A plane is lit if dot product of its normal vector and
	vector from an arbitrary point on the surface to light position is positive
    */
}

/*
	compute and draw shadow surfaces
*/
void MyObject::castShadow(float *lp){
    /*
	ENTER CODE HERE (for problem 3)
	-find edges that form the shadow
	-extrude them with respect to light position to INF_DIST
	-draw a quad for each edge (make sure its vertices are CCW, there are two possible orderings only btw :D )
	(DO NOT USE ANY OPENGL COMMANDS OTHER THAN glVertex, glBegin and glEnd. DO OTHER STUFF LIKE BLENDING EXTERNALLY)
    */
}


//given an object and a light position, calculates shadow volumes
void  MyObject::renderShadow(float *lp){
    /*
	ENTER CODE HERE (for problem 3)
	set stencil buffer values by rendering the shadow quads twice.
	You can (should) use castShadow in this function.
    */
}

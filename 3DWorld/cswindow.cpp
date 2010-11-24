#include "cswindow.h"
#include "iostream"

using namespace std;

typedef struct {         // vertex coordinates - 3d and texture
    GLfloat x, y, z;     // 3d coords.
    GLfloat u, v;        // texture coords.
} VERTEX;

typedef struct {         // triangle
    VERTEX vertex[3];    // 3 vertices array
} TRIANGLE;

typedef struct {         // sector of a 3d environment
    int numtriangles;    // number of triangles in the sector
    TRIANGLE* triangle;  // pointer to array of triangles.
} SECTOR;

SECTOR sector1;

/* Image type - contains height, width, and data */
typedef struct {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
} Image;


CSWindow::CSWindow(QWidget *parent) : QGLWidget(parent)
{
    setFormat(QGLFormat(QGL::DoubleBuffer|QGL::DepthBuffer|QGL::StencilBuffer));
    setGeometry(100,100,600,600);
    _title="3D World Test";
    setWindowTitle(tr(_title));
    setFocusPolicy(Qt::StrongFocus);
}

void CSWindow::initializeGL()
{
    light = 0;           // lighting on/off
    blend = 0;        // blending on/off

    walkbias = 0;
    walkbiasangle = 0;

    lookupdown = 0.0;

    camx = 0;
    camy = 0;
    camz = 0; // camera location.

    z = 0.0f;                       // depth into the screen.

    filter = 0;       // texture filtering method to use (nearest, linear, linear + mipmaps)

    GLfloat LightAmbient[]  = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat LightDiffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat LightPosition[] = {0.0f, 0.0f, 2.0f, 1.0f};

    /* load our world from disk */
    SetupWorld();

    LoadGLTextures();                           // load the textures.
    glEnable(GL_TEXTURE_2D);                    // Enable texture mapping.

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);          // Set the blending function for translucency (note off at init time)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	// This Will Clear The Background Color To Black
    glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);                       // type of depth test to do.
    glEnable(GL_DEPTH_TEST);                    // enables depth testing.
    glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();				// Reset The Projection Matrix

    //gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Really Nice Perspective Calculations
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glMatrixMode(GL_MODELVIEW);

    // set up lights.
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
    glEnable(GL_LIGHT1);

}

void CSWindow::resizeGL(int width, int height)
{
    if (height==0)								// Prevent A Divide By Zero By
    {
        height=1;							// Making Height Equal One
    }

    glViewport(0,0,width,height);						// Reset The Current Viewport

    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glLoadIdentity();							// Reset The Projection Matrix

    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

    glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix
    glLoadIdentity();							// Reset The Modelview Matrix
}

// Load Bitmaps And Convert To Textures
void CSWindow::LoadGLTextures()
{
    // Load Texture
    QImage image1("mud.bmp",0);

    // Create Textures
    glGenTextures(3, &texture[0]);

    // nearest filtered texture
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST); // scale cheaply when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST); // scale cheaply when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1.width(), image1.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image1.data_ptr());

    // linear filtered texture
    glBindTexture(GL_TEXTURE_2D, texture[1]);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1.width(), image1.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image1.data_ptr());

    // mipmapped texture
    glBindTexture(GL_TEXTURE_2D, texture[2]);   // 2d texture (x and y size)
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // scale mipmap when image smalled than texture
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image1.width(), image1.height(), GL_RGB, GL_UNSIGNED_BYTE, image1.data_ptr());
};

// loads the world from a text file.
void CSWindow::SetupWorld()
{
    float x, y, z, u, v;
    int vert;
    int numtriangles;
    FILE *filein;        // file to load the world from
    char oneline[255];

    filein = fopen("world.txt", "rt");

    readstr(filein, oneline);
    sscanf(oneline, "NUMPOLLIES %d\n", &numtriangles);

    sector1.numtriangles = numtriangles;
    sector1.triangle = (TRIANGLE *) malloc(sizeof(TRIANGLE)*numtriangles);

    for (loop = 0; loop < numtriangles; loop++) {
        for (vert = 0; vert < 3; vert++) {
            readstr(filein,oneline);
            sscanf(oneline, "%f %f %f %f %f", &x, &y, &z, &u, &v);
            sector1.triangle[loop].vertex[vert].x = x;
            sector1.triangle[loop].vertex[vert].y = y;
            sector1.triangle[loop].vertex[vert].z = z;
            sector1.triangle[loop].vertex[vert].u = u;
            sector1.triangle[loop].vertex[vert].v = v;
        }
    }

    fclose(filein);
    return;
}

// helper for SetupWorld.  reads a file into a string until a nonblank, non-comment line
// is found ("/" at the start indicating a comment); assumes lines < 255 characters long.
void CSWindow::readstr(FILE *f, char *string)
{
    do {
        fgets(string, 255, f); // read the line
    } while ((string[0] == '/') || (string[0] == '\n'));
    return;
}

//This function indicates which problem (setup, reflection or volume shadow) runs
//you can edit this function (you can do the assignment without changing this one)
void CSWindow::paintGL()
{
    GLfloat x_m, y_m, z_m, u_m, v_m;
    GLfloat xtrans, ztrans, ytrans;
    GLfloat sceneroty;
    int numtriangles;

    // calculate translations and rotations.
    xtrans = -xpos;
    ztrans = -zpos;
    ytrans = -walkbias-0.25f;
    sceneroty = 360.0f - yrot;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
    glLoadIdentity();

    glRotatef(lookupdown, 1.0f, 0, 0);
    glRotatef(sceneroty, 0, 1.0f, 0);

    glTranslatef(xtrans, ytrans, ztrans);

    glBindTexture(GL_TEXTURE_2D, texture[filter]);    // pick the texture.

    numtriangles = sector1.numtriangles;

    for (loop=0; loop<numtriangles; loop++) {        // loop through all the triangles
        glBegin(GL_TRIANGLES);
        glNormal3f( 0.0f, 0.0f, 1.0f);

        x_m = sector1.triangle[loop].vertex[0].x;
        y_m = sector1.triangle[loop].vertex[0].y;
        z_m = sector1.triangle[loop].vertex[0].z;
        u_m = sector1.triangle[loop].vertex[0].u;
        v_m = sector1.triangle[loop].vertex[0].v;
        glTexCoord2f(u_m,v_m);
        glVertex3f(x_m,y_m,z_m);

        x_m = sector1.triangle[loop].vertex[1].x;
        y_m = sector1.triangle[loop].vertex[1].y;
        z_m = sector1.triangle[loop].vertex[1].z;
        u_m = sector1.triangle[loop].vertex[1].u;
        v_m = sector1.triangle[loop].vertex[1].v;
        glTexCoord2f(u_m,v_m);
        glVertex3f(x_m,y_m,z_m);

        x_m = sector1.triangle[loop].vertex[2].x;
        y_m = sector1.triangle[loop].vertex[2].y;
        z_m = sector1.triangle[loop].vertex[2].z;
        u_m = sector1.triangle[loop].vertex[2].u;
        v_m = sector1.triangle[loop].vertex[2].v;
        glTexCoord2f(u_m,v_m);
        glVertex3f(x_m,y_m,z_m);

        glEnd();
    }

    // since this is double buffered, swap the buffers to display what just got drawn.
    swapBuffers();
    //glutSwapBuffers();
    glFlush();
}

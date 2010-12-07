#include "originwindow.h"

OriginWindow::OriginWindow(QComboBox* transformationSelector, QTableWidget *transformationTable, QComboBox* rotationAxisSelector, QSpinBox* rotationAnglePicker, QWidget *parent) : QGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);

    // Setup widget pointers
    this->transformationSelector = transformationSelector;
    this->transformationTable = transformationTable;

    this->rotationAxisSelector = rotationAxisSelector;
    this->rotationAnglePicker = rotationAnglePicker;

    // enable mouse cursor tracking
    //setMouseTracking(true);

    // Set minimum size for the window
    this->setFixedSize(MIN_WIDTH, MIN_HEIGHT);

    //Initialization
    walkbias = 0;
    walkbiasangle = 0;
    lookupdown = 0;
    lastPos = QPoint();
    keysPressed = KeyState();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerLoop()));
    timer->start(TIMER_LOOP_SPEED);
}

void OriginWindow::initializeGL()
{
    // Generate texture names
    this->textureCount = 0;
    glGenTextures( MAX_TEXTURES, &texture[0] );

    // Load in the HUD textures
    QString crosshairTexture = "../images/crosshair.bmp";
    this->readTexture(crosshairTexture);

    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void OriginWindow::resizeGL( int width, int height )
{
    height = height?height:1;

    glViewport(0,0,(GLint)width,(GLint)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.0001f,100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OriginWindow::paintGL()
{
    //const QCursor & cursor = QCursor(Qt::BlankCursor);
    //grabMouse ( cursor );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    GLfloat xtrans = -xpos;
    GLfloat ztrans = -zpos;
    GLfloat ytrans = -ypos; //-walkbias-0.25f
    GLfloat sceneroty = yrot;

    glRotatef(lookupdown,1.0f,0,0);
    glRotatef(sceneroty,0,1.0f,0);

    glTranslatef(xtrans, ytrans, ztrans);

    // Draw the objects stored in scene.
    for(int i=0; i<scene.objcount; i++)
    {
        // Get the object to draw.
        MyObject& object = scene.obj[i];

        // Get the position to draw it at.
        MyPoint& position = object.position;

        // Make a copy of the current matrix on top of the stack.
        glPushMatrix();

        // Translate the origin to the point's position?
        glTranslatef(position.x, position.y, position.z);

        // Draw the object.
        object.draw();

        // Discard matrix changes.
        glPopMatrix();
    }

    // Draw the props stored in scene.
    for(int i=0; i<scene.propcount; i++)
    {
        // Skip the selectedProp
        if (i == scene.selectedProp)
            continue;

        // Get the object to draw.
        MyObject& object = scene.prop[i];

        // Get the position to draw it at.
        MyPoint& position = object.position;

        // Make a copy of the current matrix on top of the stack.
        glPushMatrix();

        // Translate the origin to the point's position.
        glTranslatef(position.x, position.y, position.z);

        // Draw the object.
        object.draw();

        // Discard matrix changes.
        glPopMatrix();
    }

    //draw the selected prop with blending
    ////////////////////////////////////////////////
    if (scene.selectedProp != NO_SELECTION)
    {
        // Get the object to draw.
        MyObject& object = scene.prop[scene.selectedProp];

        // Get the position to draw it at.
        MyPoint& position = object.position;

        // Make a copy of the current matrix on top of the stack.
        glPushMatrix();

        // Translate the origin to the point's position.
        glTranslatef(position.x, position.y, position.z);

        // Enable blending on the selectedProp
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);

        // Draw the object.
        object.draw();

        // Disable blending effects
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        // Discard matrix changes.
        glPopMatrix();
    }
    ////////////////////////////////////////
    //Draw the HUD
    drawHUD();
}

void OriginWindow::drawHUD()
{
    //Draw Crosshair
    drawCrosshair();

    //Draw 3D Axis
    drawAxis();
}

void OriginWindow::drawAxis()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,this->width(),0, this->height());
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    double l = 32;
    double cx = 48;
    double cy = 48;
    double xx, xy, yx, yy , zx, zy;
    float fvViewMatrix[ 16 ];
    glGetFloatv( GL_MODELVIEW_MATRIX, fvViewMatrix );
    glLoadIdentity();
    xx = l * fvViewMatrix[0];
    xy = l * fvViewMatrix[1];
    yx = -l * fvViewMatrix[4];
    yy = l * fvViewMatrix[5];
    zx = l * fvViewMatrix[8];
    zy = l * fvViewMatrix[9];
    glLineWidth(2);

    // Save the current colour for later.
    glPushAttrib(GL_CURRENT_BIT);

    // Disable textures temporarily.
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_LINES);
    // Red.
    glColor3f(1,0,0);
    glVertex2f(cx, cy);
    glVertex2f(cx + xx, cy + xy);
    // Green.
    glColor3f(0,1,0);
    glVertex2f(cx, cy);
    glVertex2f(cx + yx, cy + yy);
    // Blue
    glColor3f(0,0,1);
    glVertex2f(cx, cy);
    glVertex2f(cx + zx, cy + zy);
    glEnd();

    // Re-enable textures.
    glEnable(GL_TEXTURE_2D);

    // Restore the old colour.
    glPopAttrib();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void OriginWindow::drawCrosshair()
{
    /*
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,this->width(),0, this->height());
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    double l = 32;
    double cx = this->width()/2;
    double cy = this->height()/2;
    glLoadIdentity();

    // Save the current colour for later.
    glPushAttrib(GL_CURRENT_BIT);

    // Disable textures temporarily.
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, texture[0]);                           // Select The Correct Texture
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f,0.0f); glVertex2f(cx,cy);                    // Bottom Left
    glTexCoord2f(1.0f,0.0f); glVertex2f(cx+l,cy);                    // Bottom Right
    glTexCoord2f(1.0f,1.0f); glVertex2f(cx+l, cy+l);                    // Top Right
    glTexCoord2f(0.0f,1.0f); glVertex2f(cx, cy+l);                    // Top Left
    glDisable(GL_BLEND);

    // Disable textures temporarily.
    glDisable(GL_TEXTURE_2D);

    // Restore the old colour.
    glPopAttrib();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    */

    // Crosshair (In Ortho View)					// Store The Projection Matrix
    glPushMatrix();
    glLoadIdentity();							// Reset The Projection Matrix
    glOrtho(0,this->width(),0,this->height(),-1,1);			// Set Up An Ortho Screen
    glMatrixMode(GL_MODELVIEW);                                         // Select The Modelview Matrix
    glTranslatef(this->width()/2, this->height()/2, 0.1);               // Move To The Middle of the screen

    // Draw The Crosshair
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, texture[0]);                           // Select The Correct Texture
    glBegin(GL_QUADS);							// Start Drawing A Quad
    glTexCoord2f(0.0f,0.0f); glVertex3f(-1,-1,0.0f);                    // Bottom Left
    glTexCoord2f(1.0f,0.0f); glVertex3f( 1,-1,0.0f);                    // Bottom Right
    glTexCoord2f(1.0f,1.0f); glVertex3f( 1, 1,0.0f);                    // Top Right
    glTexCoord2f(0.0f,1.0f); glVertex3f(-1, 1,0.0f);                    // Top Left
    glEnd();
    // Done Drawing Quad
    glDisable(GL_BLEND);
    glPopMatrix();
}

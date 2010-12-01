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
    filter = 0;
    lastPos = QPoint();
    keysPressed = KeyState();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerLoop()));
    timer->start(18);
}

void OriginWindow::initializeGL()
{
    // Generate texture names
    this->textureCount = 0;
    glGenTextures( MAX_TEXTURES, &texture[0] );

    // Load in the HUD textures
    QString crosshairTexture = "../images/crosshair.bmp";
    this->readTexture(crosshairTexture);
     QString axisTexture = "../images/axis.bmp";
    this->readTexture(axisTexture);

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

    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

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
    GLfloat ytrans = -walkbias-0.25f;
    GLfloat sceneroty = yrot;

    glRotatef(lookupdown,1.0f,0,0);
    glRotatef(sceneroty,0,1.0f,0);

    glTranslatef(xtrans, ytrans, ztrans);
    glBindTexture(GL_TEXTURE_2D, texture[filter]);

    // Draw the objects stored in scene.
    for(int i=0; i<scene.objcount; i++)
    {
        // Get the object to draw.
        MyObject& object = scene.obj[i];

        // Get the position to draw it at.
        MyPoint& position = object.position;

        // Get the rotation to rotate the object by.
        MyPoint& rotation = object.rotation;

        // Make a copy of the current matrix on top of the stack.
        glPushMatrix();

        // Translate the origin to the point's position?
        glTranslatef(position.x, position.y, position.z);

        // Rotate the object.
        glRotatef(-rotation.x, 1.0, 0.0, 0.0);
        glRotatef(-rotation.y, 0.0, 1.0, 0.0);
        glRotatef(-rotation.z, 0.0, 0.0, 1.0);

        // Draw the object.
        object.draw();

        // Discard matrix changes.
        glPopMatrix();
    }

    // Draw the props stored in scene.
    for(int i=0; i<scene.propcount; i++)
    {
        // Get the object to draw.
        MyObject& object = scene.prop[i];

        // Get the position to draw it at.
        MyPoint& position = object.position;

        // Get the rotation to rotate the object by.
        MyPoint& rotation = object.rotation;

        // Make a copy of the current matrix on top of the stack.
        glPushMatrix();

        // Translate the origin to the point's position?
        glTranslatef(position.x, position.y, position.z);

        // Rotate the object
        glRotatef(-rotation.x, 1.0, 0.0, 0.0);
        glRotatef(-rotation.y, 0.0, 1.0, 0.0);
        glRotatef(-rotation.z, 0.0, 0.0, 1.0);

        // Draw the object.
        object.draw();

        // Discard matrix changes.
        glPopMatrix();
    }

    //Draw the HUD
    drawHUD();
}

void OriginWindow::drawHUD()
{
    //Draw 3D Axis
    drawAxis();

    //Draw Crosshair
    drawCrosshair();
}

void OriginWindow::drawAxis()
{
    //1. Change viewport to cover only corner of a screen
    //2. Reinitialize modelview and projection matrices - apply all rotations and perspective you need (do not apply any scaling or translation).
    //3. Draw your axes around (0,0,0) point
    //4. Restore viewport and matrices

    //attempt1
    /*
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0,this->width(),0, this->height());
    glTranslatef(0., 0., 0.);
    glMatrixMode(GL_MODELVIEW);
    double l = 32;
    double cx = 16;
    double cy = 16;
    double xx, xy, yx, yy , zx, zy;
    float fvViewMatrix[ 16 ];
    glGetFloatv( GL_MODELVIEW_MATRIX, fvViewMatrix );
    glLoadIdentity();
    xx = l * fvViewMatrix[0];
    xy = l * fvViewMatrix[1];
    yx = l * fvViewMatrix[4];
    yy = l * fvViewMatrix[5];
    zx = l * fvViewMatrix[8];
    zy = l * fvViewMatrix[9];
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(cx, cy);
    glVertex2f(cx + xx, cy + xy);
    glVertex2f(cx, cy);
    glVertex2f(cx + yx, cy + yy);
    glVertex2f(cx, cy);
    glVertex2f(cx + zx, cy + zy);
    glEnd();
    glPopMatrix();
    */

    //attempt 2
    /*
    glPushMatrix ();

          glTranslatef (-2.4, -1.5, -5);
          glRotatef(lookupdown,1.0f,0,0);
          glRotatef(yrot,0,1.0f,0);
          glScalef(0.25, 0.25, 0.25);

          glLineWidth (2.0);

          glBegin (GL_LINES);
             glColor3f (1,0,0);  glVertex3f (0,0,0);  glVertex3f (1,0,0);    // X axis is red.
             glColor3f (0,1,0);  glVertex3f (0,0,0);  glVertex3f (0,1,0);    // Y axis is green.
             glColor3f (0,0,1);  glVertex3f (0,0,0);  glVertex3f (0,0,1);    // z axis is blue.
          glEnd();

      glPopMatrix ();
      */

    //attempt 3
    // Draw Axis
    //glPushMatrix();
    //glLoadIdentity();							// Reset The Projection Matrix
    //gluOrtho2D(0,this->width(),0,this->height());			// Set Up An Ortho Screen
    //glMatrixMode(GL_MODELVIEW);                                         // Select The Modelview Matrix
    //glTranslatef(this->width()/2-20, this->height()/2-10, 0.1);     // Move To The corner of the screen

    //GLfloat x_m, y_m, z_m, u_m, v_m;
    //GLfloat xtrans = -xpos;
    //GLfloat ztrans = -zpos;
    //GLfloat ytrans = -walkbias-0.25f;
    //GLfloat sceneroty = yrot;

    //glTranslatef(-xtrans, -ytrans, -ztrans);

    //glRotatef(lookupdown,1.0f,0,0);
    //glRotatef(sceneroty,0,1.0f,0);

    //glTranslatef(xtrans, ytrans, ztrans);
    //glBegin(GL_LINES);
    // Start Drawing A Quad
    //glColor3f(1, 0, 0);
    //glVertex3f(-2,-2,0.0f);                    // Bottom Left
    //glVertex3f( 2,-2,0.0f);                    // Bottom Right
    //glVertex3f( 2, 2,0.0f);                    // Top Right
    //glVertex3f(-2, 2,0.0f);                    // Top Left
    //glEnd();
    // Done Drawing Quad
    //glPopMatrix();

    // Draw Background
    /*
    glPushMatrix();
    glLoadIdentity();							// Reset The Projection Matrix
    gluOrtho2D(0,this->width(),0,this->height());			// Set Up An Ortho Screen
    glMatrixMode(GL_MODELVIEW);                                         // Select The Modelview Matrix
    glTranslatef(this->width()/2 - 20.1, this->height()/2 - 11, 0.1);     // Move To The corner of the screen
    glBindTexture(GL_TEXTURE_2D, texture[4]);                           // Select The Correct Texture
    glBegin(GL_QUADS);							// Start Drawing A Quad
    glTexCoord2f(0.0f,0.0f); glVertex3f(-2,-2,0.0f);                    // Bottom Left
    glTexCoord2f(1.0f,0.0f); glVertex3f( 2,-2,0.0f);                    // Bottom Right
    glTexCoord2f(1.0f,1.0f); glVertex3f( 2, 2,0.0f);                    // Top Right
    glTexCoord2f(0.0f,1.0f); glVertex3f(-2, 2,0.0f);                    // Top Left
    glEnd();
    // Done Drawing Quad
    glPopMatrix();
    */
}

void OriginWindow::drawCrosshair()
{
    // Crosshair (In Ortho View)					// Store The Projection Matrix
    glPushMatrix();
    glLoadIdentity();							// Reset The Projection Matrix
    glOrtho(0,this->width(),0,this->height(),-1,1);			// Set Up An Ortho Screen
    glMatrixMode(GL_MODELVIEW);                                         // Select The Modelview Matrix
    glTranslatef(this->width()/2, this->height()/2, 0.1);               // Move To The Middle of the screen

    // Draw The Crosshair
    glEnable(GL_BLEND);
    //glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, texture[0]);                           // Select The Correct Texture
    glBegin(GL_QUADS);							// Start Drawing A Quad
    glTexCoord2f(0.0f,0.0f); glVertex3f(-1,-1,0.0f);                    // Bottom Left
    glTexCoord2f(1.0f,0.0f); glVertex3f( 1,-1,0.0f);                    // Bottom Right
    glTexCoord2f(1.0f,1.0f); glVertex3f( 1, 1,0.0f);                    // Top Right
    glTexCoord2f(0.0f,1.0f); glVertex3f(-1, 1,0.0f);                    // Top Left
    glEnd();
    // Done Drawing Quad
    glDisable(GL_BLEND);
    //glEnable(GL_DEPTH_TEST);
    glPopMatrix();
}

void OriginWindow::updatePlayerPosition()
{
    // Handle KeysPressed
    if (keysPressed.w)
    {
        xpos += (float)sin(heading*piover180) * 0.03f;
        zpos -= (float)cos(heading*piover180) * 0.03f;
        if (walkbiasangle >= 359.0f)
        {
            walkbiasangle = 0.0f;
        }
        else
        {
            walkbiasangle+= 10;
        }
        walkbias = (float)sin(walkbiasangle * piover180)/50.0f;
    }
    if (keysPressed.s)
    {
        xpos -= (float)sin(heading*piover180) * 0.03f;
        zpos += (float)cos(heading*piover180) * 0.03f;
        if (walkbiasangle <= 1.0f)
        {
            walkbiasangle = 359.0f;
        }
        else
        {
            walkbiasangle-= 10;
        }
        walkbias = (float)sin(walkbiasangle * piover180)/50.0f;
    }
    if (keysPressed.a)
    {
        xpos -= (float)cos(heading*piover180) * 0.03f;
        zpos -= (float)sin(heading*piover180) * 0.03f;

    }
    if (keysPressed.d)
    {
        xpos += (float)cos(heading*piover180) * 0.03f;
        zpos += (float)sin(heading*piover180) * 0.03f;
    }
}

void OriginWindow::updatePropsPosition()
{
    for(int i=0; i<scene.propcount; i++)
    {
        // Create a reference to the prop.
        MyObject& o = scene.prop[i];
        MyPoint& goal = o.goalPosition;
        MyPoint& curPosition = o.position;

        // If the prop is near the goal, do nothing
        if (curPosition.isNear(goal))
            continue;

        // Otherwise, get the direction to move
        MyPoint diff = goal.minus(curPosition);
        diff.normalize();

        // Move towards the goal
        curPosition.x += diff.x * PROP_TRANSFORM_STEP;
        curPosition.y += diff.y * PROP_TRANSFORM_STEP;
        curPosition.z += diff.z * PROP_TRANSFORM_STEP;
    }
}

void OriginWindow::updatePropsRotation()
{
    for(int i=0; i<scene.propcount; i++)
    {
        // Create a reference to the prop.
        MyObject& o = scene.prop[i];
        MyPoint& goal = o.goalRotation;
        MyPoint& curRotation = o.rotation;

        // If the prop is near the goal, do nothing
        if (curRotation.equals(goal))
            continue;

        // Otherwise, get the direction to move
        MyPoint diff = goal.minus(curRotation);
        diff.normalize();

        // Move towards the goal
        curRotation.x += diff.x * PROP_ROTATE_STEP;
        curRotation.y += diff.y * PROP_ROTATE_STEP;
        curRotation.z += diff.z * PROP_ROTATE_STEP;
    }
}

// Main program loop
void OriginWindow::timerLoop()
{
    //Update the player's position
    updatePlayerPosition();

    //Update the props' position
    updatePropsPosition();

    //Update the props' rotation
    updatePropsRotation();

    //UpdateGL
    updateGL();
}

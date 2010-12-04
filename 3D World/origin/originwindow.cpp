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

    // Draw the objects stored in scene.
    for(int i=0; i<scene.objcount; i++)
    {
        // Get the object to draw.
        MyObject& object = scene.obj[i];

        // Get the position to draw it at.
        //MyPoint& position = object.position;

        // Get the rotation to rotate the object by.
        //MyPoint& rotation = object.rotation;

        // Get the object scale
        //MyPoint& scale = object.scale;

        // Make a copy of the current matrix on top of the stack.
        glPushMatrix();

        // Translate the origin to the point's position?
        //glTranslatef(position.x, position.y, position.z);

        // Rotate the object.
        //glRotatef(-rotation.x, 1.0, 0.0, 0.0);
        //glRotatef(-rotation.y, 0.0, 1.0, 0.0);
        //glRotatef(-rotation.z, 0.0, 0.0, 1.0);

        // Scale the object.
        //glScalef(scale.x,scale.y,scale.z);

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

        // Get the scale.
        MyPoint& scale = object.scale;

        // Make a copy of the current matrix on top of the stack.
        glPushMatrix();

        // Translate the origin to the point's position.
        glTranslatef(position.x, position.y, position.z);

        // Rotate the object.
        glRotatef(-rotation.x, 1.0, 0.0, 0.0);
        glRotatef(-rotation.y, 0.0, 1.0, 0.0);
        glRotatef(-rotation.z, 0.0, 0.0, 1.0);

        // Scale the object.
        glScalef(scale.x,scale.y,scale.z);

        // Translate the origin back
        glTranslatef(-position.x, -position.y, -position.z);

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
    //Draw Crosshair
    drawCrosshair();

    //Draw 3D Axis
    drawAxis();
}

void OriginWindow::drawAxis()
{

    /*
    // Draw Background
    glPushMatrix();
    glLoadIdentity();							// Reset The Projection Matrix
    gluOrtho2D(0,this->width(),0,this->height());			// Set Up An Ortho Screen
    glMatrixMode(GL_MODELVIEW);                                         // Select The Modelview Matrix
    glTranslatef(this->width()/2 - 20.1, this->height()/2 - 11, 0.1);     // Move To The corner of the screen
    glBindTexture(GL_TEXTURE_2D, texture[1]);                           // Select The Correct Texture
    glBegin(GL_QUADS);							// Start Drawing A Quad
    glTexCoord2f(0.0f,0.0f); glVertex3f(-2,-2,0.0f);                    // Bottom Left
    glTexCoord2f(1.0f,0.0f); glVertex3f( 2,-2,0.0f);                    // Bottom Right
    glTexCoord2f(1.0f,1.0f); glVertex3f( 2, 2,0.0f);                    // Top Right
    glTexCoord2f(0.0f,1.0f); glVertex3f(-2, 2,0.0f);                    // Top Left
    glEnd();
    // Done Drawing Quad
    glPopMatrix();
    */

    //1. Change viewport to cover only corner of a screen
    //2. Reinitialize modelview and projection matrices - apply all rotations and perspective you need (do not apply any scaling or translation).
    //3. Draw your axes around (0,0,0) point
    //4. Restore viewport and matrices

    //attempt1


    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,this->width(),0, this->height());
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    double l = 24;
    double cx = 32;
    double cy = 32;
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
    // Keep track of the old x and z positions for collision detection purposes.
    GLfloat oldxpos = xpos;
    GLfloat oldzpos = zpos;

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

    // Check for collisions with each object in the scene.

    // Convert the player's previous position into a MyPoint.
    MyPoint linePoint0;
    linePoint0.x = oldxpos;
    linePoint0.y = walkbias+0.25f;
    linePoint0.z = oldzpos;

    // Convert the player's new position into a MyPoint.
    MyPoint linePoint1;
    linePoint1.x = xpos;
    linePoint1.y = walkbias+0.25f;
    linePoint1.z = zpos;

    // Get a unit vector along the line.
    MyPoint lineUnitVector = linePoint1.minus(linePoint0);
    lineUnitVector.normalize();

    for(int i=0; i<scene.objcount; i++)
    {
        // Get the Object.
        MyObject& object = scene.obj[i];

        // Check for collisions with each plane of the object.
        for(int j=0; j<object.nPlanes; j++)
        {
            // Get the plane.
            MyPlane& plane = object.planes[j];

            // Get 3 points on the plane.
            MyPoint p0 = object.points[plane.pids[0]];
            MyPoint p1 = object.points[plane.pids[1]];
            MyPoint p2 = object.points[plane.pids[2]];

            // Get the normal of the plane.
            MyPoint planeNormal = (p1.minus(p0)).cross(p2.minus(p0));
            planeNormal.normalize();

            // Solve for d, the distance along the line that the intersection occurs (if it occurs at all).
            double numerator = (p0.minus(linePoint0)).dot(planeNormal);
            double denominator = lineUnitVector.dot(planeNormal);

            // If the numerator is not zero, and the denominator is zero,
            if( !(-zeroCutoff < numerator && numerator < zeroCutoff) && (-zeroCutoff < denominator && denominator < zeroCutoff) )
            {
                // There is no collision, so skip to the next iteration of the loop.
                continue;
            }
            // If the numerator and denominator are both zero,
            if( (-zeroCutoff < denominator && denominator < zeroCutoff) && (-zeroCutoff < numerator && numerator < zeroCutoff) )
            {
                // There's infinite collisions, because the line is contained in the plane.
                // In this case we should move the player in the direction of the normal: away from the plane.
                xpos = xpos + planeNormal.x;
                zpos = zpos + planeNormal.z;
                break;
            }
            // Neither the numerator nor denominator are zero, so there IS an intersection point d.
            double d = numerator/denominator;

            // However, we still need to check that d is between the line's endpoints and within the plane's boundaries.

            // Check endpoints:
            if(0 < d && linePoint0.length(lineUnitVector.times(d)) < linePoint0.length(linePoint1) ){
                // Check boundaries:
                if(true)
                {
                    // There is a collision, so reset the position to the last position.
                    xpos = oldxpos;
                    zpos = oldzpos;
                    return;
                }
            }
            // If program execution gets here, there is no collision, so allow the player's position to be updated.
        }
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
        {
            curPosition = goal;
        }

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

        // Rotate towards the goal
        curRotation.x += diff.x * PROP_ROTATE_STEP;
        curRotation.y += diff.y * PROP_ROTATE_STEP;
        curRotation.z += diff.z * PROP_ROTATE_STEP;
    }
}

void OriginWindow::updatePropsScale()
{
    for(int i=0; i<scene.propcount; i++)
    {
        // Create a reference to the prop.
        MyObject& o = scene.prop[i];
        MyPoint& goal = o.goalScale;
        MyPoint& curScale = o.scale;

        // If the prop is near the goal, do nothing
        if (curScale.equals(goal))
            continue;

        // Otherwise, get the direction to move
        MyPoint diff = goal.minus(curScale);
        diff.normalize();

        // Scale towards the goal
        curScale.x += diff.x * PROP_SCALE_STEP;
        curScale.y += diff.y * PROP_SCALE_STEP;
        curScale.z += diff.z * PROP_SCALE_STEP;
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

    //Update the props' scale
    updatePropsScale();

    //UpdateGL
    updateGL();
}

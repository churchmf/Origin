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
        //glTranslatef(position.x, position.y, position.z);

        // Rotate the object.
        glRotatef(-rotation.x, 1.0, 0.0, 0.0);
        glRotatef(-rotation.y, 0.0, 1.0, 0.0);
        glRotatef(-rotation.z, 0.0, 0.0, 1.0);

        // Scale the object.
        glScalef(scale.x,scale.y,scale.z);

        // Translate back
        //glTranslatef(-position.x, -position.y, -position.z);

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

    // Exit early if there's no movement.
    if(xpos == oldxpos && zpos == oldzpos)
    {
        return;
    }

    // Check for collisions with each object in the scene.

    // Convert the player's previous position into a MyPoint.
    MyPoint linePoint0;
    linePoint0.x = oldxpos;
    linePoint0.y = walkbias+0.5f;
    linePoint0.z = oldzpos;

    // Convert the player's new position into a MyPoint.
    MyPoint linePoint1;
    linePoint1.x = xpos;
    linePoint1.y = walkbias+0.5f;
    linePoint1.z = zpos;

//    printf("movement: %f,%f,%f \n", linePoint1.x-linePoint0.x, linePoint1.y-linePoint0.y, linePoint1.z-linePoint0.z);

    for(int i=0; i<scene.objcount; i++)
    {
        // Get the Object.
        MyObject object = scene.obj[i];

        // Check for collisions with each triangular plane of the object.
        for(int j=0; j<object.nPlanes; j++)
        {
            // Get the triangular plane.
            MyPlane plane = object.planes[j];

            // Get points on the triangular plane.
            MyPoint p1 = object.points[plane.pids[0]];
            MyPoint p2 = object.points[plane.pids[1]];
            MyPoint p3 = object.points[plane.pids[2]];

            // Compute the normal of the triangular plane.
            MyPoint planeNormal = (p3.minus(p2)).cross(p2.minus(p1));
            planeNormal.normalize();

            // If the player is too close to the triangular plane, collide.
//            printf("distance to plane: %d \n", planeNormal.dot(linePoint1.minus(p1)));
            if(fabs(planeNormal.dot(linePoint1.minus(p1))) < 0.01f)
            {
                xpos = oldxpos;
                zpos = oldzpos;
                return;
            }

            // Compute the numerator and denominator for computing d.
            double numerator = planeNormal.dot(p1.minus(linePoint0));
            double denominator = planeNormal.dot(linePoint1.minus(linePoint0));

            // If the denominator is zero, there is no intersection so skip to checking the next plane.
            if(denominator == 0){ continue; }

            // Compute d, the parameter value on the line of the point of intersection.
            double d = numerator / denominator;
//            printf("d value: %d \n",d);

            // Check if d is between the endpoints of the line:
            if(0 <= d && d <= 1 )
            {
                // Compute the point of intersection.
                MyPoint intersectionPoint = linePoint0.plus(linePoint1.times(d));

                // Check that the intersection point is within the boundaries of the triangular plane:
                if(pointInsideTriangle(intersectionPoint, p1, p2, p3))
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

/*
 P is the point of possible intersection.
 The triangle is set up like so:
 b
 | \
 a--C
 */
bool OriginWindow::pointInsideTriangle(MyPoint& p, MyPoint& a, MyPoint& b, MyPoint& c)
{
    // Create the vectors to use for the triangle.
    MyPoint v0 = c.minus(a);
    MyPoint v1 = b.minus(a);
    MyPoint v2 = p.minus(a);

    // Compute the dot products to use for changing to barycentric coordinates.
    float dotv0v0 = v0.dot(v0);
    float dotv0v1 = v0.dot(v1);
    float dotv0v2 = v0.dot(v2);
    float dotv1v1 = v1.dot(v1);
    float dotv1v2 = v1.dot(v2);

    // Compute barycentric coordinates u and v.
    float u, v;
    float denominator = dotv0v0 * dotv1v1 - dotv0v1 * dotv0v1;

    if(denominator != 0)
    {
        u = (dotv1v1 * dotv0v2 - dotv0v1 * dotv1v2)/denominator;
        v = (dotv0v0 * dotv1v2 - dotv0v1 * dotv0v2)/denominator;
    }else
    {
        printf("Denominator of barycentric coordinate conversion was zero.");
        return true;
    }

    return (0 < u) && (0 < v) && (u + v < 1);
}

bool OriginWindow::checkCollision(MyObject& object1)
{
    bool hasCollision = false;
    unsigned int numFaces = object1.nPlanes;
    // Check for collisions with each object in the scene
    for(int i=0; i<scene.objcount; i++)
    {
        // Get the Object.
        MyObject& object2 = scene.obj[i];

    }
    for (int i=0; i<scene.propcount; i++)
    {
        // Get the Object.
        MyObject& object2 = scene.prop[i];
    }
    return false;
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

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

        // Translate back
        //glTranslatef(-position.x, -position.y, -position.z);

        // Scale the object.
        glScalef(scale.x,scale.y,scale.z);

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

        // Translate back
        //glTranslatef(-position.x, -position.y, -position.z);

        // Scale the object.
        glScalef(scale.x,scale.y,scale.z);

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

void OriginWindow::selectProp()
{
    MyPoint p1;
    p1.x = xpos;
    p1.y = ypos; //walkbias+0.25f;
    p1.z = zpos;

    MyPoint gaze;
    gaze.x = (float)sin(heading*piover180);
    gaze.y = -(float)sin(lookupdown * piover180);
    gaze.z = -(float)cos(heading*piover180);

    MyPoint p2 = p1.plus((gaze.times(INF)));

    //printf("p1: %f,%f,%f \n", p1.x,p1.y,p1.z);
    //printf("gaze: %f,%f,%f \n", gaze.x,gaze.y,gaze.z);
    //printf("p2: %f,%f,%f \n", p2.x,p2.y,p2.z);

    int closestIndex = NO_SELECTION;
    float closestDist = INF;
    for(int i=0; i<scene.propcount; i++)
    {
        // Create a reference to the prop.
        MyObject& o = scene.prop[i];

        for (unsigned int j=0; j <o.nPlanes; j++)
        {
            //calculate the plane normal
            MyPlane& plane = o.planes[j];
            MyPoint A = o.points[plane.pids[0]].plus(o.position);
            MyPoint B = o.points[plane.pids[1]].plus(o.position);
            MyPoint C = o.points[plane.pids[2]].plus(o.position);

            // cast a ray and collide with any props
            MyPoint t;
            if (CheckLineTri(A,B,C,p1,p2,t))
            {
                //printf("select! %f,%f,%f\n",t.x,t.y,t.z);

                //get the closest prop
                float dist = t.length(p1);
                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestIndex = i;
                }
            }
        }
        scene.selectedProp = closestIndex;
    }
}

bool OriginWindow::checkCollisionWithAll(QList<MyPoint> before, MyPoint& delta)
{
    // Assume the same number of points before and after the move
    int numPoints = before.size();

    // Compute the new position for each point
    QList<MyPoint> after;
    for (int i=0;i < numPoints; i++)
    {
        MyPoint afterTransformation;
        afterTransformation.x = before.at(i).x + delta.x;
        afterTransformation.y = before.at(i).y + delta.y;
        afterTransformation.z = before.at(i).z + delta.z;
        after.append(afterTransformation);
    }

    // Check if there is a collision between any of the points and any objects
    for (int i=0; i < numPoints; i++)
    {
        MyPoint linePoint0 = before.at(i);
        MyPoint linePoint1 = after.at(i);

        // Check for collisions with each prop in the scene.
        for(int j=0; j<scene.propcount; j++)
        {
            MyObject& prop = scene.prop[j];

            // Check for collisions with each triangular plane of the prop.
            for(unsigned int k=0; k<prop.nPlanes; k++)
            {
                // Get the triangular plane.
                MyPlane plane = prop.planes[k];

                // Get the object's position.
                MyPoint propPos = prop.position;

                // Get points on the triangular plane.
                MyPoint p1 = prop.points[plane.pids[0]].plus(propPos);
                MyPoint p2 = prop.points[plane.pids[1]].plus(propPos);
                MyPoint p3 = prop.points[plane.pids[2]].plus(propPos);

                // Compute the point of intersection.
                MyPoint intersectionPoint;

                // Check that the intersection point is within the triangular plane:
                if(CheckLineTri(p1, p2, p3, linePoint0, linePoint1, intersectionPoint))
                {
                    return true;
                }
            }
        }

        // Check for collisions with each object in the scene.
        for(int j=0; j<scene.objcount; j++)
        {
            // Get the Object.
            MyObject& sceneObject = scene.obj[j];

            // Check for collisions with each triangular plane of the object.
            for(unsigned int k=0; k<sceneObject.nPlanes; k++)
            {
                // Get the triangular plane.
                MyPlane plane = sceneObject.planes[k];

                // Get the sceneObject's position.
                MyPoint objectPos = sceneObject.position;

                // Get points on the triangular plane.
                MyPoint p1 = sceneObject.points[plane.pids[0]].plus(objectPos);
                MyPoint p2 = sceneObject.points[plane.pids[1]].plus(objectPos);
                MyPoint p3 = sceneObject.points[plane.pids[2]].plus(objectPos);

                // Compute the point of intersection.
                MyPoint intersectionPoint;

                // Check that the intersection point is within the boundaries of the triangular plane:
                if(CheckLineTri(p1, p2, p3, linePoint0, linePoint1, intersectionPoint))
                {
                    return true;
                }
            }
        }
    }
    return false;
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
    ypos = walkbias+0.25f;
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
    if (keysPressed.space)
    {
        //ypos += JUMP_STEP;
    }

    // Exit early if there's no movement.
    if(xpos == oldxpos && zpos == oldzpos)
    {
        return;
    }

    // Convert the player's previous position into a MyPoint.
    MyPoint linePoint0;
    linePoint0.x = oldxpos;
    linePoint0.y = 0.25f;
    linePoint0.z = oldzpos;

    // Convert the player's new position into a MyPoint.
    MyPoint linePoint1;
    linePoint1.x = xpos;
    linePoint1.y = 0.25f;
    linePoint1.z = zpos;

    QList<MyPoint> cameraPos;
    cameraPos.append(linePoint0);
    MyPoint delta = linePoint1.minus(linePoint0);

    if (checkCollisionWithAll(cameraPos,delta))
    {
        // There is a collision, so reset the position to the last position.
        xpos = oldxpos;
        zpos = oldzpos;
        return;
    }
}

bool OriginWindow::CheckLineTri(MyPoint& TP1, MyPoint& TP2, MyPoint& TP3, MyPoint& LP1, MyPoint& LP2, MyPoint& HitPos)
{
    MyPoint normal, IntersectPos;

    // Find Triangle Normal
    normal = TP2.minus(TP1).cross(TP3.minus(TP1));
    normal.normalize(); // not really needed

    // Find distance from LP1 and LP2 to the plane defined by the triangle
    float Dist1 = (LP1.minus(TP1)).dot( normal );
    float Dist2 = (LP2.minus(TP1)).dot( normal );

    if ( (Dist1 * Dist2) >= 0.0f) return false;  // line doesn't cross the triangle.
    if ( Dist1 == Dist2) return false;// line and plane are parallel

    // Find point on the line that intersects with the plane
    IntersectPos = LP1.plus(LP2.minus(LP1).times(-Dist1/(Dist2-Dist1)));

    // Find if the intersection point lies inside the triangle by testing it against all edges
    MyPoint vTest;
    vTest = normal.cross(TP2.minus(TP1));
    if(vTest.dot(IntersectPos.minus(TP1)) < 0.0f) return false;
    vTest = normal.cross(TP3.minus(TP2));
    if(vTest.dot(IntersectPos.minus(TP2)) < 0.0f) return false;
    vTest = normal.cross(TP1.minus(TP3));
    if(vTest.dot(IntersectPos.minus(TP1)) < 0.0f) return false;

    HitPos = IntersectPos;
    return true;
}

void OriginWindow::updatePropsPosition()
{
    for(int i=0; i<scene.propcount; i++)
    {
        // Create a reference to the prop.
        MyObject& o = scene.prop[i];
        MyPoint& goal = o.goalPosition;
        MyPoint& curPosition = o.position;

        // If the prop is not near the goal
        if (!curPosition.equals(goal))
        {
            // Get the direction to move
            MyPoint diff = goal.minus(curPosition);
            diff.normalize();
            MyPoint delta;
            delta.x = diff.x * PROP_TRANSLATE_STEP;
            delta.y = diff.y * PROP_TRANSLATE_STEP;
            delta.z = diff.z * PROP_TRANSLATE_STEP;

            QList<MyPoint> points;
            for (unsigned int j=0; j<o.nPoints; j++)
            {
                points.append(o.points[j].plus(o.position));
            }

            // Check for collision for the move
            if (checkCollisionWithAll(points,delta))
            {
                //if there was a collision, set the goal to the current position, make the object bounce back a bit, and disable transforming
                goal = curPosition;
                o.velocity.x = -delta.x * 0.2f;
                o.velocity.y = -delta.y * 0.2f;
                o.velocity.z = -delta.z * 0.2f;
                o.isTransforming = false;
            }
            else
            {
                // Move towards the goal
                curPosition = curPosition.plus(delta);
            }
        }
        else
        {
            //Update prop's position with physics (velocity)
            applyPhysics(o);
        }
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

        // If the prop is not near the goal
        if (!curRotation.equals(goal))
        {
            // Get the direction to move
            MyPoint diff = goal.minus(curRotation);
            diff.normalize();

            // Rotate towards the goal
            curRotation.x += diff.x * PROP_ROTATE_STEP;
            curRotation.y += diff.y * PROP_ROTATE_STEP;
            curRotation.z += diff.z * PROP_ROTATE_STEP;
        }
        else
        {
            //Update prop's position with physics
            applyPhysics(o);
        }
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

        // If the prop is not near the goal
        if (!curScale.equals(goal))
        {
            // Get the direction to move
            MyPoint diff = goal.minus(curScale);
            diff.normalize();

            // Scale towards the goal
            curScale.x += diff.x * PROP_SCALE_STEP;
            curScale.y += diff.y * PROP_SCALE_STEP;
            curScale.z += diff.z * PROP_SCALE_STEP;
        }
        else
        {
            //Update prop's position with physics
            applyPhysics(o);
        }
    }
}

void OriginWindow::applyPhysics(MyObject& o)
{
    // Don't apply physics to transforming objects
    if (o.isTransforming)
        return;

    // apply gravity
    o.velocity.y += GRAVITY_STEP;

    MyPoint delta;
    delta.x = o.velocity.x;
    delta.y = o.velocity.y;
    delta.z = o.velocity.z;

    QList<MyPoint> points;
    for (unsigned int j=0; j<o.nPoints; j++)
    {
        points.append(o.points[j].plus(o.position));
    }

    // Check for collision
    if (checkCollisionWithAll(points,delta))
    {
        o.velocity.x = -delta.x * 0.2f;
        o.velocity.y = -delta.y * 0.2f;
        o.velocity.z = -delta.z * 0.2f;
    }
    else
    {
        MyPoint& curPosition = o.position;

        // Adjust position based on velocity
        curPosition.x += o.velocity.x;
        curPosition.y += o.velocity.y;
        curPosition.z += o.velocity.z;

        // reduce objects velocity (friction)
        o.velocity.times(0.8f);
    }
}

void OriginWindow::updateIsTransforming()
{
    for(int i=0; i<scene.propcount; i++)
    {
        // Create a reference to the prop.
        MyObject& o = scene.prop[i];

        // Don't worry about non-transforming objects
        if (!o.isTransforming)
            continue;

        // The object is transforming if it is not at it's goal
        o.isTransforming = !(o.goalPosition.equals(o.position) && o.goalRotation.equals(o.rotation) && o.goalScale.equals(o.scale));
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

    //Update isTransforming for all props
    updateIsTransforming();

    //UpdateGL
    updateGL();
}

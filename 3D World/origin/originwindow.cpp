#include "originwindow.h"

OriginWindow::OriginWindow(QComboBox* transformationSelector, QTableWidget *transformationTable, QComboBox* rotationAxisSelector, QSpinBox* rotationAnglePicker, QWidget *parent) : QGLWidget(parent)
{
    setFormat(QGLFormat(QGL::DoubleBuffer|QGL::DepthBuffer|QGL::StencilBuffer));
    setFocusPolicy(Qt::StrongFocus);

    // Setup widget pointers
    this->transformationSelector = transformationSelector;
    this->transformationTable = transformationTable;

    this->rotationAxisSelector = rotationAxisSelector;
    this->rotationAnglePicker = rotationAnglePicker;

    // enable mouse cursor tracking
    //setMouseTracking(true);

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
    loadGLTextures();
    loadTriangles();

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

    GLfloat x_m, y_m, z_m, u_m, v_m;
    GLfloat xtrans = -xpos;
    GLfloat ztrans = -zpos;
    GLfloat ytrans = -walkbias-0.25f;
    GLfloat sceneroty = yrot;

    glRotatef(lookupdown,1.0f,0,0);
    glRotatef(sceneroty,0,1.0f,0);

    glTranslatef(xtrans, ytrans, ztrans);
    glBindTexture(GL_TEXTURE_2D, texture[filter]);

    // Draw the triangles.
    for( QList<Triangle>::const_iterator i=triangles.begin(); i!=triangles.end(); ++i )
    {
        glBegin(GL_TRIANGLES);
        glNormal3f( 0.0f, 0.0f, 1.0f);
        x_m = (*i).vertex[0].x;
        y_m = (*i).vertex[0].y;
        z_m = (*i).vertex[0].z;
        u_m = (*i).vertex[0].u;
        v_m = (*i).vertex[0].v;
        glTexCoord2f(u_m,v_m); glVertex3f(x_m,y_m,z_m);

        x_m = (*i).vertex[1].x;
        y_m = (*i).vertex[1].y;
        z_m = (*i).vertex[1].z;
        u_m = (*i).vertex[1].u;
        v_m = (*i).vertex[1].v;
        glTexCoord2f(u_m,v_m); glVertex3f(x_m,y_m,z_m);

        x_m = (*i).vertex[2].x;
        y_m = (*i).vertex[2].y;
        z_m = (*i).vertex[2].z;
        u_m = (*i).vertex[2].u;
        v_m = (*i).vertex[2].v;
        glTexCoord2f(u_m,v_m); glVertex3f(x_m,y_m,z_m);
        glEnd();
    }

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

    // Crosshair (In Ortho View)					// Store The Projection Matrix
    glLoadIdentity();							// Reset The Projection Matrix
    glOrtho(0,this->width(),0,this->height(),-1,1);			// Set Up An Ortho Screen
    glMatrixMode(GL_MODELVIEW);                                         // Select The Modelview Matrix
    glTranslatef(this->width()/2, this->height()/2, 1);                 // Move To The Middle of the screen
    // Draw The Crosshair
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, texture[3]);                           // Select The Correct Texture
    glBegin(GL_QUADS);							// Start Drawing A Quad
    glTexCoord2f(0.0f,0.0f); glVertex3f(-8,-8,0.0f);                    // Bottom Left
    glTexCoord2f(1.0f,0.0f); glVertex3f( 8,-8,0.0f);                    // Bottom Right
    glTexCoord2f(1.0f,1.0f); glVertex3f( 8, 8,0.0f);                    // Top Right
    glTexCoord2f(0.0f,1.0f); glVertex3f(-8, 8,0.0f);                    // Top Left
    glEnd();
    // Done Drawing Quad
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void OriginWindow::loadTriangles()
{
    QFile f( "../world/world.txt" );

    if( f.open( IO_ReadOnly ) )
    {
        QTextStream ts( &f );

        Vertex v[3];
        int vcount = 0;
        bool allok, ok;

        while( !ts.atEnd() )
        {
            QStringList line = ts.readLine().trimmed().split(" ", QString::SkipEmptyParts);

            if( line.count() == 5 )
            {
                allok = true;
                v[vcount].x = line[0].toFloat( &ok );
                allok &= ok;
                v[vcount].y = line[1].toFloat( &ok );
                allok &= ok;
                v[vcount].z = line[2].toFloat( &ok );
                allok &= ok;
                v[vcount].u = line[3].toFloat( &ok );
                allok &= ok;
                v[vcount].v = line[4].toFloat( &ok );
                allok &= ok;

                if( allok )
                    vcount++;

                if( vcount == 3 )
                {
                    vcount = 0;
                    Triangle t;
                    t.vertex[0] = v[0];
                    t.vertex[1] = v[1];
                    t.vertex[2] = v[2];

                    triangles.append( t );
                }
            }
        }

        f.close();
    }
}

void OriginWindow::loadGLTextures()
{
    QImage t;
    QImage b;

    if ( !b.load( "../images/mud.bmp" ) )
    {
        b = QImage( 16, 16, QImage::Format_Mono);
        b.fill( Qt::green );
    }

    t = QGLWidget::convertToGLFormat( b );
    glGenTextures( 3, &texture[0] );

    glBindTexture( GL_TEXTURE_2D, texture[0] );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D( GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );

    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexImage2D( GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );

    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, t.width(), t.height(), GL_RGBA, GL_UNSIGNED_BYTE, t.bits());

    //Crosshair texture
    if ( !b.load( "../images/crosshair.bmp" ) )
    {
        b = QImage( 16, 16, QImage::Format_Mono);
        b.fill( Qt::green );
    }

    t = QGLWidget::convertToGLFormat( b );
    glBindTexture( GL_TEXTURE_2D, texture[3] );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D( GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );
}

void OriginWindow::loadLevel()
{
    // Get the fileName from a file picker.
    //tr("Level Files (*.txt, *.lvl)")
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Level"), "", "");

    // Will be "" if the user canceled the dialog.
    if(fileName == "")
    {
        printf("Dialog window was canceled.\n");
        return;
    }
    // Create the file.
    QFile file(fileName);

    // Stop if the file cannot be opened in ReadOnly or Text mode.
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        printf("Couldn't open the file.\n");
        if(!file.exists())
            printf("File doesn't exist.\n");
        return;
    }
    // Create the input stream.
    QTextStream in(&file);

    // Get the number of objects in the scene. 0 if not an int.
    in >> scene.objcount;

    // For each object stored in the file,
    for(int i=0; i<scene.objcount; i++)
    {
        // Create a reference to the object.
        MyObject& o = scene.obj[i];

        // Read in the number of points.
        in >> o.nPoints;

        // For each point,
        for(unsigned int j=0; j<o.nPoints; j++){
            // Read in the location.
            in >> o.points[j].x;
            in >> o.points[j].y;
            in >> o.points[j].z;
        }

        // Read in the number of planes.
        in >> o.nPlanes;

        // For each plane,
        for(unsigned int j=0; j<o.nPlanes; j++)
        {
            // Read in the number of points on the plane.
            in >> o.planes[j].nPoints;

            // For each point,
            for(unsigned int k=0; k<o.planes[j].nPoints; k++)
            {
                in >> o.planes[j].pids[k];
            }

            // Read in the plane's colour information.
            in >> o.planes[j].color.x;
            in >> o.planes[j].color.y;
            in >> o.planes[j].color.z;
        }

        // Set the object as casting a shadow.
        o.castsShadow = 1;

        // Compute the connectivity of the edges of the object.
        o.setConnectivity();

        // Compute the normals for the object.
        o.calcNormals();
    }
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

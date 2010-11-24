#include "originwindow.h"
#include "iostream"

OriginWindow::OriginWindow(QWidget *parent) : QGLWidget(parent)
{
    // listen for key events
    setFocusPolicy(Qt::StrongFocus);

    walkbias = 0;
    walkbiasangle = 0;
    lookupdown = 0;
    filter = 0;
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    GLfloat x_m, y_m, z_m, u_m, v_m;
    GLfloat xtrans = -xpos;
    GLfloat ztrans = -zpos;
    GLfloat ytrans = -walkbias-0.25f;
    GLfloat sceneroty = 360.0f - yrot;

    glRotatef(lookupdown,1.0f,0,0);
    glRotatef(sceneroty,0,1.0f,0);

    glTranslatef(xtrans, ytrans, ztrans);
    glBindTexture(GL_TEXTURE_2D, texture[filter]);

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
}

void OriginWindow::mousePressEvent(QMouseEvent *event)
{

}

void OriginWindow::mouseMoveEvent(QMouseEvent *event)
{

}

void OriginWindow::keyPressEvent( QKeyEvent *e )
{
    switch( e->key() )
    {
    case Qt::Key_B:
        if (blend)
        {
            blend = false;
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            blend = true;
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
        }

        updateGL();
        break;

                case Qt::Key_F:
        filter++;
        if( filter > 2 )
            filter = 0;

        updateGL();
        break;

                case Qt::Key_Up:
        xpos -= (float)sin(heading*piover180) * 0.05f;
        zpos -= (float)cos(heading*piover180) * 0.05f;
        if (walkbiasangle >= 359.0f)
        {
            walkbiasangle = 0.0f;
        }
        else
        {
            walkbiasangle+= 10;
        }
        walkbias = (float)sin(walkbiasangle * piover180)/20.0f;

        updateGL();
        break;

                case Qt::Key_Down:
        xpos += (float)sin(heading*piover180) * 0.05f;
        zpos += (float)cos(heading*piover180) * 0.05f;
        if (walkbiasangle <= 1.0f)
        {
            walkbiasangle = 359.0f;
        }
        else
        {
            walkbiasangle-= 10;
        }
        walkbias = (float)sin(walkbiasangle * piover180)/20.0f;

        updateGL();
        break;

                case Qt::Key_Left:

        heading += 1.0f;
        yrot = heading;

        updateGL();
        break;

                case Qt::Key_Right:

        heading -= 1.0f;
        yrot = heading;

        updateGL();

        break;

                case Qt::Key_PageUp:
        lookupdown-= 1.0f;

        updateGL();
        break;

                case Qt::Key_PageDown:
        lookupdown+= 1.0f;

        updateGL();
        break;
    }
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
}

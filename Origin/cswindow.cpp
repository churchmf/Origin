#include "cswindow.h"
#include <qgl.h>
#include <math.h>
#include <QKeyEvent>

CSWindow::CSWindow(QWidget *parent) : QGLWidget(parent)
{
    setFormat(QGLFormat(QGL::DoubleBuffer|QGL::DepthBuffer));
    setGeometry(100,100,700,700);
    rotX = -145;
    rotY = -215;

}
void CSWindow::initializeGL()
{
    qglClearColor(Qt::white);
    glColor3f(1.0,1.0,1.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
}
void CSWindow::resizeGL(int width, int height)
{
    GLdouble aspect, theta, n, f, theta_radian, theta_2_radian, top, bott,left, right;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0,0,(GLsizei) width, (GLsizei) height);
    aspect = (GLdouble)width/(GLdouble)height;
    theta = 60.0; theta_radian = theta*PI/180.0;
    theta_2_radian = theta_radian/2.0;
    n = 1.0;
    f = 1500.0;
    top = n*tan(theta_radian/2.0);
    bott = -top;
    right = top*aspect;
    left = -right;
    glFrustum(left,right, bott, top, n, f);
}
void CSWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    draw();
}

void CSWindow::draw()
{
    glLoadIdentity();
    //    GLfloat x_m, y_m, z_m, u_m, v_m;				// Floating Point For Temp X, Y, Z, U And V Vertices
    //    GLfloat xtrans = -xpos;						// Used For Player Translation On The X Axis
    //    GLfloat ztrans = -zpos;						// Used For Player Translation On The Z Axis
    //    GLfloat ytrans = -walkbias-0.25f;				// Used For Bouncing Motion Up And Down
    //    GLfloat sceneroty = 360.0f - yrot;				// 360 Degree Angle For Player Direction
    //    glRotatef(lookupdown,1.0f,0,0);					// Rotate Up And Down To Look Up And Down
    //    glRotatef(sceneroty,0,1.0f,0);					// Rotate Depending On Direction Player Is Facing
    //    glTranslatef(xtrans, ytrans, ztrans);				// Translate The Scene Based On Player Position


}
void CSWindow::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}
void CSWindow::mouseMoveEvent(QMouseEvent *event)
{
    GLfloat dx = GLfloat(event->x() - lastPos.x())/width();
    GLfloat dy = GLfloat(event->y() - lastPos.y())/height();

    if(event->buttons() & Qt::LeftButton)
    {
        rotX += 180*dx;
        rotY -= 180*dy;
        updateGL();
    }
    lastPos = event->pos();
}

/** Handles KeyEvents **/
void CSWindow::keyPressEvent(QKeyEvent * event){
    int key = event->key ();
    switch (key)
    {
    case Qt::Key_Right:
        //        yrot -= 1.5f;							// Rotate The Scene To The Left
        break;
    case Qt::Key_Left:
        //        yrot += 1.5f;							// Rotate The Scene To The Right
        break;
    case Qt::Key_Up:
        //        xpos -= (float)sin(heading*piover180) * 0.05f;			// Move On The X-Plane Based On Player Direction
        //        zpos -= (float)cos(heading*piover180) * 0.05f;			// Move On The Z-Plane Based On Player Direction
        //        if (walkbiasangle >= 359.0f)					// Is walkbiasangle>=359?
        //        {
        //            walkbiasangle = 0.0f;					// Make walkbiasangle Equal 0
        //        }
        //        else								// Otherwise
        //        {
        //            walkbiasangle+= 10;					// If walkbiasangle < 359 Increase It By 10
        //        }
        //        walkbias = (float)sin(walkbiasangle * piover180)/20.0f;		// Causes The Player To Bounce
        break;
    case Qt::Key_Down:
        //        xpos += (float)sin(heading*piover180) * 0.05f;			// Move On The X-Plane Based On Player Direction
        //        zpos += (float)cos(heading*piover180) * 0.05f;			// Move On The Z-Plane Based On Player Direction
        //        if (walkbiasangle <= 1.0f)					// Is walkbiasangle<=1?
        //        {
        //            walkbiasangle = 359.0f;					// Make walkbiasangle Equal 359
        //        }
        //        else								// Otherwise
        //        {
        //            walkbiasangle-= 10;					// If walkbiasangle > 1 Decrease It By 10
        //        }
        //        walkbias = (float)sin(walkbiasangle * piover180)/20.0f;		// Causes The Player To Bounce
        break;
    }


    //    if (event->key () == Qt::Key_Delete)
    //    {


    //    }
    //    if (keys[VK_RIGHT])							// Is The Right Arrow Being Pressed?
    //    {
    //        yrot -= 1.5f;							// Rotate The Scene To The Left
    //    }

    //    if (keys[VK_LEFT])							// Is The Left Arrow Being Pressed?
    //    {
    //        yrot += 1.5f;							// Rotate The Scene To The Right
    //    }

    //    if (keys[VK_UP])							// Is The Up Arrow Being Pressed?
    //    {
    //        xpos -= (float)sin(heading*piover180) * 0.05f;			// Move On The X-Plane Based On Player Direction
    //        zpos -= (float)cos(heading*piover180) * 0.05f;			// Move On The Z-Plane Based On Player Direction
    //        if (walkbiasangle >= 359.0f)					// Is walkbiasangle>=359?
    //        {
    //            walkbiasangle = 0.0f;					// Make walkbiasangle Equal 0
    //        }
    //        else								// Otherwise
    //        {
    //            walkbiasangle+= 10;					// If walkbiasangle < 359 Increase It By 10
    //        }
    //        walkbias = (float)sin(walkbiasangle * piover180)/20.0f;		// Causes The Player To Bounce
    //    }

    //    if (keys[VK_DOWN])							// Is The Down Arrow Being Pressed?
    //    {
    //        xpos += (float)sin(heading*piover180) * 0.05f;			// Move On The X-Plane Based On Player Direction
    //        zpos += (float)cos(heading*piover180) * 0.05f;			// Move On The Z-Plane Based On Player Direction
    //        if (walkbiasangle <= 1.0f)					// Is walkbiasangle<=1?
    //        {
    //            walkbiasangle = 359.0f;					// Make walkbiasangle Equal 359
    //        }
    //        else								// Otherwise
    //        {
    //            walkbiasangle-= 10;					// If walkbiasangle > 1 Decrease It By 10
    //        }
    //        walkbias = (float)sin(walkbiasangle * piover180)/20.0f;		// Causes The Player To Bounce
    //    }
}

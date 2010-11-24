#include "originwindow.h"
#include "iostream"

void OriginWindow::mousePressEvent(QMouseEvent *e)
{

}

void OriginWindow::mouseMoveEvent(QMouseEvent *e)
{

    e->accept();
    QPoint pos = e->pos();
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

   case Qt::Key_W:
        xpos -= (float)sin(heading*piover180) * 0.03f;
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

        updateGL();
        break;

    case Qt::Key_S:
        xpos += (float)sin(heading*piover180) * 0.03f;
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

        updateGL();
        break;

    case Qt::Key_A:

        heading += 1.0f;
        yrot = heading;

        updateGL();
        break;

    case Qt::Key_D:

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

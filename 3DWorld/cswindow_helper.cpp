#include "cswindow.h"
#define ESCAPE 27
#define PAGE_UP 73
#define PAGE_DOWN 81
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77

const float piover180 = 0.0174532925f;

void CSWindow::mousePressEvent(QMouseEvent *event)
{
    //prev = event->pos();
}
void CSWindow::mouseMoveEvent(QMouseEvent *event)
{
    //GLfloat dx = GLfloat(event->x() - prev.x());
    //GLfloat dy = GLfloat(event->y() - prev.y());
    //if(event->buttons() & Qt::LeftButton)
    //{
     //   scene.obj[scene.curObject].rotation.y += (dx);
     //   scene.obj[scene.curObject].rotation.x += (dy);
     //   updateGL();
    //}
    //prev= event->pos();
}
void CSWindow::keyPressEvent(QKeyEvent *event)
{
    int dr= 5;
    float dx = .5f;
    switch(event->key()){
    case ESCAPE: // kill everything.
        /* exit the program...normal termination. */
        exit(1);
        break; // redundant.
        //lighting
    case 'b':
    case 'B': // switch the blending
        printf("B/b pressed; blending is: %d\n", blend);
        blend = blend ? 0 : 1;              // switch the current value of blend, between 0 and 1.
        if (blend) {
            glEnable(GL_BLEND);
            glDisable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
        }
        printf("Blending is now: %d\n", blend);
        break;
    case 'f':
    case 'F': // switch the filter
        printf("F/f pressed; filter is: %d\n", filter);
        filter++;                           // switch the current value of filter, between 0/1/2;
        if (filter > 2) {
            filter = 0;
        }
        printf("Filter is now: %d\n", filter);
        break;
    case 'l':
    case 'L': // switch the lighting
        printf("L/l pressed; lighting is: %d\n", light);
        light = light ? 0 : 1;              // switch the current value of light, between 0 and 1.
        if (light) {
            glEnable(GL_LIGHTING);
        } else {
            glDisable(GL_LIGHTING);
        }
        printf("Lighting is now: %d\n", light);
        break;
    case 16777238:			//page up
        z -= 0.2f;
        lookupdown -= 0.2f;
        break;
    case 16777239:			//page down
        z += 0.2f;
        lookupdown += 1.0f;
        break;
    case 16777234:			//left
        yrot += 1.5f;
        break;
    case 16777236:
        yrot -= 1.5f;
        break;
    case 16777235:
        xpos -= (float)sin(yrot*piover180) * 0.05f;
        zpos -= (float)cos(yrot*piover180) * 0.05f;
        if (walkbiasangle >= 359.0f)
            walkbiasangle = 0.0f;
        else
            walkbiasangle+= 10;
        walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
        break;
    case 16777237:
        xpos += (float)sin(yrot*piover180) * 0.05f;
        zpos += (float)cos(yrot*piover180) * 0.05f;
        if (walkbiasangle <= 1.0f)
            walkbiasangle = 359.0f;
        else
            walkbiasangle-= 10;
        walkbias = (float)sin(walkbiasangle * piover180)/20.0f;
        break;
    }
    printf("key %d\n" , event->key());
    updateGL();
}

/*
This file is used in all of the parts without change
*/
#ifndef CSWINDOW_H
#define CSWINDOW_H
#include <QGLWidget>
#include <QtGui>
class CSWindow : public QGLWidget
{
        Q_OBJECT
public:
        CSWindow(QWidget *parent =0);
        int light;           // lighting on/off
        int blend;        // blending on/off
        GLuint loop;             // general loop variable
        GLuint texture[3];       // storage for 3 textures;



        GLfloat xrot;            // x rotation
        GLfloat yrot;            // y rotation
        GLfloat xspeed;          // x rotation speed
        GLfloat yspeed;          // y rotation speed

        GLfloat walkbias;
        GLfloat walkbiasangle;

        GLfloat lookupdown;

        GLuint filter;       // texture filtering method to use (nearest, linear, linear + mipmaps)

        float heading, xpos, zpos;

        GLfloat camx, camy, camz; // camera location.
        GLfloat therotate;

        GLfloat z;                       // depth into the screen.

protected:
        void initializeGL();
        void LoadGLTextures();
        void SetupWorld();
        float rad(float angle);
        void readstr(FILE *f, char *string);
        void resizeGL(int width, int height);
        void paintGL();
        char *_title;

        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void keyPressEvent(QKeyEvent *event);

private:
    //methods
        void rotate();
        int InitGL();
        int InitObjects();

};
#endif

#ifndef CSWINDOW_H
#define CSWINDOW_H
#include <QGLWidget>
#include <QtGui>
#define PI 3.1415926
#define piover180 PI/180
class CSWindow : public QGLWidget
{
        Q_OBJECT
public:
        CSWindow(QWidget *parent =0);
//signals:
        //void setSlider(int r);
//public slots:
//        void animate();
//        void getFaceToFold(int);
//        void rotateFace(int);  // rotate face by angle
protected:
        void initializeGL();
        void resizeGL(int width, int height);
        void paintGL();
        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void keyPressEvent(QKeyEvent *event);
private:
        void draw();
        QPoint lastPos;
        GLfloat rotX;
        GLfloat rotY;
};
#endif

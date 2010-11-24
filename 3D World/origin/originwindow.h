#ifndef ORIGINWINDOW_H
#define ORIGINWINDOW_H
#include <QGLWidget>
#include <QtGui>
#include <qgl.h>
#include <qimage.h>
#include <qfile.h>
#include <QTextStream.h>
#include <QStringList.h>
#include <math.h>
#define IO_ReadOnly QIODevice::ReadOnly

const float piover180 = 0.0174532925f;

typedef struct {
    GLfloat x, y, z, u, v;
} Vertex;

typedef struct {
    Vertex vertex[3];
} Triangle;

class OriginWindow : public QGLWidget
{
    Q_OBJECT

public:
    OriginWindow(QWidget *parent=0);

protected:
    void initializeGL();
    void resizeGL( int width, int height );

    void paintGL();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    bool blend;
    GLfloat heading;
    GLfloat xpos;
    GLfloat zpos;
    GLfloat yrot;
    GLfloat walkbias;
    GLfloat walkbiasangle;
    GLfloat lookupdown;

    GLuint filter;
    GLuint texture[3];
    QList<Triangle> triangles;

    void loadTriangles();
    void loadGLTextures();
};
#endif

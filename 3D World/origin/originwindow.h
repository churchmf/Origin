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
#include "scene.h"
#include "iostream"

#define IO_ReadOnly QIODevice::ReadOnly

const float piover180 = 0.0174532925f;

enum TransformationType { Translate, Scale };
enum RotationAxis { RotateByX, RotateByY, RotateByZ };

// track the keystate since Qt is ghetto
typedef struct {
    bool w,a,s,d,space,e;
} KeyState;

typedef struct {
    GLfloat x,y,z;
} Vector3;

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
    OriginWindow(QComboBox* transformationSelector, QTableWidget *transformationTable, QComboBox* rotationAxisSelector, QSpinBox* rotationAnglePicker, QWidget *parent=0);
public slots:
    void applyTransformation();
    void applyRotation();
    void timerLoop();

protected:
    void initializeGL();
    void resizeGL( int width, int height );
    void paintGL();

    //User Input Parsing
    MyPoint getTransformationVector(bool* valid);
    int getTransformationType(bool* valid);
    int getRotationAxis(bool* valid);
    int getRotationAngle(bool* valid);

    //Update Loops
    void updatePlayerPosition();
    void updatePropsPosition();
    void updatePropsRotation();

    //User Input Events
    void updateKeyState(int key, bool isPressed);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    //Event timer
    QTimer* timer;
    Scene scene;

    // Widgets
    QSpinBox* rotationAnglePicker;
    QComboBox* rotationAxisSelector;
    QTableWidget* transformationTable;
    QComboBox* transformationSelector;

    bool blend;
    GLfloat heading;
    GLfloat xpos;
    GLfloat zpos;
    GLfloat yrot;
    GLfloat walkbias;
    GLfloat walkbiasangle;
    GLfloat lookupdown;

    GLuint filter;
    GLuint texture[4];
    QList<Triangle> triangles;

    void loadTriangles();
    void loadGLTextures();
    void loadLevel();

    // Last mouse position
    QPoint lastPos;
    // Keys currently pressed
    KeyState keysPressed;
};
#endif

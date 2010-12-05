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
#define MIN_WIDTH 800
#define MIN_HEIGHT 600
#define MAX_TEXTURES 15

// This constant protects against rounding errors that could otherwise arise during collision detection.
const double zeroCutoff = 1e-10;

const float piover180 = 0.0174532925f;

enum TransformationType { Translate, Scale };
enum RotationAxis { RotateByX, RotateByY, RotateByZ };

// track the keystate since Qt is ghetto
typedef struct {
    bool w,a,s,d,space,e;
} KeyState;

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
    void updatePropsScale();

    //Check collisions
    bool checkCollision(MyObject& object1);
    bool pointInsideTriangle(MyPoint& p, MyPoint& a, MyPoint& b, MyPoint& c);

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

    GLfloat heading;
    GLfloat xpos;
    GLfloat zpos;
    GLfloat yrot;
    GLfloat walkbias;
    GLfloat walkbiasangle;
    GLfloat lookupdown;

    GLuint texture[MAX_TEXTURES];
    int textureCount;

    void loadLevel();

    void readObject(QFile* file, MyObject& o);  //read .obj file
    void readMaterial(QFile* file, MyObject& o);    //read .mtl file for .obj file
    GLuint& readTexture(QString fileLocation);    //read texture from .mtl file, returns the index of the texture

    void drawHUD();
    void drawCrosshair();
    void drawAxis();

    // Last mouse position
    QPoint lastPos;
    // Keys currently pressed
    KeyState keysPressed;
};
#endif

#include "originwindow.h"

void OriginWindow::applyTransformation()
{
    //verify that the input is valid
    bool validInput = true;
    MyPoint transformation = getTransformationVector(&validInput);
    int type = getTransformationType(&validInput);

    // if the input was invalid, do nothing
    if (!validInput)
        return;

    // if no prop is selected, do nothing
    int propIndex = scene.selectedProp;
    if (propIndex == NO_SELECTION)
        return;

    MyObject& selectedProp = scene.prop[propIndex];
    // Translate
    if (type == Translate)
    {
        selectedProp.goalPosition = (selectedProp.position).plus(transformation);
        selectedProp.isTransforming = true;
        //printf("Translate\n");
    }
    //Scale
    else if (type == Scale)
    {
        if (transformation.x <= 0 || transformation.y <= 0 || transformation.z <= 0)
            return;
        selectedProp.goalScale = (selectedProp.scale).scale(transformation);
        selectedProp.isTransforming = true;
        //printf("Scale\n");
    }
}

void OriginWindow::applyRotation()
{
    bool validInput = true;
    int angle = getRotationAngle(&validInput);
    int axis = getRotationAxis(&validInput);

    // if the input was invalid, do nothing
    if (!validInput)
        return;

    // if no prop is selected, do nothing
    int propIndex = scene.selectedProp;
    if (propIndex == NO_SELECTION)
        return;

    MyObject& selectedProp = scene.prop[propIndex];
    // Rotate
    if (axis == RotateByX)
    {
        selectedProp.goalRotation.x = (selectedProp.rotation).x + angle;
        selectedProp.goalRotation.y = (selectedProp.rotation).y;
        selectedProp.goalRotation.z = (selectedProp.rotation).z;
        selectedProp.isTransforming = true;
        //printf("RotateByX\n");
    }
    else if (axis == RotateByY)
    {
        selectedProp.goalRotation.x = (selectedProp.rotation).x;
        selectedProp.goalRotation.y = (selectedProp.rotation).y + angle;
        selectedProp.goalRotation.z = (selectedProp.rotation).z;
        selectedProp.isTransforming = true;
        //printf("RotateByY\n");
    }
    else if (axis == RotateByZ)
    {
        selectedProp.goalRotation.x = (selectedProp.rotation).x;
        selectedProp.goalRotation.y = (selectedProp.rotation).y;
        selectedProp.goalRotation.z = (selectedProp.rotation).z + angle;
        selectedProp.isTransforming = true;
        //printf("RotateByZ\n");
    }
}

MyPoint OriginWindow::getTransformationVector(bool* valid)
{
    MyPoint transformation;
    bool isValid = true;
    transformation.x = (transformationTable->item(0,0)->data(Qt::DisplayRole)).toFloat(&isValid);
    *valid &= isValid;
    transformation.y = (transformationTable->item(0,1)->data(Qt::DisplayRole)).toFloat(&isValid);
    *valid &= isValid;
    transformation.z = (transformationTable->item(0,2)->data(Qt::DisplayRole)).toFloat(&isValid);
    *valid &= isValid;
    return transformation;
}

int OriginWindow::getTransformationType(bool* valid)
{
    bool isValid = true;
    int value = (transformationSelector->itemData(transformationSelector->currentIndex())).toInt(&isValid);
    *valid &= isValid;
    return value;
}

int OriginWindow::getRotationAxis(bool* valid)
{
    bool isValid = true;
    int value = (rotationAxisSelector->itemData(rotationAxisSelector->currentIndex())).toInt(&isValid);
    *valid &= isValid;
    return value;
}

int OriginWindow::getRotationAngle(bool* valid)
{
    bool isValid = true;
    int value = rotationAnglePicker->value();
    *valid &= isValid;
    return value;
}

void OriginWindow::mousePressEvent(QMouseEvent *e)
{

}

void OriginWindow::mouseMoveEvent(QMouseEvent *e)
{
    GLfloat dx = GLfloat(e->x() - lastPos.x()) /width();
    GLfloat dy = GLfloat(e->y() - lastPos.y()) / height();

    if (dy > 0)
    {
        if (lookupdown < 90)
            lookupdown+= 1.0f;
    }
    else if (dy < 0)
    {
        if (lookupdown > -90)
            lookupdown-= 1.0f;
    }

    if (dx > 0)
    {
        heading += 1.0f;
        if (heading > 359.0f)
        {
            heading = 0.0f;
        }
        yrot = heading;

    }
    else if (dx < 0)
    {
        heading -= 1.0f;
        if (heading < 0)
        {
            heading = 359.0f;
        }
        yrot = heading;
    }
    updateGL();
    //printf("==================================================\n");
    //printf("lookupdown %f \n heading %f \n", lookupdown, heading);
    lastPos = e->pos();
}
void OriginWindow::keyReleaseEvent( QKeyEvent *e )
{
    int key = e->key();
    updateKeyState(key,false);
}

void OriginWindow::updateKeyState(int key, bool isPressed)
{
    if (key == Qt::Key_W)
    {
        keysPressed.w = isPressed;
    }
    else if (key == Qt::Key_S)
    {
        keysPressed.s = isPressed;
    }
    else if (key == Qt::Key_A)
    {
        keysPressed.a = isPressed;
    }
    else if (key == Qt::Key_D)
    {
        keysPressed.d = isPressed;
    }
    else if (key == Qt::Key_Space)
    {
        keysPressed.space = isPressed;
    }
}

void OriginWindow::keyPressEvent( QKeyEvent *e )
{
    int key = e->key();
    updateKeyState(key,true);

    if (key == Qt::Key_Escape)
    {
        exit(0);
    }
    else if (key == Qt::Key_T)
    {
        applyTransformation();
    }
    else if (key == Qt::Key_R)
    {
        applyRotation();
    }
    else if (key == Qt::Key_E)
    {
        selectProp();
    }
    else if (key == Qt::Key_O)
    {
        loadLevel();
    }
}

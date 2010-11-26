#include "originwindow.h"
#include "iostream"

void OriginWindow::applyTransformation()
{
    //verify that the input is valid
    bool validInput = true;
    Vector3 transformation = getTransformationVector(&validInput);
    int type = getTransformationType(&validInput);

    // if the input was invalid, do nothing
    if (!validInput)
        return;

    printf("transform\n");
}

void OriginWindow::applyRotation()
{
    bool validInput = true;
    int angle = getRotationAngle(&validInput);
    int axis = getRotationAxis(&validInput);

    // if the input was invalid, do nothing
    if (!validInput)
        return;

    printf("rotate\n");
}

Vector3 OriginWindow::getTransformationVector(bool* valid)
{
    Vector3 transformation;
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
        xpos += (float)sin(heading*piover180) * 0.03f;
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
        xpos -= (float)sin(heading*piover180) * 0.03f;
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
        xpos -= (float)cos(heading*piover180) * 0.03f;
        zpos -= (float)sin(heading*piover180) * 0.03f;

        updateGL();
        break;

    case Qt::Key_D:

        xpos += (float)cos(heading*piover180) * 0.03f;
        zpos += (float)sin(heading*piover180) * 0.03f;

        updateGL();
        break;

    case Qt::Key_Escape:
        exit(0);
        break;
    }
}

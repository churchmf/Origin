#include "originwindow.h"

void OriginWindow::selectProp()
{
    MyPoint p1;
    p1.x = xpos;
    p1.y = ypos; //walkbias+0.25f;
    p1.z = zpos;

    MyPoint gaze;
    gaze.x = (float)sin(heading*piover180);
    gaze.y = -(float)sin(lookupdown * piover180);
    gaze.z = -(float)cos(heading*piover180);

    MyPoint p2 = p1.plus((gaze.times(INF)));

    //printf("p1: %f,%f,%f \n", p1.x,p1.y,p1.z);
    //printf("gaze: %f,%f,%f \n", gaze.x,gaze.y,gaze.z);
    //printf("p2: %f,%f,%f \n", p2.x,p2.y,p2.z);

    int closestIndex = NO_SELECTION;
    float closestDist = INF;
    for(int i=0; i<scene.propcount; i++)
    {
        // Create a reference to the prop.
        MyObject& o = scene.prop[i];

        for (unsigned int j=0; j <o.nPlanes; j++)
        {
            //calculate the plane normal
            MyPlane& plane = o.planes[j];
            MyPoint A = o.points[plane.pids[0]].plus(o.position);
            MyPoint B = o.points[plane.pids[1]].plus(o.position);
            MyPoint C = o.points[plane.pids[2]].plus(o.position);

            // cast a ray and collide with any props
            MyPoint t;
            if (lineTriangleCollision(A,B,C,p1,p2,t))
            {
                //printf("select! %f,%f,%f\n",t.x,t.y,t.z);

                //get the closest prop
                float dist = t.length(p1);
                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestIndex = i;
                }
            }
        }
        scene.selectedProp = closestIndex;
    }
}

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
        selectedProp.goalRotation = selectedProp.rotation;
        selectedProp.goalPosition = (selectedProp.position).plus(transformation);
        //printf("Translate\n");
    }
    //Scale
    else if (type == Scale)
    {
        if (transformation.x <= 0 || transformation.y <= 0 || transformation.z <= 0)
            return;

        // Set the current scale to be 1. (to allow for relative scaling.)
        selectedProp.scale.x = selectedProp.scale.y = selectedProp.scale.z = 1.0f;

        selectedProp.goalScale = (selectedProp.scale).scale(transformation);
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
    selectedProp.goalPosition = selectedProp.position;

    // Rotate
    if (axis == RotateByX)
    {
        selectedProp.rotation.x = 0;
        selectedProp.goalRotation.x = angle;
    }
    else if (axis == RotateByY)
    {    
        selectedProp.rotation.y = 0;
        selectedProp.goalRotation.y = angle;
    }
    else if (axis == RotateByZ)
    {
        selectedProp.rotation.z = 0;
        selectedProp.goalRotation.z = angle;
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

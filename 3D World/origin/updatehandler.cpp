#include "originwindow.h"

// Main program loop
void OriginWindow::timerLoop()
{
    //Update isTransforming for all props
    updateIsTransforming();

    //Apply Physics for all props
    applyPhysics();

    //Update the player's position
    updatePlayerPosition();

    //Update the props' position
    updatePropsPosition();

    //Update the props' rotation
    updatePropsRotation();

    //Update the props' scale
    updatePropsScale();

    //UpdateGL
    updateGL();
}

void OriginWindow::updatePlayerPosition()
{
    // Keep track of the old x and z positions for collision detection purposes.
    GLfloat oldxpos = xpos;
    GLfloat oldzpos = zpos;

    // Handle KeysPressed
    if (keysPressed.w)
    {
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
    }
    if (keysPressed.s)
    {
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
    }
    ypos = walkbias+0.25f;
    if (keysPressed.a)
    {
        xpos -= (float)cos(heading*piover180) * 0.03f;
        zpos -= (float)sin(heading*piover180) * 0.03f;
    }
    if (keysPressed.d)
    {
        xpos += (float)cos(heading*piover180) * 0.03f;
        zpos += (float)sin(heading*piover180) * 0.03f;
    }
    if (keysPressed.space)
    {
        //ypos += JUMP_STEP;
    }

    // Exit early if there's no movement.
    if(xpos == oldxpos && zpos == oldzpos)
    {
        return;
    }

    // Convert the player's previous position into a MyPoint.
    MyPoint linePoint0;
    linePoint0.x = oldxpos;
    linePoint0.y = 0.25f;
    linePoint0.z = oldzpos;

    // Convert the player's new position into a MyPoint.
    MyPoint linePoint1;
    linePoint1.x = xpos;
    linePoint1.y = 0.25f;
    linePoint1.z = zpos;

    QList<MyPoint> before;
    before.append(linePoint0);
    QList<MyPoint> after;
    after.append(linePoint1);

    if (checkCollisionWithAll(before,after,linePoint0))
    {
        // There is a collision, so reset the position to the last position.
        xpos = oldxpos;
        zpos = oldzpos;
        return;
    }
    printf("Camera at (%f,%f,%f)\n",xpos,ypos,zpos);
}

void OriginWindow::updatePropsPosition()
{
    for(int i=0; i<scene.propcount; i++)
    {
        // Create a reference to the prop.
        MyObject& o = scene.prop[i];
        MyPoint& goal = o.goalPosition;
        MyPoint& curPosition = o.position;

        // If the prop is not near the goal
        if (!curPosition.equals(goal) && o.isTransforming)
        {
            // Get the direction to move
            MyPoint diff = goal.minus(curPosition);
            diff.normalize();
            MyPoint delta;
            delta.x = diff.x;
            delta.y = diff.y;
            delta.z = diff.z;
            // snap to place if under 0.5
            if (fabs(diff.x+diff.y+diff.z) > 0.05f)
                delta = delta.times(PROP_TRANSLATE_STEP);

            QList<MyPoint> before;
            for (unsigned int j=0; j<o.nPoints; j++)
            {
                before.append(o.points[j].plus(o.position));
            }

            QList<MyPoint> after;
            for (unsigned int j=0; j<o.nPoints; j++)
            {
                MyPoint afterTransformation;
                afterTransformation.x = before.at(j).x + delta.x;
                afterTransformation.y = before.at(j).y + delta.y;
                afterTransformation.z = before.at(j).z + delta.z;
                after.append(afterTransformation);
            }

            // Check for collision for the move
            if (checkCollisionWithAll(before,after,curPosition))
            {
                //if there was a collision, set the goal to the current position, make the object bounce back a bit
                goal = curPosition;
                o.velocity.x = -delta.x * 0.2f;
                o.velocity.y = -delta.y * 0.2f;
                o.velocity.z = -delta.z * 0.2f;
            }
            else
            {
                // Move towards the goal
                curPosition = curPosition.plus(delta);
            }
        }
    }
}

void OriginWindow::updatePropsRotation()
{
    for(int i=0; i<scene.propcount; i++)
    {
        // Create a reference to the prop.
        MyObject& o = scene.prop[i];
        MyPoint& goal = o.goalRotation;
        MyPoint& curRotation = o.rotation;

        if (!curRotation.equals(goal) && o.isTransforming)
        {
            MyPoint diff = goal.minus(curRotation);
            MyPoint delta;
            delta.x = diff.x;
            delta.y = diff.y;
            delta.z = diff.z;
            // snap to rotation if under 3 degrees
            if (fabs(diff.x+diff.y+diff.z) > 3.0f)
                delta = delta.times(PROP_ROTATE_STEP);

            //copy the object, so we can revert if there is a collision
            MyPoint oldPoints[MAX_OBJECT_POINTS];
            for (unsigned int j=0; j < o.nPoints; j++)
            {
                MyPoint point = o.points[j];
                oldPoints[j] = point;
            }

            //for every point, rotate it and check for collision
            for (unsigned int j=0; j < o.nPoints; j++)
            {
                MyPoint& oldPoint = o.points[j];
                MyPoint newPoint;
                //X rotation
                if (fabs(delta.x) > 0)
                {
                    float angle = delta.x * piover180;
                    //x' = x
                    newPoint.x = oldPoint.x;
                    //y' = y*cos q - z*sin q
                    newPoint.y = oldPoint.y*cos(angle) - oldPoint.z*sin(angle);
                    //z' = y*sin q + z*cos q
                    newPoint.z = oldPoint.y*sin(angle) + oldPoint.z*cos(angle);
                }
                // Y rotation
                else if (fabs(delta.y) > 0)
                {
                    float angle = delta.y * piover180;
                    //x' = z*sin q + x*cos q
                    newPoint.x = oldPoint.z*sin(angle) + oldPoint.x*cos(angle);
                    //y' = y
                    newPoint.y = oldPoint.y;
                    // z' = z*cos q - x*sin q
                    newPoint.z = oldPoint.z*cos(angle) - oldPoint.x*sin(angle);

                }
                // Z rotation
                else if (fabs(delta.z) > 0)
                {
                    float angle = delta.z * piover180;
                    //x' = x*cos q - y*sin q
                    newPoint.x = oldPoint.x*cos(angle) - oldPoint.y*sin(angle);
                    //y' = x*sin q + y*cos q
                    newPoint.y = oldPoint.x*sin(angle) + oldPoint.y*cos(angle);
                    //z' = z
                    newPoint.z = oldPoint.z;
                }

                QList<MyPoint> before;
                before.append(oldPoint);

                QList<MyPoint> after;
                after.append(newPoint);

                // Check for collision for the point rotate
                if (checkCollisionWithAll(before,after,o.position))
                {
                    //revert the points back since the point collides, stop rotating since we collided
                    //o = old;
                    for (unsigned int k=0; k < o.nPoints;k++)
                    {
                        MyPoint point = oldPoints[k];
                        o.points[k].x = point.x;
                        o.points[k].y = point.y;
                        o.points[k].z = point.z;
                    }
                    o.goalPosition = o.position;
                    goal.x = curRotation.x = 0;
                    goal.y = curRotation.y = 0;
                    goal.z = curRotation.z = 0;
                    return;
                }
                else
                {
                    oldPoint = newPoint;
                }
            }
            // Move towards the goal rotation since there was no collision
            curRotation = curRotation.plus(delta);
            o.goalPosition = o.position;
        }
    }
}

void OriginWindow::updatePropsScale()
{
    for(int i=0; i<scene.propcount; i++)
    {
        // Create a reference to the prop.
        MyObject& o = scene.prop[i];
        MyPoint& goal = o.goalScale;
        MyPoint& curScale = o.scale;

        // If the prop is not near the goal
        if (!curScale.equals(goal) && o.isTransforming)
        {
            QList<MyPoint> before;
            for (unsigned int j=0; j< o.nPoints; j++)
            {
                before.append(o.points[j].plus(o.position));
            }

            QList<MyPoint> after;
            for (unsigned int j=0; j < o.nPoints; j++)
            {
                MyPoint afterTransformation;
                afterTransformation.x = before.at(j).x * goal.x;
                afterTransformation.y = before.at(j).y * goal.y;
                afterTransformation.z = before.at(j).z * goal.z;
                after.append(afterTransformation);
            }

            // Check for collision for the move
            if (!checkCollisionWithAll(before,after,o.position))
            {
                // Update the scale
                curScale = goal;

                o.position.y = o.position.y + goal.y/2;
                o.goalPosition = o.position;

                // Update the point's locations.
                for(unsigned int j=0; j<o.nPoints; j++)
                {
                    MyPoint& p = o.points[j];
                    p.x *= goal.x;
                    p.y *= goal.y;
                    p.z *= goal.z;
                }
            }
        }
    }
}

void OriginWindow::applyPhysics()
{
    for(int i=0; i<scene.propcount; i++)
    {
        // Create a reference to the prop.
        MyObject& o = scene.prop[i];

        // Don't apply physics to transforming objects
        if (o.isTransforming)
            continue;

        // apply gravity
        o.velocity.y += GRAVITY_STEP;

        MyPoint delta;
        delta.x = o.velocity.x;
        delta.y = o.velocity.y;
        delta.z = o.velocity.z;

        QList<MyPoint> before;
        for (unsigned int j=0; j<o.nPoints; j++)
        {
            before.append(o.points[j].plus(o.position));
        }

        QList<MyPoint> after;
        for (unsigned int j=0; j<o.nPoints; j++)
        {
            MyPoint afterTransformation;
            afterTransformation.x = before.at(j).x + delta.x;
            afterTransformation.y = before.at(j).y + delta.y;
            afterTransformation.z = before.at(j).z + delta.z;
            after.append(afterTransformation);
        }

        MyPoint& curPosition = o.position;
        // Check for collision
        if (checkCollisionWithAll(before,after,curPosition))
        {
            o.velocity.x = -delta.x * 0.4f;
            o.velocity.y = -delta.y * 0.4f;
            o.velocity.z = -delta.z * 0.4f;
        }
        else
        {

            // Adjust position based on velocity
            curPosition.x += o.velocity.x;
            curPosition.y += o.velocity.y;
            curPosition.z += o.velocity.z;

            o.goalPosition = o.position;

            // reduce objects velocity (friction)
            o.velocity.times(0.8f);
        }
    }
}

void OriginWindow::updateIsTransforming()
{
    for(int i=0; i<scene.propcount; i++)
    {
        // Create a reference to the prop.
        MyObject& o = scene.prop[i];

        // The object is transforming if it is not at it's goal
        o.isTransforming = !(o.goalPosition.equals(o.position) && o.goalRotation.equals(o.rotation) && o.goalScale.equals(o.scale));
    }
}



#include "originwindow.h"

// Main program loop
void OriginWindow::timerLoop()
{
    //Update the player's position
    updatePlayerPosition();

    //Update the props' position
    updatePropsPosition();

    //Update the props' rotation
    updatePropsRotation();

    //Update the props' scale
    updatePropsScale();

    //Update isTransforming for all props
    updateIsTransforming();

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

    if (checkCollisionWithAll(before,after))
    {
        // There is a collision, so reset the position to the last position.
        xpos = oldxpos;
        zpos = oldzpos;
        return;
    }
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
            delta.x = diff.x * PROP_TRANSLATE_STEP;
            delta.y = diff.y * PROP_TRANSLATE_STEP;
            delta.z = diff.z * PROP_TRANSLATE_STEP;

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
            if (checkCollisionWithAll(before,after))
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
        else
        {
            //Update prop's position with physics (velocity)
            applyPhysics(o);
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

        // If the prop is not near the goal
        if (!curRotation.equals(goal))
        {
            // Get the direction to move
            MyPoint diff = goal.minus(curRotation);
            diff.normalize();

            // Rotate towards the goal
            curRotation.x += diff.x * PROP_ROTATE_STEP;
            curRotation.y += diff.y * PROP_ROTATE_STEP;
            curRotation.z += diff.z * PROP_ROTATE_STEP;
        }
        else
        {
            //Update prop's position with physics
            applyPhysics(o);
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
            for (unsigned int j=0; j<o.nPoints; j++)
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
            if (!checkCollisionWithAll(before,after))
            {
                // Update the scale
                curScale.x = 1;
                curScale.y = 1;
                curScale.z = 1;

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
        else
        {
            //Update prop's position with physics
            applyPhysics(o);
        }
    }
}

void OriginWindow::applyPhysics(MyObject& o)
{
    // Don't apply physics to transforming objects
    if (o.isTransforming)
        return;

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

    // Check for collision
    if (checkCollisionWithAll(before,after))
    {
        o.velocity.x = -delta.x * 0.4f;
        o.velocity.y = -delta.y * 0.4f;
        o.velocity.z = -delta.z * 0.4f;
    }
    else
    {
        MyPoint& curPosition = o.position;

        // Adjust position based on velocity
        curPosition.x += o.velocity.x;
        curPosition.y += o.velocity.y;
        curPosition.z += o.velocity.z;

        // reduce objects velocity (friction)
        o.velocity.times(0.8f);
    }
}

void OriginWindow::updateIsTransforming()
{
    for(int i=0; i<scene.propcount; i++)
    {
        // Create a reference to the prop.
        MyObject& o = scene.prop[i];

        // Don't worry about non-transforming objects
        if (!o.isTransforming)
            continue;

        // The object is transforming if it is not at it's goal
        o.isTransforming = !(o.goalPosition.equals(o.position) && o.goalRotation.equals(o.rotation) && o.goalScale.equals(o.scale));
    }
}



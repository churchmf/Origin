#include "originwindow.h"

bool OriginWindow::checkCollisionWithAll(QList<MyPoint> before, QList<MyPoint> after)
{
    // Assume the same number of points before and after the move
    int numPoints = before.size();

    // Check if there is a collision between any of the points and any objects
    for (int i=0; i < numPoints; i++)
    {
        MyPoint linePoint0 = before.at(i);
        MyPoint linePoint1 = after.at(i);

        // Check for collisions with each prop in the scene.
        for(int j=0; j<scene.propcount; j++)
        {
            MyObject& prop = scene.prop[j];

            // Check for collisions with each triangular plane of the prop.
            for(unsigned int k=0; k<prop.nPlanes; k++)
            {
                // Get the triangular plane.
                MyPlane plane = prop.planes[k];

                // Get the object's position.
                MyPoint propPos = prop.position;

                // Get points on the triangular plane.
                MyPoint p1 = prop.points[plane.pids[0]].plus(propPos);
                MyPoint p2 = prop.points[plane.pids[1]].plus(propPos);
                MyPoint p3 = prop.points[plane.pids[2]].plus(propPos);

                // Compute the point of intersection.
                MyPoint intersectionPoint;

                // Check that the intersection point is within the triangular plane:
                if(lineTriangleCollision(p1, p2, p3, linePoint0, linePoint1, intersectionPoint))
                {
                    return true;
                }
            }
        }

        // Check for collisions with each object in the scene.
        for(int j=0; j<scene.objcount; j++)
        {
            // Get the Object.
            MyObject& sceneObject = scene.obj[j];

            // Check for collisions with each triangular plane of the object.
            for(unsigned int k=0; k<sceneObject.nPlanes; k++)
            {
                // Get the triangular plane.
                MyPlane plane = sceneObject.planes[k];

                // Get the sceneObject's position.
                MyPoint objectPos = sceneObject.position;

                // Get points on the triangular plane.
                MyPoint p1 = sceneObject.points[plane.pids[0]].plus(objectPos);
                MyPoint p2 = sceneObject.points[plane.pids[1]].plus(objectPos);
                MyPoint p3 = sceneObject.points[plane.pids[2]].plus(objectPos);

                // Compute the point of intersection.
                MyPoint intersectionPoint;

                // Check that the intersection point is within the boundaries of the triangular plane:
                if(lineTriangleCollision(p1, p2, p3, linePoint0, linePoint1, intersectionPoint))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool OriginWindow::lineTriangleCollision(MyPoint& TP1, MyPoint& TP2, MyPoint& TP3, MyPoint& LP1, MyPoint& LP2, MyPoint& HitPos)
{
    MyPoint normal, IntersectPos;

    // Find Triangle Normal
    normal = TP2.minus(TP1).cross(TP3.minus(TP1));
    normal.normalize(); // not really needed

    // Find distance from LP1 and LP2 to the plane defined by the triangle
    float Dist1 = (LP1.minus(TP1)).dot( normal );
    float Dist2 = (LP2.minus(TP1)).dot( normal );

    if ( (Dist1 * Dist2) >= 0.0f) return false;  // line doesn't cross the triangle.
    if ( Dist1 == Dist2) return false;// line and plane are parallel

    // Find point on the line that intersects with the plane
    IntersectPos = LP1.plus(LP2.minus(LP1).times(-Dist1/(Dist2-Dist1)));

    // Find if the intersection point lies inside the triangle by testing it against all edges
    MyPoint vTest;
    vTest = normal.cross(TP2.minus(TP1));
    if(vTest.dot(IntersectPos.minus(TP1)) < 0.0f) return false;
    vTest = normal.cross(TP3.minus(TP2));
    if(vTest.dot(IntersectPos.minus(TP2)) < 0.0f) return false;
    vTest = normal.cross(TP1.minus(TP3));
    if(vTest.dot(IntersectPos.minus(TP1)) < 0.0f) return false;

    HitPos = IntersectPos;
    return true;
}

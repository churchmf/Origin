#include "originwindow.h"

//::::FORMATING OF LEVEL FILES:::
//NUMBER_OF_OBJECTS
//NUMBER_OF_PROPS
//OBJECTS_BY_RELATIVE_FILE_NAME
//PROPS_BY_RELATIVE_FILE_NAME
/*example:
    1
    0
    objects/room.obj
 */
void OriginWindow::loadLevel()
{
    // Get the fileName from a file picker.
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Level"), "", "");

    // Will be "" if the user canceled the dialog.
    if(fileName == "")
    {
        printf("Dialog window was canceled.\n");
        return;
    }
    // Create the file.
    QFile file(fileName);

    // Stop if the file cannot be opened in ReadOnly or Text mode.
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        printf("Couldn't open the file.\n");
        if(!file.exists())
            printf("File doesn't exist.\n");
        return;
    }

    // Create the input stream.
    QTextStream in(&file);

    // Get the number of objects in the scene. 0 if not an int.
    in >> scene.objcount;

    // Get the number of objects in the scene. 0 if not an int.
    in >> scene.propcount;

    // For each object stored in the file,
    for(int i=0; i<scene.objcount; i++)
    {
        QString objLocation;
        // Create a reference to the object.
        MyObject& o  = scene.obj[i];
        in >> objLocation;

        QFileInfo info(file);
        QString objPath = info.absoluteDir().absolutePath() + "/" + objLocation;
        QFile objFile(objPath);
        if(objFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            this->readObject(&objFile, o);
        }
    }

    // For each prop stored in the file,
    for(int i=0; i<scene.propcount; i++)
    {
        QString objLocation;
        // Create a reference to the object.
        MyObject& o  = scene.prop[i];
        in >> objLocation;

        QFileInfo info(file);
        QString objPath = info.absoluteDir().absolutePath() + "/" + objLocation;
        QFile objFile(objPath);
        if(objFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            this->readObject(&objFile, o);
        }
    }
}

void OriginWindow::readMaterial(QFile* file, MyObject &o)
{
    QTextStream in(file);
    QString line = in.readLine();
    while (line != NULL)
    {
        if (line.startsWith("Ka"))
        {
            QStringList colour = line.split(" ");
            o.material.ka.setRed(((QString)colour[1]).toInt());
            o.material.ka.setGreen(((QString)colour[2]).toInt());
            o.material.ka.setBlue(((QString)colour[3]).toInt());
        }
        else if (line.startsWith("Kd"))
        {
            QStringList colour = line.split(" ");
            o.material.kd.setRed(((QString)colour[1]).toFloat());
            o.material.kd.setGreen(((QString)colour[2]).toFloat());
            o.material.kd.setBlue(((QString)colour[3]).toFloat());
        }
        else if (line.startsWith("Ks"))
        {
            QStringList colour = line.split(" ");
            o.material.ks.setRed(((QString)colour[1]).toInt());
            o.material.ks.setGreen(((QString)colour[2]).toInt());
            o.material.ks.setBlue(((QString)colour[3]).toInt());
        }
        else if (line.startsWith("d") || line.startsWith("Tr"))
        {
            QStringList alpha = line.split(" ");
            o.material.alpha = ((QString)alpha[1]).toFloat();
        }
        else if (line.startsWith("Ns"))
        {
            QStringList shininess = line.split(" ");
            o.material.s = ((QString)shininess[1]).toFloat();
        }
        else if (line.startsWith("map_Kd"))
        {
            QStringList texture = line.split(" ");
            QString textureLocation = "../images/" + ((QString)texture[1]);

            //set the object's material's texture to texture we are loading in
            o.material.texture = this->readTexture(textureLocation);
        }
        line = in.readLine();
    }
}

//This does all of the .obj file reading. Currently supports reading in of normals, vertices, and faces
void OriginWindow::readObject(QFile* file, MyObject& o)
{
    QTextStream in(file);
    QString line = in.readLine();
    int pointNum = 0;
    int textureNum = 0;
    int planeNum = 0;
    int normalNum = 0;

    while (line != NULL)
    {
        if (line.startsWith("mtllib"))
        {
            QStringList material = line.split(" ");
            QFileInfo info(*file);
            QString matPath = info.absoluteDir().absolutePath() + "/" + material[1];
            QFile materialFile(matPath);
            if(materialFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                this->readMaterial(&materialFile, o);
            }
        }
        //normals
        else if (line.startsWith("vn"))
        {
            QStringList normal = line.split(" ");
            o.normals[normalNum].x = ((QString)normal[1]).toFloat();
            o.normals[normalNum].y = ((QString)normal[2]).toFloat();
            o.normals[normalNum].z = ((QString)normal[3]).toFloat();
            normalNum++;
        }
        //texture coordinates
        else if (line.startsWith("vt"))
        {
            QStringList textureCoord = line.split(" ");
            o.textureCoords[textureNum].u = ((QString)textureCoord[1]).toFloat();
            o.textureCoords[textureNum].v = ((QString)textureCoord[2]).toFloat();
            textureNum++;
        }
        //points
        else if (line.startsWith("v"))
        {
            QStringList point = line.split(" ");
            o.points[pointNum].x = ((QString)point[1]).toFloat();
            o.points[pointNum].y = ((QString)point[2]).toFloat();
            o.points[pointNum].z = ((QString)point[3]).toFloat();
            pointNum++;
        }
        //planes
        else if (line.startsWith("f"))
        {
            //get the 3 components of the face (vertex/texture/normal) indices
            QStringList plane = line.split(" ");
            int planePointNum = 0;
            MyPlane& aPlane = o.planes[planeNum];
            for (int i=1; i<plane.length();i++)
            {
                //isolate the vertices of the face
                QStringList planePoints = ((QString)plane[i]).split("/");
                QString thePoint = (QString)planePoints[0];
                QString theTextureCoord = (QString)planePoints[1];
                QString theNormal = (QString)planePoints[2];

                if (!thePoint.isEmpty())
                {
                    //shift down one because they aren't zero indexed in the obj file...
                    aPlane.pids[planePointNum] = thePoint.toInt()-1;
                }
                if (!theTextureCoord.isEmpty() && !thePoint.isEmpty())
                {
                    //shift down one because they aren't zero indexed in the obj file...
                    aPlane.tids[planePointNum] = theTextureCoord.toInt()-1;
                }
                if (!theNormal.isEmpty())
                {
                    //shift down one because they aren't zero indexed in the obj file...
                    aPlane.nids[planePointNum] = theNormal.toInt()-1;
                }
                planePointNum++;
            }
            aPlane.nPoints = planePointNum;
            planeNum++;
        }

        line = in.readLine();
    }
    o.nPlanes = planeNum;
    o.nPoints = pointNum;

    //initialize object's position
    if (pointNum > 0)
    {
        // Take the average of all the points for the objects position (center)
        MyPoint center;
        for (int i=0; i<pointNum; i++)
        {
            center = center.plus(o.points[i]);
        }
        o.position = center.divide(pointNum);
        o.goalPosition = o.position;
    }
}

GLuint& OriginWindow::readTexture(QString textureLocation)
{
    QImage t;
    QImage b;

    if ( !b.load( textureLocation ) )
    {
        b = QImage( 16, 16, QImage::Format_Mono);
        b.fill( Qt::green );
    }

    t = QGLWidget::convertToGLFormat( b );

    glBindTexture( GL_TEXTURE_2D, texture[textureCount] );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D( GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );

    textureCount++;
    return texture[textureCount-1];
}

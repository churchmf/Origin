#include "originwindow.h"

//::::FORMATING OF LEVEL FILES:::
//NUMBER_OF_OBJECTS
//NUMBER_OF_PROPS
//OBJECTS_BY_RELATIVE_FILE_NAME
//PROPS_BY_RELATIVE_FILE_NAME
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
            o.textureCoords[textureNum].x = ((QString)textureCoord[1]).toFloat();
            o.textureCoords[textureNum].y = ((QString)textureCoord[2]).toFloat();
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
                ((QString)planePoints[1]).toInt();
                ((QString)planePoints[2]).toInt()-1;
                if (!thePoint.isEmpty())
                {
                    //shift down one because they aren't zero indexed in the obj file...
                    aPlane.pids[planePointNum] = thePoint.toInt()-1;
                }
                if (!theTextureCoord.isEmpty() && !thePoint.isEmpty())
                {
                    //textureCoords
                    o.points[thePoint.toInt()-1].u = o.textureCoords[theTextureCoord.toInt()].x;
                    o.points[thePoint.toInt()-1].v = o.textureCoords[theTextureCoord.toInt()].y;
                }
                if (!theNormal.isEmpty())
                {
                    //normal
                    aPlane.normal = o.normals[theNormal.toInt()];
                }
                //TODO Read colour information/texture information
                aPlane.color.x = 1;
                planePointNum++;
            }
            aPlane.nPoints = planePointNum;
            planeNum++;
        }

        line = in.readLine();
    }
    o.nPlanes = planeNum;
    o.nPoints = pointNum;

    // Set the object as casting a shadow.
    o.castsShadow = 1;

    // Compute the connectivity of the edges of the object.
    o.setConnectivity();
}

void OriginWindow::loadGLTextures()
{
    QImage t;
    QImage b;

    if ( !b.load( "../images/brick.bmp" ) )
    {
        b = QImage( 16, 16, QImage::Format_Mono);
        b.fill( Qt::green );
    }

    t = QGLWidget::convertToGLFormat( b );
    glGenTextures( 4, &texture[0] );

    glBindTexture( GL_TEXTURE_2D, texture[0] );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D( GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );

    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexImage2D( GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );

    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, t.width(), t.height(), GL_RGBA, GL_UNSIGNED_BYTE, t.bits());

    loadCrosshairTexture();
    loadAxisTexture();
}

void OriginWindow::loadCrosshairTexture()
{
    QImage t;
    QImage b;

    //Crosshair texture
    if ( !b.load( "../images/crosshair.bmp" ) )
    {
        b = QImage( 16, 16, QImage::Format_Mono);
        b.fill( Qt::green );
    }

    t = QGLWidget::convertToGLFormat( b );
    glBindTexture( GL_TEXTURE_2D, texture[3] );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D( GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );

}

void OriginWindow::loadAxisTexture()
{
    QImage t;
    QImage b;

    //Axis texture
    if ( !b.load( "../images/axis.bmp" ) )
    {
        b = QImage( 16, 16, QImage::Format_Mono);
        b.fill( Qt::green );
    }

    t = QGLWidget::convertToGLFormat( b );
    glBindTexture( GL_TEXTURE_2D, texture[4] );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D( GL_TEXTURE_2D, 0, 3, t.width(), t.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, t.bits() );

}

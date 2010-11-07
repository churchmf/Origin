#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QDialog (parent)
{
        myWin = new CSWindow(parent);
        //animateButton = new QPushButton(tr("animate"));
        quitButton = new QPushButton(tr("quit"));
        //selFace = new QSpinBox(this);
        //selFace->setRange(0,NUMBEROFFACES-2);
        //faceLabel = new QLabel(tr("face"));
        //rotSlider = new QSlider(Qt::Horizontal,this);
        //rotSlider->setRange(0,ANGLETOROTATE);
        //rotSlider->setMinimum(0);
        //rotSlider->setMaximum(ANGLETOROTATE);
        //connect(animateButton,SIGNAL(clicked()),myWin,SLOT(animate()));
        connect(quitButton,SIGNAL(clicked()),qApp,SLOT(quit()));
        //connect(selFace,SIGNAL(valueChanged(int)),myWin, SLOT(getFaceToFold(int)));
        //connect(myWin,SIGNAL(setSlider(int)), rotSlider,SLOT(setValue(int)));
        //connect(rotSlider,SIGNAL(valueChanged(int)),myWin,SLOT(rotateFace(int)));
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(myWin);
        QHBoxLayout *bLayout = new QHBoxLayout;
        bLayout->addStretch();
        //bLayout->addWidget(faceLabel);
        //bLayout->addWidget(selFace);
        //bLayout->addWidget(rotSlider);
        //bLayout->addWidget(animateButton);
        bLayout->addWidget(quitButton);
        layout->addLayout(bLayout);
        setGeometry(myWin->geometry());
        _title="CMPUT 411 Project";
        setWindowTitle(tr(_title));
        setLayout(layout);
}

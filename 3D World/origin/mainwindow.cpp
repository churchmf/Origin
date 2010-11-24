#include "mainwindow.h"
//#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QDialog (parent)//QMainWindow(parent) ,  ui(new Ui::MainWindow)
{
   // ui->setupUi(this);
    //create and arrange ui components
    myWin = new OriginWindow(parent);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(myWin);

    setGeometry(myWin->geometry());

    _title="3D World Test";
    setWindowTitle(tr(_title));
    setLayout(layout);
}

//MainWindow::~MainWindow()
//{
    //delete ui;
//}

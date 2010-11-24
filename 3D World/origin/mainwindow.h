#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <QMainWindow>
#include "originwindow.h"

//namespace Ui {
  //  class MainWindow;
//}

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
private:
    char *_title;
    //Ui::MainWindow *ui;
    OriginWindow *myWin;
};

#endif // MAINWINDOW_H

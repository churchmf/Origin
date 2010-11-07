#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "cswindow.h"
#include <QSpinBox>
#include <QSlider>
class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
private:
//        QPushButton *animateButton;
        QPushButton *quitButton;
        CSWindow *myWin;
//        QSpinBox *selFace;
//        QLabel *faceLabel;
//        QSlider *rotSlider;
        char *_title;
};

#endif // MAINWINDOW_H

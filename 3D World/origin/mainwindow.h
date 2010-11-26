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
    enum TransformationType { Translate, Scale };
    enum RotationAxis { RotateByX, RotateByY, RotateByZ };
    MainWindow(QWidget *parent = 0);

private:
    char *_title;
    //Ui::MainWindow *ui;
    OriginWindow *myWin;

    // Widgets
    QPushButton* rotationButton;
    QSpinBox* rotationAnglePicker;
    QComboBox* rotationAxisSelector;
    QPushButton* transformationButton;
    QComboBox* transformationSelector;
    QTableWidget* transformationTable;
};

#endif // MAINWINDOW_H

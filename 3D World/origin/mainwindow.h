#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "originwindow.h"

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);

private:
    char *_title;
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

#include "mainwindow.h"
const int COLUMN_COUNT = 3;

MainWindow::MainWindow(QWidget *parent) : QDialog (parent)
{
    resize(800, 600);
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);

    //vertical layout
    QVBoxLayout* verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing(2);
    verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    verticalLayout->setContentsMargins(-1, -1, -1, 0);

    //setup transformationSelector
    transformationSelector = new QComboBox();
    sizePolicy1.setHeightForWidth(transformationSelector->sizePolicy().hasHeightForWidth());
    transformationSelector->addItem("Translate", Translate);
    transformationSelector->addItem("Scale", Scale);
    transformationSelector->setSizePolicy(sizePolicy1);

    //setup rotationAxisSelector
    rotationAxisSelector = new QComboBox();
    sizePolicy1.setHeightForWidth(rotationAxisSelector->sizePolicy().hasHeightForWidth());
    rotationAxisSelector->addItem("Rotate by X", RotateByX);
    rotationAxisSelector->addItem("Rotate by Y", RotateByY);
    rotationAxisSelector->addItem("Rotate by Z", RotateByZ);
    rotationAxisSelector->setSizePolicy(sizePolicy1);

    //setup transformationTable
    transformationTable = new QTableWidget(1,COLUMN_COUNT);
    QStringList headers;
    headers << "X" << "Y" << "Z";
    transformationTable->setHorizontalHeaderLabels(headers);
    transformationTable->setVerticalHeaderLabels(QStringList(""));

    //setup headers
    QColor red = QColor(255,0,0,200);
    QColor green = QColor(0,255,0,200);
    QColor blue = QColor(0,0,255,200);
    QTableWidgetItem* item1 = new QTableWidgetItem("0",0);
    item1->setBackgroundColor(red);
    QTableWidgetItem* item2 = new QTableWidgetItem("0",0);
    item2->setBackgroundColor(green);
    QTableWidgetItem* item3 = new QTableWidgetItem("0",0);
    item3->setBackgroundColor(blue);
    transformationTable->setItem(0,0, item1);
    transformationTable->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
    transformationTable->setItem(0,1, item2);
    transformationTable->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
    transformationTable->setItem(0,2, item3);
    transformationTable->horizontalHeader()->setResizeMode(2,QHeaderView::Stretch);

    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Ignored);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(transformationTable->sizePolicy().hasHeightForWidth());

    transformationTable->setSizePolicy(sizePolicy2);
    transformationTable->setMinimumSize(QSize(0, 50));
    transformationTable->viewport()->setProperty("cursor", QVariant(QCursor(Qt::IBeamCursor)));
    transformationTable->setFrameShape(QFrame::StyledPanel);
    transformationTable->setFrameShadow(QFrame::Sunken);
    transformationTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    transformationTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    transformationTable->setCornerButtonEnabled(false);
    transformationTable->setSelectionMode(QAbstractItemView::NoSelection);
    transformationTable->horizontalHeader()->setHighlightSections(false);
    transformationTable->horizontalHeader()->setStretchLastSection(true);
    transformationTable->verticalHeader()->setVisible(false);
    transformationTable->verticalHeader()->setHighlightSections(false);
    transformationTable->verticalHeader()->setMinimumSectionSize(19);
    transformationTable->verticalHeader()->setStretchLastSection(true);

    //setup rotationAnglePicker
    rotationAnglePicker = new QSpinBox();
    sizePolicy1.setHeightForWidth(rotationAnglePicker->sizePolicy().hasHeightForWidth());
    rotationAnglePicker->setSizePolicy(sizePolicy1);
    rotationAnglePicker->setMaximum(360);
    rotationAnglePicker->setMinimum(-360);


    //setup transformationButton
    transformationButton = new QPushButton(tr("Apply Transformation"));
    sizePolicy1.setHeightForWidth(transformationButton->sizePolicy().hasHeightForWidth());
    transformationButton->setSizePolicy(sizePolicy1);

    //setup rotationButton
    rotationButton = new QPushButton(tr("Apply Rotation"));
    sizePolicy1.setHeightForWidth(rotationButton->sizePolicy().hasHeightForWidth());
    rotationButton->setSizePolicy(sizePolicy1);

    //create originwindow (pass in widgets for parsing)
    myWin = new OriginWindow(transformationSelector, transformationTable, rotationAxisSelector, rotationAnglePicker, parent);
    sizePolicy1.setHeightForWidth(myWin->sizePolicy().hasHeightForWidth());
    myWin->setSizePolicy(sizePolicy1);

    //horizontal layout
    QHBoxLayout* horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(0);
    horizontalLayout->setSizeConstraint(QLayout::SetMinimumSize);
    horizontalLayout->setContentsMargins(0, 0, -1, 0);

    verticalLayout->addWidget(myWin);
    verticalLayout->addLayout(horizontalLayout);
    verticalLayout->setStretch(0, 1);

    horizontalLayout->addWidget(transformationSelector);
    horizontalLayout->addWidget(transformationTable);
    horizontalLayout->addWidget(transformationButton);
    QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);
    horizontalLayout->addWidget(rotationAxisSelector);
    horizontalLayout->addWidget(rotationAnglePicker);
    horizontalLayout->addWidget(rotationButton);

    //Setup button listeners
    connect(transformationButton,SIGNAL(clicked()),myWin,SLOT(applyTransformation()));
    connect(rotationButton,SIGNAL(clicked()),myWin,SLOT(applyRotation()));

    //setGeometry(myWin->geometry());
    _title="Origin";
    setWindowTitle(tr(_title));
    setLayout(verticalLayout);
}

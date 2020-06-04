#include "mainwindow.h"

#include <QWidget>
#include <QImage>
#include <QFileDialog>
#include <QPixmap>
#include <QAction>
#include <QMessageBox>
#include <QDebug>
#include <QScrollArea>
#include <QGridLayout>
#include <QErrorMessage>
#include <QApplication>
#include <QLabel>  // display static text, or image
#include <QPushButton>
#include <QLineEdit>  // single line
#include <QComboBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QTextEdit>  // multi lines, rich text
#include <QTextBrowser>  //readonly text edit
#include <QGroupBox>
#include <QSlider>  // 模拟显示数字
#include <QSpinBox>  // 数字
#include <QDateEdit>
#include <QTimeEdit>
#include <QDateTimeEdit>
#include <QTabWidget>  // 不算控件
#include <QVBoxLayout>
#include <QCompleter>
#include <QHBoxLayout>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{

    /* init main window */
    initMainWindow();

    /* init ui */
    initUiComponent();

    /* init resource */
    initImageResource();

    /* create imageViewer */
    imageViewer = new QImageViewer();


}
void MainWindow::slotComBoxIndexChanged(const QString &str){
    qDebug() << str;
}
void MainWindow::initImageResource(void)
{
    imageLabel1->clear();
    imageLabel1->resize(QSize(200, 100));
    setWindowTitle(tr("SiftGUI"));
}

void MainWindow::loadImageResource(void)
{
    imageLabel1->setPixmap(imageViewer->pixmap);
    imageLabel1->resize(imageViewer->size);
    setWindowTitle(QFileInfo(imageViewer->filename).fileName() + tr(" - QImageViewer"));
}

void MainWindow::openActionTriggered(void)
{
    int ret = imageViewer->openImageFile(tr("Select image:"),
                                         "D:\\Documents\\Pictures",
                                         tr("Images (*.jpg *.jpeg *.png *.bmp *.gif)"));
    if (ret) {
        QMessageBox::information(this, tr("Error"), tr("Open a file failed!"));
        return ;
    }

    loadImageResource();
}

void MainWindow::closeActionTriggered(void)
{
    initImageResource();
    imageViewer->closeImageFile();
}

void MainWindow::lastActionTriggered(void)
{
    int ret = imageViewer->last();
    if (ret) {
        QMessageBox::information(this,
                                 tr("Error"),
                                 tr("Open a image, please!"));
        return ;
    }

    loadImageResource();
}

void MainWindow::nextActionTriggered(void)
{
    int ret = imageViewer->next();
    if (ret) {
        QMessageBox::information(this,
                                 tr("Error"),
                                 tr("Open a image, please!"));
        return ;
    }

    loadImageResource();
}

void MainWindow::toLeftActionTriggered(void)
{
    int ret = imageViewer->spinToLeft();
    if (ret) {
        QMessageBox::information(this,
                                 tr("Error"),
                                 tr("Open a image, please!"));
        return ;
    }

    loadImageResource();
}

void MainWindow::toRightActionTriggered(void)
{
    int ret = imageViewer->spinToRight();
    if (ret) {
        QMessageBox::information(this,
                                 tr("Error"),
                                 tr("Open a image, please!"));
        return ;
    }

    loadImageResource();
}

void MainWindow::toEnlargeActionTriggered(void)
{
    int ret = imageViewer->zoomIn();
    if (ret) {
        QMessageBox::information(this,
                                 tr("Error"),
                                 tr("Open a image, please!"));
        return ;
    }

    loadImageResource();
}

void MainWindow::toLessenActionTriggered(void)
{
    int ret = imageViewer->zoomOut();
    if (ret) {
        QMessageBox::information(this,
                                 tr("Error"),
                                 tr("Open a image, please!"));
        return ;
    }

    loadImageResource();
}

void MainWindow::deleteActionTriggered(void)
{
    if (!QFile(imageViewer->filename).exists()) {
        QMessageBox::information(this,
                                 tr("Error"),
                                 tr("Open a image, please!"));
        return ;
    }

    QMessageBox message(QMessageBox::Warning,
                        tr("Warning"),
                        tr("Do you want to delete this image?"),
                        QMessageBox::Yes|QMessageBox::No,
                        NULL);
    if (message.exec() == QMessageBox::No) {
        return ;
    }

    int ret = imageViewer->delImageFile();
    if (ret) {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Delete a image failed!"));
        return ;
    }

    initImageResource();
}


void MainWindow::setQImageViewerWidget(void)
{
    /* label show image */
    rootPath += "D:/Code/cpp/homework_cv/SiftHuster/";
    imageLabel1 = new QLabel();
    imageLabel2 = new QLabel();
    QPushButton* buttonOn = new QPushButton("run");
    QPushButton* buttonOff = new QPushButton("exit");
    button1 = new QPushButton("mosaic");
    button2 = new QPushButton("DOGfeature");
    button3 = new QPushButton("keypoint");
    button4 = new QPushButton("diriction");


    connect(buttonOn, &QPushButton::clicked, [this](){
        program(imageViewer->filename.toStdString().c_str());
        QMessageBox::information(NULL, "process", "sift process is finished!", QMessageBox::Ok);

    });

    connect(buttonOff, &QPushButton::clicked, [this](){
        if (!(QMessageBox::information(this,tr("exit tip"),tr("Do you really want exit ?"),tr("Yes"),tr("No"))))
        {
            QApplication* app;
            app->exit(0);
        }

    });

    connect(button1, &QPushButton::clicked, [this](){
        QImage image(rootPath + "results/mosaic1.jpg");
        imageLabel2->setPixmap(QPixmap::fromImage(image));
        imageLabel2->resize(image.size());

    });

    connect(button2, &QPushButton::clicked, [this](){
        QImage image(rootPath + "results/DOGPyramid.jpg");
        imageLabel2->setPixmap(QPixmap::fromImage(image));
        imageLabel2->resize(image.size());
    });

    connect(button3, &QPushButton::clicked, [this](){
        QImage image(rootPath + "results/keypoint.jpg");
        imageLabel2->setPixmap(QPixmap::fromImage(image));
        imageLabel2->resize(image.size());
    });

    connect(button4, &QPushButton::clicked, [this](){
        QImage image(rootPath + "results/direction.jpg");
        imageLabel2->setPixmap(QPixmap::fromImage(image));
        imageLabel2->resize(image.size());
    });

    QScrollArea *imageScrollArea1 = new QScrollArea();
    imageScrollArea1->setAlignment(Qt::AlignCenter);
    imageScrollArea1->setFrameShape(QFrame::NoFrame);
    imageScrollArea1->setWidget(imageLabel1);

    QScrollArea *imageScrollArea2 = new QScrollArea();
    imageScrollArea2->setAlignment(Qt::AlignCenter);
    imageScrollArea2->setFrameShape(QFrame::NoFrame);
    imageScrollArea2->setWidget(imageLabel2);

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addWidget(imageScrollArea1, 0, 0);
    mainLayout->addWidget(imageScrollArea2, 0, 2);

//    QWidget *verticalLineWidget = new QWidget;
//    verticalLineWidget->setFixedWidth(5);
//    verticalLineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
//    verticalLineWidget->setStyleSheet(QString("background-color: #c0c0c0;"));
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::VLine); // Vertical line
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(5);
//    QLayout* layLine = new QHBoxLayout();
//    layLine->addWidget(verticalLineWidget);
//    QSpacerItem *spacerItem = new QSpacerItem(2, this->size().height(), QSizePolicy::Expanding);
//    mainLayout->addItem(spacerItem, 0, 1, 1, 1);
    mainLayout->addWidget(line, 0, 1, 2, 1);
    QLayout* lay = new QHBoxLayout();
    lay->addWidget(button1);
    lay->addWidget(button2);
    lay->addWidget(button3);
    lay->addWidget(button4);
    mainLayout->addLayout(lay, 1, 2);

    QLayout* lay2 = new QHBoxLayout();
    lay2->addWidget(buttonOn);
    lay2->addWidget(buttonOff);
    mainLayout->addLayout(lay2, 1, 0);

    centralWidget->setLayout(mainLayout);

}

void MainWindow::setWindowComponet(void)
{
    openAction = new QAction(tr("Open"), this);
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip(tr("Open a image."));
    openAction->setIcon(QIcon(":/images/open.png"));

    closeAction = new QAction(tr("Close"), this);
    closeAction->setShortcut(QKeySequence::Close);
    closeAction->setStatusTip(tr("Close a image."));
    closeAction->setIcon(QIcon(":/images/close.png"));

    lastAction = new QAction(tr("Last"), this);
    lastAction->setStatusTip(tr("Last image."));
    lastAction->setIcon(QIcon(":/images/left.png"));

    nextAction = new QAction(tr("Next"), this);
    nextAction->setStatusTip(tr("Next image"));
    nextAction->setIcon(QIcon(":/images/right.png"));

    toLeftAction = new QAction(tr("LeftSpin"), this);
    toLeftAction->setStatusTip(tr("To Left."));
    toLeftAction->setIcon(QIcon(":/images/toLeft.png"));

    toRightAction = new QAction(tr("RightSpin"), this);
    toRightAction->setStatusTip(tr("To Right."));
    toRightAction->setIcon(QIcon(":/images/toRight.png"));

    toEnlargeAction = new QAction(tr("Enlarge"), this);
    toEnlargeAction->setStatusTip(tr("To Enlarge."));
    toEnlargeAction->setIcon(QIcon(":/images/large.png"));

    toLessenAction = new QAction(tr("Lessen"), this);
    toLessenAction->setStatusTip(tr("To Lessen."));
    toLessenAction->setIcon(QIcon(":/images/small.png"));

    deleteAction = new QAction(tr("Delete"), this);
    deleteAction->setStatusTip(tr("Delete a image"));
    deleteAction->setIcon(QIcon(":/images/clear.png"));
    deleteAction->setShortcut(QKeySequence::Delete);

    QAction *exitAction = new QAction(tr("Exit"), this);
    exitAction->setStatusTip(tr("Exit"));
    exitAction->setIcon(QIcon(":/images/quit.png"));
    exitAction->setShortcut(QKeySequence::Quit);

    QAction *aboutQt = new QAction(tr("About Qt"), this);
    aboutQt->setStatusTip(tr("About Qt"));
    aboutQt->setIcon(QIcon(":/images/Qt.png"));

    QAction *about = new QAction(tr("About SiftGUI"), this);
    about->setStatusTip(tr("About SiftGUI"));
    about->setIcon(QIcon(":/images/help.png"));

    QMenu *fileMenu = menuBar->addMenu(tr("File"));
    fileMenu->addAction(openAction);
    fileMenu->addAction(closeAction);
    fileMenu->addSeparator();
    fileMenu->addAction(deleteAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    QMenu *operationMenu = menuBar->addMenu(tr("Operate"));
    operationMenu->addAction(lastAction);
    operationMenu->addAction(nextAction);
    operationMenu->addSeparator();
    operationMenu->addAction(toLeftAction);
    operationMenu->addAction(toRightAction);
    operationMenu->addSeparator();
    operationMenu->addAction(toEnlargeAction);
    operationMenu->addAction(toLessenAction);

    QMenu *helpMenu = menuBar->addMenu(tr("Help"));
    helpMenu->addAction(aboutQt);
    helpMenu->addAction(about);

    toolBar->addAction(openAction);
    toolBar->addAction(closeAction);
    toolBar->addAction(lastAction);
    toolBar->addAction(nextAction);
    toolBar->addAction(toLeftAction);
    toolBar->addAction(toRightAction);
    toolBar->addAction(toEnlargeAction);
    toolBar->addAction(toLessenAction);
    toolBar->addAction(deleteAction);
    toolBar->addAction(about);

    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openActionTriggered()));
    connect(closeAction, SIGNAL(triggered(bool)), this, SLOT(closeActionTriggered()));
    connect(lastAction, SIGNAL(triggered(bool)), this, SLOT(lastActionTriggered()));
    connect(nextAction, SIGNAL(triggered(bool)), this, SLOT(nextActionTriggered()));
    connect(toLeftAction, SIGNAL(triggered(bool)), this, SLOT(toLeftActionTriggered()));
    connect(toRightAction, SIGNAL(triggered(bool)), this, SLOT(toRightActionTriggered()));
    connect(toEnlargeAction, SIGNAL(triggered(bool)), this, SLOT(toEnlargeActionTriggered()));
    connect(toLessenAction, SIGNAL(triggered(bool)), this, SLOT(toLessenActionTriggered()));
    connect(deleteAction, SIGNAL(triggered(bool)), this, SLOT(deleteActionTriggered()));
    connect(about, SIGNAL(triggered(bool)), this, SLOT(aboutTriggered()));
    connect(aboutQt, SIGNAL(triggered(bool)), this, SLOT(aboutQtTriggered()));
    connect(exitAction, SIGNAL(triggered(bool)), this, SLOT(close()));

}

void MainWindow::aboutQtTriggered(void)
{
    qApp->aboutQt();
}

void MainWindow::aboutTriggered(void)
{
    aboutWidget.setWindowTitle("Help Infomation");
    aboutWidget.setWindowIcon(QIcon(":/images/help.png"));
    aboutWidget.setFixedSize(QABOUT_WIDGET_WIDTH, QABOUT_WIDGET_HEIGHT);
    aboutWidget.show();
}

void MainWindow::initUiComponent(void)
{
    /* set menu and toolbar */
    setWindowComponet();

    /* image show widget */
    setQImageViewerWidget();
}

void MainWindow::initMainWindow(void)
{
    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    toolBar = new QToolBar(this);
    addToolBar(toolBar);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
}


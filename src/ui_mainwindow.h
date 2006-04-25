#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <Qt3Support/Q3Header>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "davetabwidget.h"
#include "Qt3Support/Q3ListViewItemIterator"

class Ui_MainWindow
{
public:
    QAction *actionQuit;
    QAction *actionConfigure;
    QWidget *centralwidget;
    QVBoxLayout *vboxLayout;
    QSplitter *splitter;
    QSplitter *splitter1;
    DaveTabWidget *resultsTabWidget;
    QWidget *tab_2;
    DaveTabWidget *hubTabWidget;
    QWidget *tab;
    QVBoxLayout *vboxLayout1;
    Q3ListView *hubList;
    QHBoxLayout *hboxLayout;
    QPushButton *newHubButton;
    QPushButton *deleteHubButton;
    QSpacerItem *spacerItem;
    QPushButton *connectHubButton;
    QSplitter *splitter2;
    QWidget *widget;
    QVBoxLayout *vboxLayout2;
    QHBoxLayout *hboxLayout1;
    QLabel *label;
    QSpacerItem *spacerItem1;
    QToolButton *userOptionsButton;
    Q3ListView *userList;
    Q3ListView *transferList;
    QMenuBar *menubar;
    QMenu *menuMeep;
    QMenu *menuTools;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
    MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
    MainWindow->resize(QSize(686, 814).expandedTo(MainWindow->minimumSizeHint()));
    actionQuit = new QAction(MainWindow);
    actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
    actionConfigure = new QAction(MainWindow);
    actionConfigure->setObjectName(QString::fromUtf8("actionConfigure"));
    centralwidget = new QWidget(MainWindow);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    vboxLayout = new QVBoxLayout(centralwidget);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    splitter = new QSplitter(centralwidget);
    splitter->setObjectName(QString::fromUtf8("splitter"));
    splitter->setOrientation(Qt::Horizontal);
    splitter1 = new QSplitter(splitter);
    splitter1->setObjectName(QString::fromUtf8("splitter1"));
    splitter1->setOrientation(Qt::Vertical);
    resultsTabWidget = new DaveTabWidget(splitter1);
    resultsTabWidget->setObjectName(QString::fromUtf8("resultsTabWidget"));
    resultsTabWidget->setEnabled(true);
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(13), static_cast<QSizePolicy::Policy>(7));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(10);
    sizePolicy.setHeightForWidth(resultsTabWidget->sizePolicy().hasHeightForWidth());
    resultsTabWidget->setSizePolicy(sizePolicy);
    tab_2 = new QWidget();
    tab_2->setObjectName(QString::fromUtf8("tab_2"));
    resultsTabWidget->addTab(tab_2, QApplication::translate("MainWindow", "Search", 0, QApplication::UnicodeUTF8));
    splitter1->addWidget(resultsTabWidget);
    hubTabWidget = new DaveTabWidget(splitter1);
    hubTabWidget->setObjectName(QString::fromUtf8("hubTabWidget"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(1);
    sizePolicy1.setHeightForWidth(hubTabWidget->sizePolicy().hasHeightForWidth());
    hubTabWidget->setSizePolicy(sizePolicy1);
    hubTabWidget->setMinimumSize(QSize(0, 250));
    tab = new QWidget();
    tab->setObjectName(QString::fromUtf8("tab"));
    vboxLayout1 = new QVBoxLayout(tab);
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(9);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    hubList = new Q3ListView(tab);
    hubList->setObjectName(QString::fromUtf8("hubList"));

    vboxLayout1->addWidget(hubList);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    newHubButton = new QPushButton(tab);
    newHubButton->setObjectName(QString::fromUtf8("newHubButton"));

    hboxLayout->addWidget(newHubButton);

    deleteHubButton = new QPushButton(tab);
    deleteHubButton->setObjectName(QString::fromUtf8("deleteHubButton"));
    deleteHubButton->setEnabled(false);

    hboxLayout->addWidget(deleteHubButton);

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    connectHubButton = new QPushButton(tab);
    connectHubButton->setObjectName(QString::fromUtf8("connectHubButton"));
    connectHubButton->setEnabled(false);

    hboxLayout->addWidget(connectHubButton);


    vboxLayout1->addLayout(hboxLayout);

    hubTabWidget->addTab(tab, QApplication::translate("MainWindow", "Hub Browser", 0, QApplication::UnicodeUTF8));
    splitter1->addWidget(hubTabWidget);
    splitter->addWidget(splitter1);
    splitter2 = new QSplitter(splitter);
    splitter2->setObjectName(QString::fromUtf8("splitter2"));
    splitter2->setOrientation(Qt::Vertical);
    widget = new QWidget(splitter2);
    widget->setObjectName(QString::fromUtf8("widget"));
    vboxLayout2 = new QVBoxLayout(widget);
    vboxLayout2->setSpacing(6);
    vboxLayout2->setMargin(0);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    label = new QLabel(widget);
    label->setObjectName(QString::fromUtf8("label"));

    hboxLayout1->addWidget(label);

    spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem1);

    userOptionsButton = new QToolButton(widget);
    userOptionsButton->setObjectName(QString::fromUtf8("userOptionsButton"));

    hboxLayout1->addWidget(userOptionsButton);


    vboxLayout2->addLayout(hboxLayout1);

    userList = new Q3ListView(widget);
    userList->setObjectName(QString::fromUtf8("userList"));
    userList->setFrameShape(QFrame::StyledPanel);
    userList->setHScrollBarMode(Q3ScrollView::AlwaysOff);
    userList->setDragAutoScroll(false);

    vboxLayout2->addWidget(userList);

    splitter2->addWidget(widget);
    transferList = new Q3ListView(splitter2);
    transferList->setObjectName(QString::fromUtf8("transferList"));
    transferList->setMinimumSize(QSize(0, 150));
    transferList->setHScrollBarMode(Q3ScrollView::AlwaysOff);
    splitter2->addWidget(transferList);
    splitter->addWidget(splitter2);

    vboxLayout->addWidget(splitter);

    MainWindow->setCentralWidget(centralwidget);
    menubar = new QMenuBar(MainWindow);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 686, 28));
    menuMeep = new QMenu(menubar);
    menuMeep->setObjectName(QString::fromUtf8("menuMeep"));
    menuMeep->setGeometry(QRect(320, 307, 119, 46));
    menuTools = new QMenu(menubar);
    menuTools->setObjectName(QString::fromUtf8("menuTools"));
    menuFile = new QMenu(menubar);
    menuFile->setObjectName(QString::fromUtf8("menuFile"));
    MainWindow->setMenuBar(menubar);
    statusbar = new QStatusBar(MainWindow);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    statusbar->setGeometry(QRect(0, 793, 686, 21));
    MainWindow->setStatusBar(statusbar);

    menubar->addAction(menuFile->menuAction());
    menubar->addAction(menuTools->menuAction());
    menuTools->addAction(actionConfigure);
    menuFile->addAction(actionQuit);
    retranslateUi(MainWindow);

    QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
    MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Dave++", 0, QApplication::UnicodeUTF8));
    actionQuit->setText(QApplication::translate("MainWindow", "Quit", 0, QApplication::UnicodeUTF8));
    actionConfigure->setText(QApplication::translate("MainWindow", "Configure Dave++...", 0, QApplication::UnicodeUTF8));
    resultsTabWidget->setTabText(resultsTabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Search", 0, QApplication::UnicodeUTF8));
    newHubButton->setText(QApplication::translate("MainWindow", "New...", 0, QApplication::UnicodeUTF8));
    deleteHubButton->setText(QApplication::translate("MainWindow", "Delete", 0, QApplication::UnicodeUTF8));
    connectHubButton->setText(QApplication::translate("MainWindow", "Connect", 0, QApplication::UnicodeUTF8));
    hubTabWidget->setTabText(hubTabWidget->indexOf(tab), QApplication::translate("MainWindow", "Hub Browser", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("MainWindow", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Users</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    userOptionsButton->setText(QApplication::translate("MainWindow", "Options...", 0, QApplication::UnicodeUTF8));
    menuMeep->setTitle(QApplication::translate("MainWindow", "Meep", 0, QApplication::UnicodeUTF8));
    menuTools->setTitle(QApplication::translate("MainWindow", "Settings", 0, QApplication::UnicodeUTF8));
    menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

#endif // UI_MAINWINDOW_H

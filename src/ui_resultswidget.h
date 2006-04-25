#ifndef UI_RESULTSWIDGET_H
#define UI_RESULTSWIDGET_H

#include <Qt3Support/Q3Header>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "Qt3Support/Q3ListViewItemIterator"

class Ui_UIResultsWidget
{
public:
    QVBoxLayout *vboxLayout;
    QFrame *searchFrame;
    QVBoxLayout *vboxLayout1;
    QHBoxLayout *hboxLayout;
    QLineEdit *searchBox;
    QPushButton *searchButton;
    QPushButton *pushButton;
    QFrame *optionsFrame;
    QHBoxLayout *hboxLayout1;
    QComboBox *dataTypeBox;
    QComboBox *sizeRestrictedBox;
    QSpinBox *sizeBox;
    QComboBox *sizeTypeBox;
    Q3ListView *listView;

    void setupUi(QWidget *UIResultsWidget)
    {
    UIResultsWidget->setObjectName(QString::fromUtf8("UIResultsWidget"));
    UIResultsWidget->resize(QSize(440, 341).expandedTo(UIResultsWidget->minimumSizeHint()));
    vboxLayout = new QVBoxLayout(UIResultsWidget);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    searchFrame = new QFrame(UIResultsWidget);
    searchFrame->setObjectName(QString::fromUtf8("searchFrame"));
    searchFrame->setFrameShape(QFrame::NoFrame);
    vboxLayout1 = new QVBoxLayout(searchFrame);
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(0);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    searchBox = new QLineEdit(searchFrame);
    searchBox->setObjectName(QString::fromUtf8("searchBox"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(1));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(searchBox->sizePolicy().hasHeightForWidth());
    searchBox->setSizePolicy(sizePolicy);

    hboxLayout->addWidget(searchBox);

    searchButton = new QPushButton(searchFrame);
    searchButton->setObjectName(QString::fromUtf8("searchButton"));

    hboxLayout->addWidget(searchButton);

    pushButton = new QPushButton(searchFrame);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));
    pushButton->setCheckable(true);

    hboxLayout->addWidget(pushButton);


    vboxLayout1->addLayout(hboxLayout);

    optionsFrame = new QFrame(searchFrame);
    optionsFrame->setObjectName(QString::fromUtf8("optionsFrame"));
    optionsFrame->setFrameShape(QFrame::NoFrame);
    optionsFrame->setFrameShadow(QFrame::Raised);
    hboxLayout1 = new QHBoxLayout(optionsFrame);
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    dataTypeBox = new QComboBox(optionsFrame);
    dataTypeBox->setObjectName(QString::fromUtf8("dataTypeBox"));

    hboxLayout1->addWidget(dataTypeBox);

    sizeRestrictedBox = new QComboBox(optionsFrame);
    sizeRestrictedBox->setObjectName(QString::fromUtf8("sizeRestrictedBox"));

    hboxLayout1->addWidget(sizeRestrictedBox);

    sizeBox = new QSpinBox(optionsFrame);
    sizeBox->setObjectName(QString::fromUtf8("sizeBox"));
    sizeBox->setEnabled(false);
    sizeBox->setMaximum(65000);

    hboxLayout1->addWidget(sizeBox);

    sizeTypeBox = new QComboBox(optionsFrame);
    sizeTypeBox->setObjectName(QString::fromUtf8("sizeTypeBox"));
    sizeTypeBox->setEnabled(false);

    hboxLayout1->addWidget(sizeTypeBox);


    vboxLayout1->addWidget(optionsFrame);


    vboxLayout->addWidget(searchFrame);

    listView = new Q3ListView(UIResultsWidget);
    listView->setObjectName(QString::fromUtf8("listView"));
    listView->setSelectionMode(Q3ListView::Extended);
    listView->setAllColumnsShowFocus(true);
    listView->setRootIsDecorated(true);

    vboxLayout->addWidget(listView);

    QWidget::setTabOrder(searchBox, searchButton);
    QWidget::setTabOrder(searchButton, pushButton);
    QWidget::setTabOrder(pushButton, dataTypeBox);
    QWidget::setTabOrder(dataTypeBox, sizeRestrictedBox);
    QWidget::setTabOrder(sizeRestrictedBox, sizeBox);
    QWidget::setTabOrder(sizeBox, sizeTypeBox);
    retranslateUi(UIResultsWidget);
    QObject::connect(pushButton, SIGNAL(toggled(bool)), optionsFrame, SLOT(setVisible(bool)));

    sizeTypeBox->setCurrentIndex(2);


    QMetaObject::connectSlotsByName(UIResultsWidget);
    } // setupUi

    void retranslateUi(QWidget *UIResultsWidget)
    {
    UIResultsWidget->setWindowTitle(QApplication::translate("UIResultsWidget", "Form", 0, QApplication::UnicodeUTF8));
    searchButton->setText(QApplication::translate("UIResultsWidget", "Search", 0, QApplication::UnicodeUTF8));
    pushButton->setText(QApplication::translate("UIResultsWidget", "Options...", 0, QApplication::UnicodeUTF8));
    dataTypeBox->clear();
    dataTypeBox->addItem(QApplication::translate("UIResultsWidget", "Any file type", 0, QApplication::UnicodeUTF8));
    dataTypeBox->addItem(QApplication::translate("UIResultsWidget", "Videos", 0, QApplication::UnicodeUTF8));
    dataTypeBox->addItem(QApplication::translate("UIResultsWidget", "Music", 0, QApplication::UnicodeUTF8));
    dataTypeBox->addItem(QApplication::translate("UIResultsWidget", "Compressed files", 0, QApplication::UnicodeUTF8));
    dataTypeBox->addItem(QApplication::translate("UIResultsWidget", "Pictures", 0, QApplication::UnicodeUTF8));
    dataTypeBox->addItem(QApplication::translate("UIResultsWidget", "Programs", 0, QApplication::UnicodeUTF8));
    dataTypeBox->addItem(QApplication::translate("UIResultsWidget", "Documents", 0, QApplication::UnicodeUTF8));
    dataTypeBox->addItem(QApplication::translate("UIResultsWidget", "Folders", 0, QApplication::UnicodeUTF8));
    sizeRestrictedBox->clear();
    sizeRestrictedBox->addItem(QApplication::translate("UIResultsWidget", "Any size", 0, QApplication::UnicodeUTF8));
    sizeRestrictedBox->addItem(QApplication::translate("UIResultsWidget", "Bigger than...", 0, QApplication::UnicodeUTF8));
    sizeRestrictedBox->addItem(QApplication::translate("UIResultsWidget", "Smaller than...", 0, QApplication::UnicodeUTF8));
    sizeTypeBox->clear();
    sizeTypeBox->addItem(QApplication::translate("UIResultsWidget", "bytes", 0, QApplication::UnicodeUTF8));
    sizeTypeBox->addItem(QApplication::translate("UIResultsWidget", "Kb", 0, QApplication::UnicodeUTF8));
    sizeTypeBox->addItem(QApplication::translate("UIResultsWidget", "Mb", 0, QApplication::UnicodeUTF8));
    sizeTypeBox->addItem(QApplication::translate("UIResultsWidget", "Gb", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(UIResultsWidget);
    } // retranslateUi

};

namespace Ui {
    class UIResultsWidget: public Ui_UIResultsWidget {};
} // namespace Ui

#endif // UI_RESULTSWIDGET_H

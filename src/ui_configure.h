#ifndef UI_CONFIGURE_H
#define UI_CONFIGURE_H

#include <Qt3Support/Q3Header>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "Qt3Support/Q3ListViewItemIterator"

class Ui_UIConfigureDialog
{
public:
    QVBoxLayout *vboxLayout;
    QTabWidget *tabWidget;
    QWidget *tab_2;
    QVBoxLayout *vboxLayout1;
    QGridLayout *gridLayout;
    QLabel *label_5;
    QLabel *label_7;
    QLabel *label_8;
    QLineEdit *descriptionBox;
    QLineEdit *emailBox;
    QComboBox *connSpeedBox;
    QLabel *label_6;
    QLineEdit *nickBox;
    QSpacerItem *spacerItem;
    QWidget *tab;
    QVBoxLayout *vboxLayout2;
    QGridLayout *gridLayout1;
    QLabel *label;
    QLabel *label_3;
    QHBoxLayout *hboxLayout;
    QSpinBox *uploadSpeedBox;
    QLabel *label_4;
    QSpacerItem *spacerItem1;
    QHBoxLayout *hboxLayout1;
    QSpinBox *slotsBox;
    QSpacerItem *spacerItem2;
    QFrame *line;
    QLabel *label_2;
    Q3ListView *shareList;
    QHBoxLayout *hboxLayout2;
    QSpacerItem *spacerItem3;
    QPushButton *shareAddButton;
    QPushButton *shareRemoveButton;
    QPushButton *shareEditButton;
    QHBoxLayout *hboxLayout3;
    QSpacerItem *spacerItem4;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *UIConfigureDialog)
    {
    UIConfigureDialog->setObjectName(QString::fromUtf8("UIConfigureDialog"));
    UIConfigureDialog->resize(QSize(678, 586).expandedTo(UIConfigureDialog->minimumSizeHint()));
    vboxLayout = new QVBoxLayout(UIConfigureDialog);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    tabWidget = new QTabWidget(UIConfigureDialog);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    tab_2 = new QWidget();
    tab_2->setObjectName(QString::fromUtf8("tab_2"));
    vboxLayout1 = new QVBoxLayout(tab_2);
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(9);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    gridLayout = new QGridLayout();
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    label_5 = new QLabel(tab_2);
    label_5->setObjectName(QString::fromUtf8("label_5"));

    gridLayout->addWidget(label_5, 0, 0, 1, 1);

    label_7 = new QLabel(tab_2);
    label_7->setObjectName(QString::fromUtf8("label_7"));

    gridLayout->addWidget(label_7, 2, 0, 1, 1);

    label_8 = new QLabel(tab_2);
    label_8->setObjectName(QString::fromUtf8("label_8"));

    gridLayout->addWidget(label_8, 3, 0, 1, 1);

    descriptionBox = new QLineEdit(tab_2);
    descriptionBox->setObjectName(QString::fromUtf8("descriptionBox"));

    gridLayout->addWidget(descriptionBox, 2, 1, 1, 1);

    emailBox = new QLineEdit(tab_2);
    emailBox->setObjectName(QString::fromUtf8("emailBox"));

    gridLayout->addWidget(emailBox, 1, 1, 1, 1);

    connSpeedBox = new QComboBox(tab_2);
    connSpeedBox->setObjectName(QString::fromUtf8("connSpeedBox"));

    gridLayout->addWidget(connSpeedBox, 3, 1, 1, 1);

    label_6 = new QLabel(tab_2);
    label_6->setObjectName(QString::fromUtf8("label_6"));

    gridLayout->addWidget(label_6, 1, 0, 1, 1);

    nickBox = new QLineEdit(tab_2);
    nickBox->setObjectName(QString::fromUtf8("nickBox"));

    gridLayout->addWidget(nickBox, 0, 1, 1, 1);


    vboxLayout1->addLayout(gridLayout);

    spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout1->addItem(spacerItem);

    tabWidget->addTab(tab_2, QApplication::translate("UIConfigureDialog", "Identity", 0, QApplication::UnicodeUTF8));
    tab = new QWidget();
    tab->setObjectName(QString::fromUtf8("tab"));
    vboxLayout2 = new QVBoxLayout(tab);
    vboxLayout2->setSpacing(6);
    vboxLayout2->setMargin(9);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
    gridLayout1 = new QGridLayout();
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(0);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    label = new QLabel(tab);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout1->addWidget(label, 0, 0, 1, 1);

    label_3 = new QLabel(tab);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    gridLayout1->addWidget(label_3, 1, 0, 1, 1);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    uploadSpeedBox = new QSpinBox(tab);
    uploadSpeedBox->setObjectName(QString::fromUtf8("uploadSpeedBox"));
    uploadSpeedBox->setMaximum(60000);
    uploadSpeedBox->setValue(0);

    hboxLayout->addWidget(uploadSpeedBox);

    label_4 = new QLabel(tab);
    label_4->setObjectName(QString::fromUtf8("label_4"));

    hboxLayout->addWidget(label_4);

    spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem1);


    gridLayout1->addLayout(hboxLayout, 0, 1, 1, 1);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    slotsBox = new QSpinBox(tab);
    slotsBox->setObjectName(QString::fromUtf8("slotsBox"));
    slotsBox->setMaximum(20);
    slotsBox->setValue(6);

    hboxLayout1->addWidget(slotsBox);

    spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem2);


    gridLayout1->addLayout(hboxLayout1, 1, 1, 1, 1);


    vboxLayout2->addLayout(gridLayout1);

    line = new QFrame(tab);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShape(QFrame::HLine);

    vboxLayout2->addWidget(line);

    label_2 = new QLabel(tab);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    vboxLayout2->addWidget(label_2);

    shareList = new Q3ListView(tab);
    shareList->setObjectName(QString::fromUtf8("shareList"));
    shareList->setAllColumnsShowFocus(true);

    vboxLayout2->addWidget(shareList);

    hboxLayout2 = new QHBoxLayout();
    hboxLayout2->setSpacing(6);
    hboxLayout2->setMargin(0);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    spacerItem3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout2->addItem(spacerItem3);

    shareAddButton = new QPushButton(tab);
    shareAddButton->setObjectName(QString::fromUtf8("shareAddButton"));

    hboxLayout2->addWidget(shareAddButton);

    shareRemoveButton = new QPushButton(tab);
    shareRemoveButton->setObjectName(QString::fromUtf8("shareRemoveButton"));

    hboxLayout2->addWidget(shareRemoveButton);

    shareEditButton = new QPushButton(tab);
    shareEditButton->setObjectName(QString::fromUtf8("shareEditButton"));

    hboxLayout2->addWidget(shareEditButton);


    vboxLayout2->addLayout(hboxLayout2);

    tabWidget->addTab(tab, QApplication::translate("UIConfigureDialog", "Uploads", 0, QApplication::UnicodeUTF8));

    vboxLayout->addWidget(tabWidget);

    hboxLayout3 = new QHBoxLayout();
    hboxLayout3->setSpacing(6);
    hboxLayout3->setMargin(0);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    spacerItem4 = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout3->addItem(spacerItem4);

    okButton = new QPushButton(UIConfigureDialog);
    okButton->setObjectName(QString::fromUtf8("okButton"));
    okButton->setDefault(true);

    hboxLayout3->addWidget(okButton);

    cancelButton = new QPushButton(UIConfigureDialog);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout3->addWidget(cancelButton);


    vboxLayout->addLayout(hboxLayout3);

    QWidget::setTabOrder(tabWidget, nickBox);
    QWidget::setTabOrder(nickBox, emailBox);
    QWidget::setTabOrder(emailBox, descriptionBox);
    QWidget::setTabOrder(descriptionBox, connSpeedBox);
    QWidget::setTabOrder(connSpeedBox, uploadSpeedBox);
    QWidget::setTabOrder(uploadSpeedBox, slotsBox);
    QWidget::setTabOrder(slotsBox, shareList);
    QWidget::setTabOrder(shareList, shareAddButton);
    QWidget::setTabOrder(shareAddButton, shareRemoveButton);
    QWidget::setTabOrder(shareRemoveButton, shareEditButton);
    QWidget::setTabOrder(shareEditButton, okButton);
    QWidget::setTabOrder(okButton, cancelButton);
    retranslateUi(UIConfigureDialog);
    QObject::connect(okButton, SIGNAL(clicked()), UIConfigureDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), UIConfigureDialog, SLOT(reject()));

    connSpeedBox->setCurrentIndex(2);


    QMetaObject::connectSlotsByName(UIConfigureDialog);
    } // setupUi

    void retranslateUi(QDialog *UIConfigureDialog)
    {
    UIConfigureDialog->setWindowTitle(QApplication::translate("UIConfigureDialog", "Configure Dave++", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("UIConfigureDialog", "Nickname", 0, QApplication::UnicodeUTF8));
    label_7->setText(QApplication::translate("UIConfigureDialog", "Description", 0, QApplication::UnicodeUTF8));
    label_8->setText(QApplication::translate("UIConfigureDialog", "Connection speed", 0, QApplication::UnicodeUTF8));
    descriptionBox->setText(QApplication::translate("UIConfigureDialog", "Dave++", 0, QApplication::UnicodeUTF8));
    emailBox->setText(QApplication::translate("UIConfigureDialog", "enoon@erewhon.com", 0, QApplication::UnicodeUTF8));
    connSpeedBox->clear();
    connSpeedBox->addItem(QApplication::translate("UIConfigureDialog", "Modem", 0, QApplication::UnicodeUTF8));
    connSpeedBox->addItem(QApplication::translate("UIConfigureDialog", "DSL", 0, QApplication::UnicodeUTF8));
    connSpeedBox->addItem(QApplication::translate("UIConfigureDialog", "Cable", 0, QApplication::UnicodeUTF8));
    connSpeedBox->addItem(QApplication::translate("UIConfigureDialog", "Satellite", 0, QApplication::UnicodeUTF8));
    connSpeedBox->addItem(QApplication::translate("UIConfigureDialog", "LAN(T1)", 0, QApplication::UnicodeUTF8));
    connSpeedBox->addItem(QApplication::translate("UIConfigureDialog", "LAN(T3)", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("UIConfigureDialog", "Email address", 0, QApplication::UnicodeUTF8));
    nickBox->setText(QApplication::translate("UIConfigureDialog", "Nick", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("UIConfigureDialog", "Identity", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("UIConfigureDialog", "Maximum speed", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("UIConfigureDialog", "Number of slots", 0, QApplication::UnicodeUTF8));
    uploadSpeedBox->setSuffix(QApplication::translate("UIConfigureDialog", " Kb/s", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("UIConfigureDialog", "(0 for unlimited)", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("UIConfigureDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Shared folders</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    shareAddButton->setText(QApplication::translate("UIConfigureDialog", "Add...", 0, QApplication::UnicodeUTF8));
    shareRemoveButton->setText(QApplication::translate("UIConfigureDialog", "Remove", 0, QApplication::UnicodeUTF8));
    shareEditButton->setText(QApplication::translate("UIConfigureDialog", "Edit...", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("UIConfigureDialog", "Uploads", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("UIConfigureDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("UIConfigureDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(UIConfigureDialog);
    } // retranslateUi

};

namespace Ui {
    class UIConfigureDialog: public Ui_UIConfigureDialog {};
} // namespace Ui

#endif // UI_CONFIGURE_H

#ifndef UI_SHAREEDIT_H
#define UI_SHAREEDIT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

class Ui_UIShareEditDialog
{
public:
    QVBoxLayout *vboxLayout;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *nameBox;
    QLineEdit *folderBox;
    QPushButton *browseButton;
    QLabel *label_2;
    QSpacerItem *spacerItem;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem1;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *UIShareEditDialog)
    {
    UIShareEditDialog->setObjectName(QString::fromUtf8("UIShareEditDialog"));
    UIShareEditDialog->resize(QSize(400, 129).expandedTo(UIShareEditDialog->minimumSizeHint()));
    vboxLayout = new QVBoxLayout(UIShareEditDialog);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    gridLayout = new QGridLayout();
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    label = new QLabel(UIShareEditDialog);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout->addWidget(label, 0, 0, 1, 1);

    nameBox = new QLineEdit(UIShareEditDialog);
    nameBox->setObjectName(QString::fromUtf8("nameBox"));

    gridLayout->addWidget(nameBox, 1, 1, 1, 2);

    folderBox = new QLineEdit(UIShareEditDialog);
    folderBox->setObjectName(QString::fromUtf8("folderBox"));

    gridLayout->addWidget(folderBox, 0, 1, 1, 1);

    browseButton = new QPushButton(UIShareEditDialog);
    browseButton->setObjectName(QString::fromUtf8("browseButton"));

    gridLayout->addWidget(browseButton, 0, 2, 1, 1);

    label_2 = new QLabel(UIShareEditDialog);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout->addWidget(label_2, 1, 0, 1, 1);


    vboxLayout->addLayout(gridLayout);

    spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem1 = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem1);

    okButton = new QPushButton(UIShareEditDialog);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout->addWidget(okButton);

    cancelButton = new QPushButton(UIShareEditDialog);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout->addWidget(cancelButton);


    vboxLayout->addLayout(hboxLayout);

    QWidget::setTabOrder(folderBox, browseButton);
    QWidget::setTabOrder(browseButton, nameBox);
    QWidget::setTabOrder(nameBox, okButton);
    QWidget::setTabOrder(okButton, cancelButton);
    retranslateUi(UIShareEditDialog);
    QObject::connect(okButton, SIGNAL(clicked()), UIShareEditDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), UIShareEditDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(UIShareEditDialog);
    } // setupUi

    void retranslateUi(QDialog *UIShareEditDialog)
    {
    UIShareEditDialog->setWindowTitle(QApplication::translate("UIShareEditDialog", "Edit shared folder", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("UIShareEditDialog", "Folder:", 0, QApplication::UnicodeUTF8));
    browseButton->setText(QApplication::translate("UIShareEditDialog", "Browse...", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("UIShareEditDialog", "Share name:", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("UIShareEditDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("UIShareEditDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(UIShareEditDialog);
    } // retranslateUi

};

namespace Ui {
    class UIShareEditDialog: public Ui_UIShareEditDialog {};
} // namespace Ui

#endif // UI_SHAREEDIT_H

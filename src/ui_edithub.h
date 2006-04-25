#ifndef UI_EDITHUB_H
#define UI_EDITHUB_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>

class Ui_EditHub
{
public:
    QVBoxLayout *vboxLayout;
    QGridLayout *gridLayout;
    QLineEdit *nameBox;
    QCheckBox *autoConnectBox;
    QLabel *label;
    QLabel *label_3;
    QSpinBox *portBox;
    QLineEdit *hostBox;
    QLabel *label_2;
    QSpacerItem *spacerItem;
    QFrame *line;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem1;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *EditHub)
    {
    EditHub->setObjectName(QString::fromUtf8("EditHub"));
    EditHub->resize(QSize(292, 178).expandedTo(EditHub->minimumSizeHint()));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(1), static_cast<QSizePolicy::Policy>(1));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(EditHub->sizePolicy().hasHeightForWidth());
    EditHub->setSizePolicy(sizePolicy);
    vboxLayout = new QVBoxLayout(EditHub);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    gridLayout = new QGridLayout();
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    nameBox = new QLineEdit(EditHub);
    nameBox->setObjectName(QString::fromUtf8("nameBox"));

    gridLayout->addWidget(nameBox, 0, 1, 1, 1);

    autoConnectBox = new QCheckBox(EditHub);
    autoConnectBox->setObjectName(QString::fromUtf8("autoConnectBox"));

    gridLayout->addWidget(autoConnectBox, 3, 0, 1, 2);

    label = new QLabel(EditHub);
    label->setObjectName(QString::fromUtf8("label"));

    gridLayout->addWidget(label, 1, 0, 1, 1);

    label_3 = new QLabel(EditHub);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    gridLayout->addWidget(label_3, 0, 0, 1, 1);

    portBox = new QSpinBox(EditHub);
    portBox->setObjectName(QString::fromUtf8("portBox"));
    portBox->setMaximum(65536);
    portBox->setMinimum(1);
    portBox->setValue(4977);

    gridLayout->addWidget(portBox, 2, 1, 1, 1);

    hostBox = new QLineEdit(EditHub);
    hostBox->setObjectName(QString::fromUtf8("hostBox"));

    gridLayout->addWidget(hostBox, 1, 1, 1, 1);

    label_2 = new QLabel(EditHub);
    label_2->setObjectName(QString::fromUtf8("label_2"));

    gridLayout->addWidget(label_2, 2, 0, 1, 1);


    vboxLayout->addLayout(gridLayout);

    spacerItem = new QSpacerItem(274, 21, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem);

    line = new QFrame(EditHub);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShape(QFrame::HLine);

    vboxLayout->addWidget(line);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem1 = new QSpacerItem(68, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem1);

    okButton = new QPushButton(EditHub);
    okButton->setObjectName(QString::fromUtf8("okButton"));

    hboxLayout->addWidget(okButton);

    cancelButton = new QPushButton(EditHub);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout->addWidget(cancelButton);


    vboxLayout->addLayout(hboxLayout);

    QWidget::setTabOrder(nameBox, hostBox);
    QWidget::setTabOrder(hostBox, portBox);
    QWidget::setTabOrder(portBox, autoConnectBox);
    QWidget::setTabOrder(autoConnectBox, okButton);
    QWidget::setTabOrder(okButton, cancelButton);
    retranslateUi(EditHub);
    QObject::connect(cancelButton, SIGNAL(clicked()), EditHub, SLOT(reject()));
    QObject::connect(okButton, SIGNAL(clicked()), EditHub, SLOT(accept()));

    QMetaObject::connectSlotsByName(EditHub);
    } // setupUi

    void retranslateUi(QDialog *EditHub)
    {
    EditHub->setWindowTitle(QApplication::translate("EditHub", "Edit Hub", 0, QApplication::UnicodeUTF8));
    autoConnectBox->setText(QApplication::translate("EditHub", "Connect automatically on startup", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("EditHub", "Hostname", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("EditHub", "Name", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("EditHub", "Port", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("EditHub", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("EditHub", "Cancel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(EditHub);
    } // retranslateUi

};

namespace Ui {
    class EditHub: public Ui_EditHub {};
} // namespace Ui

#endif // UI_EDITHUB_H

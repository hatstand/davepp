#ifndef UI_HUBWIDGET_H
#define UI_HUBWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_UIHubWidget
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QLabel *statusLabel;
    QToolButton *disconnectButton;
    QTextEdit *chatBox;
    QHBoxLayout *hboxLayout1;
    QLineEdit *inputBox;
    QPushButton *sendButton;

    void setupUi(QWidget *UIHubWidget)
    {
    UIHubWidget->setObjectName(QString::fromUtf8("UIHubWidget"));
    UIHubWidget->resize(QSize(400, 300).expandedTo(UIHubWidget->minimumSizeHint()));
    vboxLayout = new QVBoxLayout(UIHubWidget);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(9);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    statusLabel = new QLabel(UIHubWidget);
    statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
    statusLabel->setTextFormat(Qt::RichText);

    hboxLayout->addWidget(statusLabel);

    disconnectButton = new QToolButton(UIHubWidget);
    disconnectButton->setObjectName(QString::fromUtf8("disconnectButton"));

    hboxLayout->addWidget(disconnectButton);


    vboxLayout->addLayout(hboxLayout);

    chatBox = new QTextEdit(UIHubWidget);
    chatBox->setObjectName(QString::fromUtf8("chatBox"));
    chatBox->setReadOnly(true);

    vboxLayout->addWidget(chatBox);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setSpacing(6);
    hboxLayout1->setMargin(0);
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    inputBox = new QLineEdit(UIHubWidget);
    inputBox->setObjectName(QString::fromUtf8("inputBox"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(1));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(inputBox->sizePolicy().hasHeightForWidth());
    inputBox->setSizePolicy(sizePolicy);

    hboxLayout1->addWidget(inputBox);

    sendButton = new QPushButton(UIHubWidget);
    sendButton->setObjectName(QString::fromUtf8("sendButton"));

    hboxLayout1->addWidget(sendButton);


    vboxLayout->addLayout(hboxLayout1);

    retranslateUi(UIHubWidget);

    QMetaObject::connectSlotsByName(UIHubWidget);
    } // setupUi

    void retranslateUi(QWidget *UIHubWidget)
    {
    statusLabel->setText(QApplication::translate("UIHubWidget", "", 0, QApplication::UnicodeUTF8));
    disconnectButton->setText(QApplication::translate("UIHubWidget", "Disconnect", 0, QApplication::UnicodeUTF8));
    sendButton->setText(QApplication::translate("UIHubWidget", "Send", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(UIHubWidget);
    } // retranslateUi

};

namespace Ui {
    class UIHubWidget: public Ui_UIHubWidget {};
} // namespace Ui

#endif // UI_HUBWIDGET_H

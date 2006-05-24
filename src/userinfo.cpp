#include "userinfo.h"
#include "user.h"

UserInfo::UserInfo(QWidget* parent, User* user):
	QDialog(parent),
	Ui::UIUserInfo(),
	m_user(user)
{
	setupUi(this);

	connect(okButton, SIGNAL(clicked()), SLOT(accept()));
}

UserInfo::~UserInfo()
{}

void UserInfo::show()
{
	setWindowTitle("User Info - " + m_user->nick);
	nick->setText(m_user->nick);
	speed->setText(m_user->speed);
	share->setText(m_user->niceShareSize);

	QDialog::show();
}

void UserInfo::accept()
{
	close();
	deleteLater();
}

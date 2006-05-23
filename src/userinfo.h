#ifndef USERINFO_H
#define USERINFO_H

#include "ui_userinfo.h"

#include <QDialog>

class User;

class UserInfo : public QDialog, public Ui::UIUserInfo
{
	Q_OBJECT

public:
	UserInfo(QWidget* parent, User* user);
	~UserInfo();
	
public slots:
	void show();
	void accept();

private:
	User* m_user;
};


#endif

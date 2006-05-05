/***************************************************************************
 *   Copyright (C) 2006 by David Sansome   *
 *   david@linux   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef HUBWIDGET_H
#define HUBWIDGET_H

#include "ui_hubwidget.h"
#include "chatwidget.h"
#include "mainwindow.h"

#include <Q3ListView>
#include <QTime>
#include <QDebug>

class Server;
class PrivateChatWidget;
class User;

class HubWidget : public ChatWidget
{
	Q_OBJECT
public:
	HubWidget(HubDetailsListItem* details, Server* server, Q3ListView* userList) :ChatWidget(server, ""),
	m_details(details),
	m_userList(userList),
	m_userHeader(NULL),
	m_privateChats()
	{
		details->setConnection(m_server);
		connect(server, SIGNAL(chatMessage(QString, QString)), SLOT(chatMessage(QString, QString)));
		connect(server, SIGNAL(privateChatMessage(QString, QString)), SLOT(privateChatMessage(QString, QString)));
		connect(server, SIGNAL(stateChanged(int)), SLOT(stateChanged(int)));
		connect(server, SIGNAL(error(QString)), SLOT(error(QString)));

		qDebug() << "Created HubWidget";
	}
	~HubWidget();
	
	HubDetailsListItem* detailsListItem() {return m_details;}
	Server* server() {return m_server;}
	bool privateChatsOpen() {return m_privateChats.count() > 0;}
	
public slots:
	void disconnectPressed();
	void privateChatClosed(PrivateChatWidget* widget);
	
signals:
	void newPrivateChat(PrivateChatWidget* widget);
	
private slots:
	void stateChanged(int state);
	void error(QString message);
	void userJoined(User* user);
	void userQuit(User* user);
	void privateChatMessage(QString from, QString message);
	void chatMessage(QString from, QString message);

private:
	HubDetailsListItem* m_details;
	Q3ListView* m_userList;
	UserHeaderListItem* m_userHeader;
	QList<PrivateChatWidget*> m_privateChats;

};

#endif

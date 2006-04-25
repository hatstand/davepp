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

#include <Q3ListView>

class Server;
class MainWindow;
class HubDetailsListItem;
class ChatWidget;
class User;
class UserHeaderListItem;

class HubWidget : public QWidget, public Ui::UIHubWidget
{
	Q_OBJECT
public:
	HubWidget(HubDetailsListItem* details, Server* server, Q3ListView* userList);
	~HubWidget();
	
	bool isConnected();
	HubDetailsListItem* detailsListItem() {return m_details;}
	Server* server() {return m_server;}
	bool privateChatsOpen() {return m_privateChats.count() > 0;}
	
public slots:
	void disconnectPressed();
	void privateChatClosed(ChatWidget* widget);
	
signals:
	void newPrivateChat(ChatWidget* widget);
	
private slots:
	void chatMessage(QString from, QString message, bool priv);
	void stateChanged(int state);
	void error(QString message);
	void userJoined(User*);
	void userQuit(User*);
	
	void sendPressed();

private:
	HubDetailsListItem* m_details;
	Server* m_server;
	Q3ListView* m_userList;
	UserHeaderListItem* m_userHeader;
	QList<ChatWidget*> m_privateChats;
};

#endif

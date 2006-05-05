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
#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QTimer>

class HubWidget;
#include <ui_hubwidget.h>
#include "chatwidget.h"
#include "hubwidget.h"

class PrivateChatWidget : public ChatWidget
{
	Q_OBJECT
public:
	PrivateChatWidget(Server* server, QString nick) : ChatWidget(server, nick),
	userConnected(true)
	{
		statusLabel->setText("Chat with <b>" + nick + "</b> on <b>" + server->hubName() + "</b>");
		connect(server, SIGNAL(privateChatMessage()), SLOT(chatMessage()));
		connect(this, SIGNAL(newPrivateChat(PrivateChatWidget* widget)), MainWindow::getInstance(), SLOT(newPrivateChat(PrivateChatWidget* widget)));
		connect(this, SIGNAL(privateChatClosed(PrivateChatWidget* widget)), MainWindow::getInstance(), SLOT(privateChatClosed(PrivateChatWidget* widget)));
		emit newPrivateChat(this);
	}
	
	QString nick() {return m_nick;}

private:
	bool userConnected;

private slots:
	void disconnectPressed();
	void userJoined(User* user);
	void userQuit(User* user);
	void privateChatMessage(QString from, QString message);

signals:
	void privateChatClosed(PrivateChatWidget* widget);
	void newPrivateChat(PrivateChatWidget* widget);
};

#endif

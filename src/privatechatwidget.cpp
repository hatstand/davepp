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
#include "privatechatwidget.h"
#include "hubwidget.h"
#include "server.h"
#include "configuration.h"
#include "user.h"

#include <QTime>
#include <QDebug>
#include <QMetaMethod>

PrivateChatWidget::PrivateChatWidget(Server* server, QString nick, QMap<QString, PrivateChatWidget*> map) :
	ChatWidget(server, nick),
	userConnected(true),
	m_map(map)
{
	statusLabel->setText("Chat with <b>" + nick + "</b> on <b>" + server->hubName() + "</b>");
	disconnectButton->setText("Close");

	m_map.insert(nick, this);
	MainWindow::getInstance()->newPrivateChat(this);
}

PrivateChatWidget::~PrivateChatWidget()
{
	m_map.remove(m_nick);
	MainWindow::getInstance()->privateChatClosed(this);
}

void PrivateChatWidget::disconnectPressed()
{
	deleteLater();	
}

void PrivateChatWidget::userJoined(User* user)
{
	if(user->nick == m_nick)
	{
		userConnected = true;
		chatBox->append("<b>(User connected)</b>");
	}
}

void PrivateChatWidget::userQuit(User* user)
{
	if(user->nick == m_nick)
	{
		userConnected = false;
		chatBox->append("<b>(User disconnected)</b>");
	}
}

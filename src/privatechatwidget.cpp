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

#include <QTime>
#include <QDebug>
#include <QMetaMethod>

PrivateChatWidget::PrivateChatWidget(Server* server, QString nick) : ChatWidget(server, nick),
	userConnected(true)
{
	statusLabel->setText("Chat with <b>" + nick + "</b> on <b>" + server->hubName() + "</b>");
	disconnectButton->setText("Close");

	connect(server, SIGNAL(privateChatMessage(QString, QString)), SLOT(chatMessage(QString, QString)));
	connect((PrivateChatWidget*)this, SIGNAL(newPrivateChat(PrivateChatWidget*)), MainWindow::getInstance(), SLOT(newPrivateChat(PrivateChatWidget*)));
	connect(this, SIGNAL(privateChatClosed(PrivateChatWidget*)), MainWindow::getInstance(), SLOT(privateChatClosed(PrivateChatWidget*)));
	emit newPrivateChat(this);
}

void PrivateChatWidget::disconnectPressed()
{
	emit(privateChatClosed(this));
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

void PrivateChatWidget::privateChatMessage(QString from, QString message)
{
	if(from == m_nick)
		chatMessage(from, message);
}

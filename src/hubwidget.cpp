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
#include "hubwidget.h"
#include "server.h"
#include "mainwindow.h"
#include "chatwidget.h"

#include <QTime>
#include <QColor>

HubWidget::HubWidget(HubDetailsListItem* details, Server* server, Q3ListView* userList)
 : QWidget(NULL),
   Ui::UIHubWidget(),
   m_details(details),
   m_server(server),
   m_userList(userList),
   m_userHeader(NULL)
{
	setupUi(this);
	
	connect(server, SIGNAL(chatMessage(QString, QString, bool)), SLOT(chatMessage(QString, QString, bool)));
	connect(server, SIGNAL(stateChanged(int)), SLOT(stateChanged(int)));
	connect(server, SIGNAL(error(QString)), SLOT(error(QString)));
	connect(server, SIGNAL(userJoined(User*)), SLOT(userJoined(User*)));
	connect(server, SIGNAL(userQuit(User*)), SLOT(userQuit(User*)));
	
	connect(disconnectButton, SIGNAL(pressed()), SLOT(disconnectPressed()));
	connect(sendButton, SIGNAL(pressed()), SLOT(sendPressed()));
	connect(inputBox, SIGNAL(returnPressed()), SLOT(sendPressed()));
	
	details->setConnection(server);
	inputBox->setFocus();

	printTime();
	m_lastOutput = QTime::currentTime();
}


HubWidget::~HubWidget()
{
	delete m_server;
}

void HubWidget::chatMessage(QString from, QString message, bool priv)
{
	QString decoded = message;
	decoded.replace("<", "&lt;");
	decoded.replace(">", "&gt;");
	decoded.replace("\n", "<br>");
	bool time = false;
	if(m_lastOutput.secsTo(QTime::currentTime()) > 60 * 5)
	{
		time = true;
		m_lastOutput = QTime::currentTime();
	}

	if (!priv)
	{
		if(time)
			printTime();
		chatBox->append("<b>&lt;" + from + "&gt;</b> " + decoded);
		return;
	}
	
	ChatWidget* widget = NULL;
	
	foreach(ChatWidget* w, m_privateChats)
	{
		if (from == w->nick())
		{
			widget = w;
			break;
		}
	}
	
	if (widget == NULL)
	{
		widget = new ChatWidget(this, from);
		emit newPrivateChat(widget);
		m_privateChats << widget;
	}
	

	if(time)
		widget->printTime();

	widget->chatBox->append("<b>&lt;" + from + "&gt;</b> " + decoded);
}

void HubWidget::stateChanged(int state)
{
	QString label;
	switch (state)
	{
	case Server::NotConnected: label = "Not connected"; break;
	case Server::LookingUpHost: label = "Looking up host..."; break;
	case Server::Connecting: label = "Connecting..."; break;
	case Server::LoggingIn: label = "Logging in..."; break;
	case Server::Connected: label = m_server->hubName(); break;
	}
	
	if (state == Server::NotConnected)
		disconnectButton->setText("Connect");
	else
		disconnectButton->setText("Disconnect");
	
	statusLabel->setText("<b>" + label + "</b>");
}

bool HubWidget::isConnected()
{
	return (m_server->state() != Server::NotConnected);
}

void HubWidget::error(QString message)
{
	chatBox->append("<b>Error: " + message + "</b>");
}

void HubWidget::disconnectPressed()
{
	if (m_server->state() == Server::NotConnected)
		m_server->open();
	else
	{
		chatBox->append("(Disconnected)");
		m_server->disconnectFromHub();
	}
}

void HubWidget::userJoined(User* user)
{
	if (m_userHeader == NULL)
	{
		if (m_server->userCount() > 0)
			m_userHeader = new UserHeaderListItem(m_userList, m_server);
	}
}

void HubWidget::userQuit(User* user)
{
	if (m_userHeader != NULL)
	{
		if (m_server->userCount() <= 0)
		{
			delete m_userHeader;
			m_userHeader = NULL;
		}
	}
}

void HubWidget::sendPressed()
{
	if (inputBox->text().length() <= 0)
		return;
	
	if (isConnected())
	{
		m_server->sendMessage(inputBox->text());
		inputBox->clear();
	}
	else
	{
		chatBox->append("(You are not connected to the hub)");
	}
	inputBox->setFocus();
}

void HubWidget::privateChatClosed(ChatWidget* widget)
{
	m_privateChats.removeAll(widget);
}

void HubWidget::printTime()
{
	chatBox->append("<font color=\"gray\"><small>" + QTime::currentTime().toString("h:mm") + "</small></font>");
}

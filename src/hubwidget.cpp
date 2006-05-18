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
#include "privatechatwidget.h"

#include <QTime>
#include <QColor>
#include <QDebug>

HubWidget::HubWidget(HubDetailsListItem* details, Server* server, Q3ListView* userList)
 : ChatWidget(server, QString::null),
   m_details(details),
   m_userList(userList),
   m_userHeader(NULL),
	m_state(0)
{
	m_server->setHubChat(this);
	details->setConnection(m_server);
	connect(server, SIGNAL(stateChanged(int)), SLOT(stateChanged(int)));
	connect(server, SIGNAL(error(QString)), SLOT(error(QString)));
	connect(server, SIGNAL(becameOp(bool)), SLOT(becameOp(bool)));

	m_op = false;
}

HubWidget::~HubWidget()
{
	delete m_server;
}

void HubWidget::stateChanged(int state)
{
	m_state = state;

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

	if(m_op)
		statusLabel->setText("<b>" + label + "</b> (Op)");
	else
		statusLabel->setText("<b>" + label + "</b>");
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

void HubWidget::becameOp(bool yes)
{
	m_op = yes;

	stateChanged(m_state);
}

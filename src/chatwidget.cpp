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
#include "chatwidget.h"
#include "server.h"
#include "configuration.h"

#include <QTime>
#include <QDebug>
#include <Q3StyleSheet>

ChatWidget::ChatWidget(Server* server, QString nick)
 : Ui::UIHubWidget(), QWidget(NULL), m_server(server), m_nick(nick), m_lastOutput()
{
	setupUi(this);
	
	connect(m_server, SIGNAL(userJoined(User*)), SLOT(userJoined(User*)));
	connect(m_server, SIGNAL(userQuit(User*)), SLOT(userQuit(User*)));
	connect(disconnectButton, SIGNAL(pressed()), SLOT(disconnectPressed()));

	connect(sendButton, SIGNAL(pressed()), SLOT(sendPressed()));
	connect(inputBox, SIGNAL(returnPressed()), SLOT(sendPressed()));

	setInputBoxFocus();

	m_lastOutput.start();
	printTime();
}

ChatWidget::~ChatWidget()
{
}

void ChatWidget::setInputBoxFocus()
{
	inputBox->setFocus();
}

void ChatWidget::chatMessage(QString from, QString message)
{
	message = Q3StyleSheet::escape(message);
	printTimeIfNeeded();

	message.replace("\n", "<br />");
	chatBox->append("<b>" + from + ":</b> " + message);
}

void ChatWidget::printTime()
{
	chatBox->append("&nbsp;&nbsp;&nbsp;&nbsp;<font color=\"gray\"><small>" + QTime::currentTime().toString("h:mm") + "</small></font><br />");
}

void ChatWidget::printTimeIfNeeded()
{
	if(m_lastOutput.elapsed() > 60 * 5 * 1000)
	{
		printTime();
		m_lastOutput.restart();
	}
}

void ChatWidget::sendPressed()
{
	if (inputBox->text().length() <= 0)
		return;
	
	QString message = inputBox->text();

	if (isConnected())
	{
		m_server->sendMessage(message, m_nick);
		inputBox->clear();

		message = Q3StyleSheet::escape(message);
		QString from = Configuration::instance()->nick();
		chatMessage(from, message);
	}
	else
	{
		chatBox->append("(You are not connected to the hub)");
	}
	inputBox->setFocus();
}

bool ChatWidget::isConnected()
{
	return (m_server->state() != Server::NotConnected);
}

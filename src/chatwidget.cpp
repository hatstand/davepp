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
#include "hubwidget.h"
#include "server.h"
#include "configuration.h"

#include <QTime>
#include <QDebug>

ChatWidget::ChatWidget(HubWidget* hub, QString nick)
 : Ui::UIHubWidget(), QWidget(NULL), m_hub(hub), m_nick(nick)
{
	setupUi(this);
	
	disconnectButton->hide();
	statusLabel->setText("Chat with <b>" + nick + "</b> on <b>" + hub->server()->hubName() + "</b>");

	m_timer = new QTimer(this);
	m_timer->start(60000*5);
	connect(m_timer, SIGNAL(timeout()), SLOT(printTime()));
	printTime();

	connect(sendButton, SIGNAL(pressed()), SLOT(sendPressed()));
}


ChatWidget::~ChatWidget()
{
}

void ChatWidget::printTime()
{
	chatBox->append("<font color=\"gray\"><small>" + QTime::currentTime().toString("h:mm") + "</small></font>");
}

void ChatWidget::sendPressed()
{
	if (inputBox->text().length() <= 0)
		return;
	
	if (m_hub->isConnected())
	{
		QString message = inputBox->text();
		m_hub->server()->sendMessage(message, m_nick);
		inputBox->clear();
		// TODO: properly escape this
		message.replace("<", "&lt;");
		message.replace(">", "&gt;");
		message.replace("\n", "<br>");
		chatBox->append("<" + Configuration::instance()->nick() + "> " + message);
	}
	else
	{
		chatBox->append("(You are not connected to the hub)");
	}
	inputBox->setFocus();
}

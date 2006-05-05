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
#ifndef GENCHATWIDGET_H
#define GENCHATWIDGET_H

#include <QTime>

#include <ui_hubwidget.h>
#include "server.h"
#include "user.h"

class ChatWidget : public QWidget, public Ui::UIHubWidget
{
	Q_OBJECT
public:
 	ChatWidget(Server* server, QString nick);
	 ~ChatWidget();

	bool isConnected();
	QString nick() {return m_nick;}
	void setInputBoxFocus();
	
public slots:
	void chatMessage(QString from, QString message);


protected:
	QTime m_lastOutput;
	Server* m_server;
	QString m_nick;
	void printTime();
	void printTimeIfNeeded();

protected slots:
	void sendPressed();
	virtual void disconnectPressed() = 0;
	virtual void userJoined(User* user) =0;
	virtual void userQuit(User* user) = 0;
	virtual void privateChatMessage(QString from, QString message) = 0;
};

#endif

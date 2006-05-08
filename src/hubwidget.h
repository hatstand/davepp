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
#include "server.h"


#include <Q3ListView>
#include <QTime>
#include <QDebug>

class User;
class HubDetailsListItem;
class UserHeaderListItem;
class Server;

class HubWidget : public ChatWidget
{
	Q_OBJECT
public:
	HubWidget(HubDetailsListItem* details, Server* server, Q3ListView* userList);
	virtual ~HubWidget();
	
	HubDetailsListItem* detailsListItem() {return m_details;}
	Server* server() {return m_server;}
	
public slots:
	void disconnectPressed();
	
private slots:
	void stateChanged(int state);
	void error(QString message);
	void userJoined(User* user);
	void userQuit(User* user);

private:
	HubDetailsListItem* m_details;
	Q3ListView* m_userList;
	UserHeaderListItem* m_userHeader;

};

#endif

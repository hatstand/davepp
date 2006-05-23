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
#include "newclient.h"
#include "server.h"

#include <QDebug>
#include <QTimer>

NewClient::NewClient(Server* server, QTcpSocket* sock)
 : QObject(server), m_server(server), m_speedCount(0), m_socket(sock)
{
	m_speedTimer = new QTimer(this);
	connect(m_speedTimer, SIGNAL(timeout()), SLOT(speedTimerTimeout()));
	m_speedTimer->start(1000);
}


NewClient::~NewClient()
{
}

void NewClient::changeState(ConnectionState newState)
{
	m_state = newState;
	emit stateChanged(newState);
}

NewClient::ConnectionState NewClient::state()
{
	return m_state;
}

void NewClient::speedTimerTimeout()
{
	if (state() != Transferring)
		return;
	
	m_speedLog << m_speedCount;
	m_speedCount = 0;
	while (m_speedLog.count() > 5)
		m_speedLog.removeFirst();
	
	m_speed = 0;
	foreach(quint64 speed, m_speedLog)
		m_speed += speed;
	
	m_speed /= m_speedLog.count();
	
	emit speedChanged(m_speed);
}



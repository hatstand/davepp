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
#ifndef CLIENT_H
#define CLIENT_H

#include "utilities.h"

class Server;

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTextStream>
#include <QTimer>

class NewClient : public QObject
{
	Q_OBJECT
public:
	NewClient(Server* server, QTcpSocket* sock);
	virtual ~NewClient();
	
	enum ConnectionState
	{
		Idle,
		WaitingForConnection,
		LookingUpHost,
		Connecting,
		Handshaking,
		Transferring,
		NoSlots,
		Failed,
		Success
	};
	
	enum TransferResult
	{
		TransferFailed,
		TransferSucceeded
	};
	
	ConnectionState state();
	QString error() {return m_error;}
	quint64 speed() {return m_speed;}
	
protected:
	void changeState(ConnectionState newState);
	void socketReadyRead();

private slots:
	void speedTimerTimeout();

signals:
	void stateChanged(int newState);
	void speedChanged(quint64 speed);
	void result(int code);
	void progress(uint size, uint totalSize);
	
protected:
	Server* m_server;
	QTcpSocket* m_socket;
	QTextStream m_stream;
	QString m_error;
	
	QList<quint64> m_speedLog;
	QTimer* m_speedTimer;
	quint64 m_speedCount;
	quint64 m_speed;
	
private:
	ConnectionState m_state;
	QString m_buffer;
};

#endif

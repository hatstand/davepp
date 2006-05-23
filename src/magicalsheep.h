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
#ifndef MAGICALSHEEP_H
#define MAGICALSHEEP_H

#include "filelistbuilder.h"

#include <QTcpSocket>
#include <QTcpServer>
#include <QTextStream>
#include <QFile>
#include <QTimer>

class Server;

class MagicalSheep : public QObject
{
	Q_OBJECT
public:
	MagicalSheep(Server* parent, QString host = QString::null, quint16 port = 0);
	~MagicalSheep();
	
	quint16 listen();
	
	enum Direction
	{
		Connecting,
		Listening
	};

	enum TransferResult
	{
		TransferFailed,
		TransferSucceeded
	};

private:
	void parseCommand(QString command);
	void setupSocket();
	void sendSomeData();
	
private slots:
	void newConnection();
	
	void socketConnected();
	void socketDisconnected();
	void socketError(QAbstractSocket::SocketError error);
	void socketHostFound();
	void socketBytesWritten(quint64 num);
	void socketReadyRead();

signals:
	void infoChanged();
	void result(TransferResult);
	
private:
	Direction m_direction;
	QTcpSocket* m_socket;
	QTcpServer* m_tcpServer;
	Server* m_server;
	QTextStream m_stream;
	QString m_buffer;
	bool m_weWant;
	bool m_theyWant;
	int m_random;
	QString m_nick;
	QString m_lock;
	bool m_extendedClient;
	bool m_supportsBZList;
	bool m_supportsXmlBZList;
	QString m_fileName;
	quint64 m_offset;
	quint64 m_length;
	qint64 m_numbytes;
	quint64 m_sendPos;
	bool m_gotSlot;
	QString m_error;
	QFile m_file;
	QTimer m_sendTimer;
};

#endif

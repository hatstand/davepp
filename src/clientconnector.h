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
#ifndef CLIENTCONNECTOR_H
#define CLIENTCONNECTOR_H

#include "client.h"
#include <QObject>
#include <QTime>

class Server;

// Uploading
class ClientConnector : public Client
{
Q_OBJECT
public:
	ClientConnector(Server* server);
	~ClientConnector();
	
	void connectToClient(QString hostName, quint16 port);
	
	QString nick() {return m_nick;}
	QString fileName() {return m_fileName;}
	
private slots:
	void socketHostFound();
	void socketConnected();
	void socketError(QAbstractSocket::SocketError err);
	void socketBytesWritten(qint64 num);
	void socketDisconnected();
	void sendSomeData();
	void gotUserIP(QString nick, QString host);
	
signals:
	void infoChanged();
	
private:
	void parseCommand(QString command);
	
private:
	QString m_lock;
	QString m_nick;
	QString m_fileName;
	quint64 m_offset;
	quint64 m_fileLength;
	qint64 m_numbytes;
	QFile m_file;
	quint16 m_port;

	bool extendedClient;
	bool supportsBZList;
	bool supportsXmlBZList;
	
	QTime m_sendTimer;
	QTimer* m_timer;
	qint64 m_sendPos;
};

#endif

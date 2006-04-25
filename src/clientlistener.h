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
#ifndef CLIENTLISTENER_H
#define CLIENTLISTENER_H

#include <QObject>
#include <QTcpServer>
#include <QFile>

#include "client.h"

class User;

class ClientListener : public Client
{
	Q_OBJECT
public:
	ClientListener(Server* server, User* user, QString filename, QString downloadDir, uint offset = 1);
	ClientListener(Server* server, User* user);
	~ClientListener();
	
	void listenForClients(int port);
	
	enum TransferResult
	{
		TransferFailed,
		TransferSucceeded
	};
	
	bool isFileList() {return m_dclst;}
	User* user() {return m_user;}
	
private slots:
	void newConnection();
	void socketHostFound();
	void socketConnected();
	void socketError(QAbstractSocket::SocketError err);
	void socketBytesWritten(qint64 num);
	void socketDisconnected();
	void socketReadyRead();
	
private:
	void parseCommand(QString command);

private:
	QTcpServer* m_tcpServer;
	bool m_receiving;
	// Where we're downloading to
	QString m_downloadDir;
	// The name of our currently downloading file
	QString m_filename;
	// Whether we're downloading a file list or not
	bool m_dclst;
	QByteArray m_dclistData;
	// The user we're currently downloading from
	User* m_user;

	Server* m_server;
	uint m_offset;
	
	uint m_size;
	uint m_totalSize;
	
	QFile* m_outFile;

	bool extendedClient;
	bool supportsBZList;

signals:
	void result(int code);
	void progress(uint size, uint totalSize);
};

#endif

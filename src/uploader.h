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
#ifndef UPLOADER_H
#define UPLOADER_H

#include "newclient.h"
#include <QObject>
#include <QTime>

class Server;

// Uploading
class Uploader : public NewClient
{
Q_OBJECT
public:
	Uploader(Server* server, QTcpSocket* sock, QString localFile, quint64 localOffset, quint64 localLength, bool uget);
	~Uploader();
	
private slots:
	void socketError(QAbstractSocket::SocketError err);
	void socketBytesWritten(qint64 num);
	void socketDisconnected();
	void socketReadyRead();
	void sendSomeData();
	
signals:
	void infoChanged();
	
private:
	void parseCommand(QString command);
	
private:
	QString m_filename;
	quint64 m_offset;
	quint64 m_fileLength;
	qint64 m_numbytes;
	QFile m_file;
	bool m_dclst;

	QTime m_sendTimer;
	QTimer* m_timer;
	qint64 m_sendPos;

	bool gotSlot;
};

#endif

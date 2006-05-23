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
#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QTcpServer>
#include <QFile>
#include <QTimer>

#include "newclient.h"

class User;

// Downloading
class Downloader : public NewClient
{
	Q_OBJECT
public:
	Downloader(Server* server, QTcpSocket* sock, QString remoteFile, quint64 offset, quint64 length);
	~Downloader();
	
	bool isFileList() {return m_dclst;}
	
private slots:
	void socketError(QAbstractSocket::SocketError err);
	void socketBytesWritten(qint64 num);
	void socketDisconnected();
	void socketReadyRead();
	
private:
	void parseCommand(QString command);
	void setup();

private:
	QString m_downloadDir;
	// The name of our currently downloading file
	QString m_filename;
	// Whether we're downloading a file list or not
	bool m_dclst;
	QByteArray m_dclistData;

	quint64 m_offset;
	quint64 m_size;
	quint64 m_totalSize;
	
	QFile* m_outFile;

	uint m_speedCount;

	bool m_receiving;
	quint64 bytesWritten;
};

#endif

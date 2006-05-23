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
#include "downloader.h"
#include "server.h"
#include "utilities.h"
#include "filelist.h"
#include "user.h"
#include "commands.h"
#include "configuration.h"

#include <QDebug>
#include <QDir>
#include <QTemporaryFile>
#include <QTimer>

#include <QTreeWidget>


Downloader::Downloader(Server* server, QTcpSocket* sock, QString filename, quint64 offset, quint64 length)
 : NewClient(server, sock),
 	m_filename(filename),
	m_offset(offset),
	m_totalSize(length)
{
	if(m_filename == "MyList.DcLst" || m_filename == "MyList.bz2" || m_filename == "files.xml.bz2")
		m_dclst = true;
	else
	{
		m_outFile = new QFile(m_filename);
		m_outFile->open(QIODevice::WriteOnly);
	}

	setup();
}

void Downloader::setup()
{
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(readyRead()), SLOT(socketReadyRead()));
	connect(m_socket, SIGNAL(disconnected()), SLOT(socketDisconnected()));
	connect(m_socket, SIGNAL(bytesWritten(qint64)), SLOT(socketBytesWritten(qint64)));

	m_receiving = true;
	m_speedCount = 0;

	socketReadyRead();
}

Downloader::~Downloader()
{
	qDebug() << "Deleting clientlistener";
	if(!m_dclst && bytesWritten < m_totalSize)
	{
		qDebug() << "Saving file";
		Configuration::instance()->saveFile(m_outFile->name(), m_server->me()->nick, m_filename, m_server->hubName(), bytesWritten);
	}

	if(!m_dclst)
		delete m_outFile;
}

void Downloader::socketError(QAbstractSocket::SocketError err)
{
	qDebug() << "Error" << err;
	m_error = m_socket->errorString();
}

void Downloader::socketReadyRead()
{
	qDebug() << "Received Data";
	if (!m_receiving)
		return;
	
	quint64 bytesAvailable = m_socket->bytesAvailable();
	m_size += bytesAvailable;
	m_speedCount += bytesAvailable;
	
	if (!m_dclst) // Downloading a file
		bytesWritten += m_outFile->write(m_socket->readAll());
	else // Downloading a dclist
		m_dclistData.append(m_socket->readAll());
	
	emit progress(m_size, m_totalSize);
	
	if (m_size >= m_totalSize)
	{
		m_socket->close();
		
		m_receiving = false;
		changeState(Idle);
		deleteLater();

		if(m_dclst)
		// Do a whole load of decoding of the list (Huffman)
		{
			QByteArray decodedList;
				 
			if(m_filename != "MyList.DcLst")
				decodedList = Utilities::decodeBZList(m_dclistData);
			else
				decodedList = Utilities::decodeList(m_dclistData);

			FileList* fileList;
			
			if(m_filename == "files.xml.bz2")
				fileList = new FileList(decodedList);
			else
			{
				QTextStream* stream = new QTextStream(decodedList);
				fileList = new FileList(stream);
				delete stream;
			}
			
			m_server->me()->setFileList(fileList);
		}
		else
			m_outFile->close();
		
		emit result(TransferSucceeded);
		qDebug() << "Download finished";
	}
}

void Downloader::socketBytesWritten(qint64 num)
{
	qDebug() << "Bytes written" << num;
}

void Downloader::socketDisconnected()
{
	qDebug() << "Disconnected";
}


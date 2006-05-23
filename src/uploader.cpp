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
#include "uploader.h"
#include "server.h"
#include "user.h"
#include "filelistbuilder.h"
#include "configuration.h"
#include "commands.h"

#include <QDebug>
#include <QFileInfo>
#include <QTimer>
#include <QAbstractSocket>

Uploader::Uploader(Server* server, QTcpSocket* sock, QString localFile, quint64 localOffset, quint64 localLength, bool uget)
:	NewClient(server, sock),
	m_filename(localFile),
	m_offset(localOffset),
	m_numbytes(localLength)
{
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(bytesWritten(qint64)), SLOT(socketBytesWritten(qint64)));
	connect(m_socket, SIGNAL(disconnected()), SLOT(socketDisconnected()));
	if(!uget)
		connect(m_socket, SIGNAL(readyRead()), SLOT(socketReadyRead()));

	m_stream.setDevice(m_socket);

	m_sendPos = m_offset;
	m_numbytes = m_fileLength;

	if(m_filename == "MyList.DcLst" || m_filename == "MyList.bz2" || m_filename == "files.xml.bz2")
		m_dclst = true;
	else
		m_dclst = false;

	if(!m_dclst)
	{
		if(!Configuration::instance()->getSlot())
		{
			m_stream << MAXEDOUT << "|";
			m_stream.flush();

			qDebug() << "No slots";
			return;
		}

	}

	FileListBuilder* builder = FileListBuilder::instance();
	if(m_filename == "MyList.DcLst")
		m_fileLength = builder->huffmanList().length();
	else if(m_filename == "MyList.bz2")
		m_fileLength = builder->bzList().length();
	else if(m_filename == "files.xml.bz2")
		m_fileLength = builder->xmlBZList().length();
	else
	{
		QString filename = Configuration::instance()->sharedFilename(m_filename);
		m_fileLength = QFileInfo(filename).size();
		m_file.setFileName(filename);
		if (m_file.exists())
		{
			m_file.open(QIODevice::ReadOnly);
			m_file.seek(m_offset);
		}
	}

	if(m_numbytes <= 0)
		m_numbytes = m_fileLength;

	if(uget)
	{
		if(m_fileLength == 0)
			m_stream << "$Sending " << m_fileLength - m_offset << "|";
		else
			m_stream << "$Sending|";

		m_stream.flush();
		sendSomeData();
	}
	else
	{
		m_stream << FILELENGTH << " " << m_fileLength - m_offset << "|";
	}

}

Uploader::~Uploader()
{
	if(gotSlot)
		Configuration::instance()->revokeSlot();

	delete(m_socket);
}

void Uploader::socketReadyRead()
{
	if(m_socket->readAll() == "$Send|")
	{
		disconnect(this, SLOT(socketReadyRead()));
		sendSomeData();
	}
	else
	{
		qDebug() << "CLIENT sending weird shit";
		deleteLater();
	}
}

void Uploader::socketError(QAbstractSocket::SocketError err)
{
	if(m_sendPos < m_numbytes)
	{
		qDebug() << "Error" << err;
		changeState(Failed);
	}
	else
	{
		qDebug() << "Sent file";
		changeState(Success);
	}
}

void Uploader::sendSomeData()
{
	// qDebug() << "SendSomeData()";
	if (m_socket->state() != QAbstractSocket::ConnectedState)
	{
		qDebug() << "Sending failed (connection closed)";
		changeState(Failed);
		return;
	}
	
	if (m_sendPos >= m_numbytes + m_offset)
	{
		qDebug() << "Sending finished (" << m_sendPos << m_fileLength << ")";
		changeState(Success);
		return;
	}
	
	QByteArray data;
	int uploadSpeed;
	
	int slotsInUse = Configuration::instance()->slotsInUse(); 
	if(slotsInUse > 1)
		uploadSpeed = Configuration::instance()->uploadSpeed() / slotsInUse;
	else
		uploadSpeed = Configuration::instance()->uploadSpeed();

	if (uploadSpeed <= 0)
		uploadSpeed = 102400;
	
	int uploadBytes = uploadSpeed * 1024;
	if(uploadBytes > m_numbytes)
		uploadBytes = m_numbytes;
	
	if(m_dclst)
	{
		if (m_filename == "MyList.DcLst")
			data = FileListBuilder::instance()->huffmanList().mid(m_sendPos, uploadBytes);
		else if(m_filename == "MyList.bz2")
			data = FileListBuilder::instance()->bzList().mid(m_sendPos, uploadBytes);
		else if(m_filename == "files.xml.bz2")
			data = FileListBuilder::instance()->xmlBZList().mid(m_sendPos, uploadBytes);
	}
	else
		data = m_file.read(uploadBytes);
	
	qint64 temp = m_socket->write(data);
	if(temp < 0)
		qDebug() << "Error in socket writing";
	else
		m_sendPos += temp;

	m_sendTimer.start();
	m_socket->flush();
	
	if(data.length() <= 0)
		qDebug() << "No data to read";
}

void Uploader::socketBytesWritten(qint64 num)
{
	qDebug() << "Bytes written" << num;
	if (state() != Transferring)
		return;
	
	m_speedCount += num;
	
	if (m_socket->bytesToWrite() > 0)
		return;
	
	emit progress(m_sendPos - m_offset, m_numbytes);
	
	if ((Configuration::instance()->uploadSpeed() != 0) && (m_sendTimer.elapsed() < 1000))
	{
		m_timer->start(1000 - m_sendTimer.elapsed());
	}
	else
		sendSomeData();
}

void Uploader::socketDisconnected()
{
	qDebug() << "Disconnected";

	if(m_sendPos < m_offset + m_numbytes)
		emit result(TransferSucceeded);
	else
		emit result(TransferFailed);

	deleteLater();
}

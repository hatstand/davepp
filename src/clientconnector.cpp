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
#include "clientconnector.h"
#include "server.h"
#include "user.h"
#include "filelistbuilder.h"
#include "configuration.h"
#include "commands.h"

#include <QDebug>
#include <QFileInfo>
#include <QTimer>

ClientConnector::ClientConnector(Server* server)
 : Client(server)
{
	m_socket = new QTcpSocket(this);
	
	connect(m_socket, SIGNAL(hostFound()), SLOT(socketHostFound()));
	connect(m_socket, SIGNAL(connected()), SLOT(socketConnected()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(readyRead()), SLOT(socketReadyRead()));
	connect(m_socket, SIGNAL(bytesWritten(qint64)), SLOT(socketBytesWritten(qint64)));
	connect(m_socket, SIGNAL(disconnected()), SLOT(socketDisconnected()));
	
	m_stream.setDevice(m_socket);
	
	changeState(Idle);
}


ClientConnector::~ClientConnector()
{
}

void ClientConnector::connectToClient(QString hostName, int port)
{
	if (state() != Idle)
		return;
	
	changeState(LookingUpHost);
	m_socket->connectToHost(hostName, port);
}

void ClientConnector::socketHostFound()
{
	changeState(Connecting);
	qDebug() << "Host found";
}

void ClientConnector::socketConnected()
{
	changeState(Handshaking);
	qDebug() << "Connected";
	
	m_stream << "MYNICK " << m_server->me()->nick << "|";
	m_stream << "$Lock something Pk=DavePlusPlus|";
	m_stream.flush();
}

void ClientConnector::socketError(QAbstractSocket::SocketError err)
{
	qDebug() << "Error" << err;
	changeState(Idle);
}



void ClientConnector::parseCommand(QString command)
{
	qDebug() << "CLIENT Command: " << command;
	if (command.startsWith("$"))
	{
		QStringList words = command.split(" ");
		if (words[0] == MYNICK)
		{
			m_nick = command.section(' ', 1);
			emit infoChanged();
		}
		else if (words[0] == "$Lock")
		{
			m_lock = words[1];
		}
		else if (words[0] == DIRECTION)
		{
		}
		else if (words[0] == "$Key")
		{
			qDebug() << "Sending key:" << Utilities::lockToKey(m_lock);
			m_stream << DIRECTION << " Download 1234|";
			m_stream << "$Key " << Utilities::lockToKey(m_lock) << "|";
			m_stream.flush();
		}
		else if (words[0] == GET)
		{
			QString temp = command.section(" ", 1);
			m_fileName = temp.section("$", 0, 0);
			m_offset = temp.section("$", 1).toLongLong();
			
			emit infoChanged();
			
			FileListBuilder* builder = FileListBuilder::instance();
			if (m_fileName == "MyList.DcLst")
				m_fileLength = builder->huffmanList().length();
			else
			{
				QString fileName = Configuration::instance()->sharedFilename(m_fileName);
				m_fileLength = QFileInfo(fileName).size();
				m_file.setFileName(fileName);
				if (m_file.exists())
				{
					m_file.open(QIODevice::ReadOnly);
					m_file.seek(m_offset-1);
				}
			}
			
			if (m_fileLength <= 0)
			{
				m_stream << "$Error File Not Available|";
				deleteLater();
			}
			else
				m_stream << FILELENGTH << " " << m_fileLength << "|";
			
			m_stream.flush();
			
			qDebug() << "Uploading" << m_fileName << "from position" << m_offset << "(length" << m_fileLength << ")";
		}
		else if (words[0] == SEND)
		{
			changeState(Transferring);
			m_sendPos = m_offset - 1;
			sendSomeData();
		}
		else
			qDebug() << "\"" << words[0] << "\" not understood";
	}
}

void ClientConnector::sendSomeData()
{
	if (m_socket->state() != QAbstractSocket::ConnectedState)
	{
		deleteLater();
		qDebug() << "Sending failed (connection closed)";
		return;
	}
	
	if (m_sendPos >= m_fileLength)
	{
		m_socket->close();
		deleteLater();
		qDebug() << "Sending finished (" << m_sendPos << m_fileLength;
		return;
	}
	
	QByteArray data;
	int uploadSpeed = Configuration::instance()->uploadSpeed();
	if (uploadSpeed <= 0)
		uploadSpeed = 50;
	
	if (m_fileName == "MyList.DcLst")
		data = FileListBuilder::instance()->huffmanList().mid(m_sendPos, uploadSpeed*1024);
	else
		data = m_file.read(uploadSpeed*1024);
	
	m_socket->write(data);
	m_sendPos += data.length();
	m_sendTimer.start();
}

void ClientConnector::socketBytesWritten(qint64 num)
{
	qDebug() << "Bytes written" << num;
	if (state() != Transferring)
		return;
	
	m_speedCount += num;
	
	if (m_socket->bytesToWrite() > 0)
		return;
	
	emit progress(m_sendPos, m_fileLength);
	
	if ((Configuration::instance()->uploadSpeed() != 0) && (m_sendTimer.elapsed() < 1000))
		QTimer::singleShot(1000 - m_sendTimer.elapsed(), this, SLOT(sendSomeData()));
	else
		sendSomeData();
}

void ClientConnector::socketDisconnected()
{
	qDebug() << "Disconnected";
}





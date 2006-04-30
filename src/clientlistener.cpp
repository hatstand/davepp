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
#include "clientlistener.h"
#include "server.h"
#include "utilities.h"
#include "filelist.h"
#include "user.h"
#include "commands.h"

#include <QDebug>
#include <QDir>
#include <QTemporaryFile>
#include <QTimer>

#include <QTreeWidget>


ClientListener::ClientListener(Server* server, User* user, QString filename, QString downloadDir, uint offset)
 : Client(server),
   m_receiving(false),
   m_user(user),
   m_server(server),
   m_filename(filename),
   m_offset(offset),
   m_dclst(false),
   m_downloadDir(downloadDir),
   m_outFile(NULL)
{
	m_tcpServer = new QTcpServer(this);
	
	connect(m_tcpServer, SIGNAL(newConnection()), SLOT(newConnection()));
}

ClientListener::ClientListener(Server* server, User* user)
 : Client(server),
   m_receiving(false),
   m_user(user),
   m_server(server),
   m_filename("MyList.DcLst"),
   m_offset(1),
   m_dclst(true),
   m_outFile(NULL),
	supportsBZList(false),
	extendedClient(false),
	supportsXmlBZList(false)
{
	m_tcpServer = new QTcpServer(this);
	
	connect(m_tcpServer, SIGNAL(newConnection()), SLOT(newConnection()));
}


ClientListener::~ClientListener()
{
	delete m_outFile;
}

void ClientListener::listenForClients(int port)
{
	m_tcpServer->listen(QHostAddress::Any, port);
	changeState(WaitingForConnection);

	QTimer::singleShot(10000, this, SLOT(deleteLater()));
}

void ClientListener::newConnection()
{
	m_socket = m_tcpServer->nextPendingConnection();
	m_tcpServer->close();
	
	connect(m_socket, SIGNAL(hostFound()), SLOT(socketHostFound()));
	connect(m_socket, SIGNAL(connected()), SLOT(socketConnected()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(readyRead()), SLOT(socketReadyRead()));
	connect(m_socket, SIGNAL(bytesWritten(qint64)), SLOT(socketBytesWritten(qint64)));
	connect(m_socket, SIGNAL(disconnected()), SLOT(socketDisconnected()));
	
	changeState(Handshaking);
	
	m_stream.setDevice(m_socket);
}

void ClientListener::socketHostFound()
{
	changeState(Connecting);
	qDebug() << "Host found";
}

void ClientListener::socketConnected()
{
	changeState(Handshaking);
	qDebug() << "Connected";
}

void ClientListener::socketError(QAbstractSocket::SocketError err)
{
	qDebug() << "Error" << err;
	m_error = m_socket->errorString();
	changeState(Idle);
	emit result(TransferFailed);
	deleteLater();
}

void ClientListener::socketReadyRead()
{
	if (!m_receiving)
	{
		Client::socketReadyRead();
		return;
	}
	
	quint64 bytesAvailable = m_socket->bytesAvailable();
	m_size += bytesAvailable;
	m_speedCount += bytesAvailable;
	
	if (!m_dclst) // Downloading a file
		m_outFile->write(m_socket->readAll());
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
				 
			if(supportsXmlBZList || supportsBZList)
				decodedList = Utilities::decodeBZList(m_dclistData);
			else
				decodedList = Utilities::decodeList(m_dclistData);

			FileList* fileList;
			
			if(supportsXmlBZList)
				fileList = new FileList(decodedList);
			else
			{
				QTextStream* stream = new QTextStream(decodedList);
				fileList = new FileList(stream);
				delete stream;
			}
			
			m_user->setFileList(fileList);
		}
		else
			m_outFile->close();
		
		emit result(TransferSucceeded);
	}
}

void ClientListener::parseCommand(QString command)
{
	qDebug() << "CLIENT Command: " << command;
	if (command.startsWith("$"))
	{
		QStringList words = command.split(" ");
		if (words[0] == MYNICK)
		{
		}
		else if (words[0] == "$Lock")
		{
			if(words[1].split(" ")[0].startsWith("EXTENDEDPROTOCOL"))
				extendedClient = true; // Client supports as least some extensions
			else
				extendedClient = false;
				
			m_stream << MYNICK << " " << m_server->me()->nick << "|";
			m_stream << "$Lock EXTENDEDPROTOCOLsomething Pk=DavePlusPlus|";
			if(extendedClient)
				m_stream << SUPPORTS << " BZList XmlBZList |";
			m_stream << DIRECTION << " Download 1234|";
			m_stream << "$Key " << Utilities::lockToKey(words[1]) << "|";
			m_stream.flush();
		}
		else if(words[0] == SUPPORTS)
		{
			if(words.contains("XmlBZList"))
			{
				supportsXmlBZList = true;
				qDebug() << "XmlBZList";
				if(m_dclst)
					m_filename = "files.xml.bz2";
			}
			else if(words.contains("BZList"))
			{
				supportsBZList = true; // Supports the BZ2List extension
				qDebug() << "BZList";
				if(m_dclst)
					m_filename = "MyList.bz2";
			}
		}
		else if (words[0] == DIRECTION)
		{
		}
		else if (words[0] == "$Key")
		{
			m_stream << GET << " " << m_filename << "$" << m_offset << "|";
			m_stream.flush();
		}
		else if (words[0] == FILELENGTH)
		{
			changeState(Transferring);
			
			m_stream << SEND << "|";
			m_stream.flush();
			
			m_receiving = true;
			m_size = 0;
			m_totalSize = words[1].toUInt();
			qDebug() << "Opening file";
			
			if (!m_dclst)
			{
				QString trimmedFileName = m_filename;
				trimmedFileName.replace('\\', '/');
				int pos = trimmedFileName.lastIndexOf('/');
				if (pos != -1)
					trimmedFileName = m_filename.mid(pos+1);
				
				m_outFile = new QFile(m_downloadDir + QDir::separator() + trimmedFileName);
				if(m_outFile->exists() && (m_offset == 1))
					m_outFile->remove();
				
				m_outFile->open(QIODevice::WriteOnly | QIODevice::Append);
			}
		}
		else if (words[0] == "$Error")
		{
			m_error = command.mid(7);
			changeState(Idle);
			emit result(TransferFailed);
			m_socket->close();
			deleteLater();
		}
		else if (words[0] == MAXEDOUT)
		{
			changeState(Idle);
			m_error = "No slots";
			emit result(TransferFailed);
			m_socket->close();
			deleteLater();
		}
		else
			qDebug() << "\"" << words[0] << "\" not understood";
	}
}

void ClientListener::socketBytesWritten(qint64 num)
{
	qDebug() << "Bytes written" << num;
}

void ClientListener::socketDisconnected()
{
	qDebug() << "Disconnected";
}


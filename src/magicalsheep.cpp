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
#include "magicalsheep.h"
#include "commands.h"

MagicalSheep::MagicalSheep(Server* parent, QString host, quint16 port)
 : QObject(parent), m_socket(NULL), m_server(server)
{
	if (host.isNull())
		m_direction = Listening;
	else
	{
		m_direction = Connecting;
		
		m_socket = new QTcpSocket(this);
		m_socket->connectToHost(host, port);
		setupSocket();
	}
}

MagicalSheep::~MagicalSheep()
{
}

void MagicalSheep::setupSocket()
{
	Q_ASSERT(m_socket);
	
	connect(m_socket, SIGNAL(connected()), SLOT(socketConnected()));
	connect(m_socket, SIGNAL(disconnected()), SLOT(socketDisconnected()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(hostFound()), SLOT(socketHostFound()));
	connect(m_socket, SIGNAL(bytesWritten(quint64)), SLOT(socketBytesWritten(quint64)));
	connect(m_socket, SIGNAL(readyRead()), SLOT(socketReadyRead()));
	
	m_stream.setDevice(m_socket);
}

quint16 MagicalSheep::listen()
{
	m_tcpServer = new QTcpServer(this);
	m_tcpServer->listen();
	connect(m_tcpServer, SIGNAL(newConnection()), SLOT(newConnection()));
	
	return m_tcpServer->serverPort();
}

void MagicalSheep::newConnection()
{
	if (m_socket != NULL)
		return;
	
	m_socket = m_tcpServer->nextPendingConnection();
	setupSocket();
}

void MagicalSheep::socketConnected()
{
	qDebug() << "Socket connected";
	
	if (m_direction != Connecting)
		return;
	
	m_stream << MYNICK << " " << m_server->me()->nick << "|";
	m_stream << "$Lock EXTENDEDPROTOCOLsomething Pk=DavePlusPlus|";
	m_stream.flush();
}

void MagicalSheep::socketDisconnected()
{
	qDebug() << "Socket disconnected";
}

void MagicalSheep::socketError(QAbstractSocket::SocketError error)
{
	qDebug() << "Socket error" << error;
}

void MagicalSheep::socketHostFound()
{
	qDebug() << "Socket host found";
}

void MagicalSheep::socketBytesWritten(quint64 num)
{
	qDebug() << "Socket bytes written" << num;
}

void MagicalSheep::socketReadyRead()
{
	qDebug() << "Socket ready read";
	
	QString data = m_socket->readAll();
	data = m_buffer + data;
	if(data.endsWith("|"))
	{
		QStringList commands = data.split("|");
		for ( QStringList::iterator it = commands.begin(); it != commands.end(); ++it )
		{
			QString command = (*it);
			parseCommand(command);
		}
		m_buffer = "";
	}
	else
	{
		qDebug() << "DATA TO BUFFER" << data;
		m_buffer += data;
	}
}

void MagicalSheep::parseCommand(QString command)
{
	qDebug() << "CLIENT Command: " << command;
	if (!command.startsWith("$"))
		return;
	
	QStringList words = command.split(" ");
	if (words[0] == MYNICK)
	{
		m_nick = command.section(' ', 1);
		emit infoChanged();
	}
	else if (words[0] == "$Lock")
	{
		m_lock = words[1];
		if(m_lock.startsWith("EXTENDEDPROTOCOL"))
			extendedClient = true;

		if(m_direction == Listening)
		{
			m_stream << MYNICK << " " << m_server->me()->nick << "|";
			m_stream << "$Lock EXTENDEDPROTOCOLsomething Pk=DavePlusPlus|";
			if(extendedClient)
				m_stream << SUPPORTS << " BZList XmlBZList |";
			
			if (m_server->isFileQueuedFrom(m_nick))
				m_stream << "$Direction Download " << Utilities::randomDirection() << "|";
			m_stream << "$Key " << Utilities::lockToKey(m_lock) << "|";
			m_stream.flush();
		}
	}
	else if (words[0] == SUPPORTS)
	{
		if(words[1].contains("BZList"))
			supportsBZList = true;
		if(words[1].contains("XmlBZList"))
			supportsXmlBZList = true;
	}
	else if (words[0] == DIRECTION)
	{
	}
	else if (words[0] == "$Key")
	{
		if(m_passive)
			return;

		if(extendedClient)
		{
			// XmlBZList implies support for UGetBlock
			m_stream << SUPPORTS << " BZList XmlBZList |";
			//m_stream << SUPPORTS << " BZList |";
		}
		qDebug() << "Sending key:" << Utilities::lockToKey(m_lock);
		m_stream << DIRECTION << " Upload 1234|";
		m_stream << "$Key " << Utilities::lockToKey(m_lock) << "|";
		m_stream.flush();
	}
	else if (words[0] == GET || words[0] == UGETBLOCK)
	{
		if(words[0] == GET)
		{
			QString temp = command.section(" ", 1);
			m_fileName = temp.section("$", 0, 0);
			m_offset = temp.section("$", 1).toLongLong() - 1;
		}
		else
		{
			m_offset = words[1].toULongLong();
			m_fileName = command.section(" ", 3);
			m_numbytes = words[2].toLongLong();;
		}
		
		emit infoChanged();
		
		bool dcLst;
		if(m_fileName == "MyList.DcLst" || m_fileName == "MyList.bz2" || m_fileName == "files.xml.bz2")
			dcLst = true;
		else
			dcLst = false;

		if(!dcLst && !gotSlot)
		{
			if (!Configuration::instance()->getSlot())
			{
				m_stream << MAXEDOUT << "|";
				m_stream.flush();
				m_error = "No slots";
			
				emit result(TransferFailed);
				changeState(NoSlots);
				endTransfer();
				return;
			}
			else
				gotSlot = true;
		}
		
		FileListBuilder* builder = FileListBuilder::instance();
		if (m_fileName == "MyList.DcLst")
			m_fileLength = builder->huffmanList().length();
		else if(m_fileName == "MyList.bz2")
			m_fileLength = builder->bzList().length();
		else if(m_fileName == "files.xml.bz2")
			m_fileLength = builder->xmlBZList().length();
		else
		{
			QString fileName = Configuration::instance()->sharedFilename(m_fileName);
			m_fileLength = QFileInfo(fileName).size();
			m_file.setFileName(fileName);
			if (m_file.exists())
			{
				m_file.open(QIODevice::ReadOnly);
				m_file.seek(m_offset);
			}
		}
		
		if (m_fileLength <= 0)
		{
			if(words[0] == GET)
				m_stream << "$Error File Not Available|";
			else
				m_stream << "$Failed File Not Available|";

			m_error = "File not found";
			emit result(TransferFailed);
			qDebug() << "File not found";
			m_stream.flush();
			changeState(Failed);
			endTransfer();
			return;
		}
		
		if(words[0] == GET)
		{
			m_stream << FILELENGTH << " " << m_fileLength << "|";
			m_numbytes = m_fileLength - m_offset;
		}
		else
		{
			if(m_numbytes < 0)
				m_stream << "$Sending|"; // Undocumented, unless you count the source as doc
			else
				m_stream << "$Sending " << m_numbytes << "|";

			if(m_numbytes + m_offset > m_fileLength || m_numbytes < 0)
				m_numbytes = m_fileLength - m_offset;

			changeState(Transferring);
			m_sendPos = m_offset;
			m_stream.flush();
			realBytesWritten = 0;
			sendSomeData();
		}
		
		m_stream.flush();

		qDebug() << "Uploading" << m_fileName << "from position" << m_offset << "(length" << m_numbytes << ")";
	}
	else if (words[0] == SEND)
	{
		changeState(Transferring);
		m_sendPos = m_offset;
		realBytesWritten = 0;
		sendSomeData();
	}
	else
	{
		qDebug() << words[0] << "not understood";
		changeState(Failed);
		endTransfer();
	}
}




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
#include "utilities.h"
#include "server.h"
#include "user.h"
#include "configuration.h"
#include "filelist.h"

#include <QFileInfo>
#include <QFile>
#include <QStringList>
#include <QDebug>
#include <QDir>

MagicalSheep::MagicalSheep(Server* parent, QString host, quint16 port)
 : QObject(parent), m_socket(NULL), m_server(parent)
{
	if (host.isNull())
	{
		m_direction = Listening;
		setState(WaitingForConnection);
	}
	else
	{
		m_direction = Connecting;
		setState(LookingUpHost);
		
		m_socket = new QTcpSocket(this);
		m_socket->connectToHost(host, port);
		setupSocket();
	}
	
	m_transfer = new Transfer();
	m_transfer->baaaaa(this);
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
	setState(Handshaking);
	
	if (m_direction != Connecting)
		return;
	
	m_stream << MYNICK << " " << m_server->me()->nick << "|";
	m_stream << "$Lock EXTENDEDPROTOCOLsomething Pk=DavePlusPlus|";
	m_stream.flush();
}

void MagicalSheep::socketDisconnected()
{
	qDebug() << "Socket disconnected";
	setState(Finished);
}

void MagicalSheep::socketError(QAbstractSocket::SocketError error)
{
	qDebug() << "Socket error" << error;
}

void MagicalSheep::socketHostFound()
{
	qDebug() << "Socket host found";
	setState(ConnectingToHost);
}

void MagicalSheep::socketBytesWritten(quint64 num)
{
	qDebug() << "Socket bytes written" << num;
}

void MagicalSheep::socketReadyRead()
{
	qDebug() << "Socket ready read";


	if(m_state == Transferring)
	{
		m_bytesRead += m_socket->bytesAvailable();
		if(m_dcLst)
			m_lstBuffer.append(m_socket->readAll());
		else
			m_file.write(m_socket->readAll());

		if(m_bytesRead >= m_length)
		{
			emit result(TransferSucceeded);
			m_socket->close();
			if(m_dcLst && m_user)
			{
				QByteArray decodedList;

				if(m_supportsBZList || m_supportsXmlBZList)
					decodedList = Utilities::decodeBZList(m_lstBuffer);
				else
					decodedList = Utilities::decodeList(m_lstBuffer);

				if(m_supportsXmlBZList)
					m_user->setFileList(new FileList(decodedList));
				else
				{
					QTextStream stream(decodedList);
					m_user->setFileList(new FileList(&stream));
				}
			}
		}
		return;
	}

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
//		qDebug() << "DATA TO BUFFER" << data;
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
		QString nick = command.section(' ', 1);
		m_user = m_server->getUser(nick);
		if(!m_user)
			qWarning() << "User not connected to hub";

		m_transfer->setUserName(nick);
	}
	else if (words[0] == "$Lock")
	{
		m_lock = words[1];
		if(m_lock.startsWith("EXTENDEDPROTOCOL"))
			m_extendedClient = true;
		m_weWant = m_server->isFileQueuedFrom(m_transfer->userName());

		if(m_direction == Listening)
		{
			m_stream << MYNICK << " " << m_server->me()->nick << "|";
			m_stream << "$Lock EXTENDEDPROTOCOLsomething Pk=DavePlusPlus|";
			if(m_extendedClient)
				m_stream << SUPPORTS << " BZList XmlBZList |";

			m_ourRandom = Utilities::randomDirection();
			
			if (m_weWant)
				m_stream << DIRECTION << " Download " << m_ourRandom << "|";
			else
				m_stream << DIRECTION << " Upload " << m_ourRandom << "|";

			m_stream << "$Key " << Utilities::lockToKey(m_lock) << "|";
			m_stream.flush();
		}
	}
	else if (words[0] == SUPPORTS)
	{
		if(words.contains("BZList"))
			m_supportsBZList = true;
		if(words.contains("XmlBZList"))
			m_supportsXmlBZList = true;
	}
	else if (words[0] == DIRECTION)
	{
		if(words[1] == "Download")
			m_theyWant = true;
		else
			m_theyWant = false;
		
		m_theirRandom = words[2].toInt();

		if (m_direction == Listening)
		{
			negotiateDirection();
		}
	}
	else if (words[0] == "$Key")
	{
		if(m_direction == Listening)
		{
			if (m_dataDirection == Downloading)
				startDownload();
			return;
		}

		if(m_extendedClient)
		{
			// XmlBZList implies support for UGetBlock
			m_stream << SUPPORTS << " BZList XmlBZList |";
		}

		m_ourRandom = Utilities::randomDirection();
		
		if(m_weWant)
			m_stream << DIRECTION << " Download " << m_ourRandom << "|";
		else
			m_stream << DIRECTION << " Upload " << m_ourRandom << "|";

		m_stream << "$Key " << Utilities::lockToKey(m_lock) << "|";
		m_stream.flush();
		
		if (m_direction == Connecting)
		{
			negotiateDirection();
			
			if (m_dataDirection == Downloading)
				startDownload();
		}
	}
	else if (words[0] == GET || words[0] == UGETBLOCK)
	{
		bool uget;
		QString fileName;
		if(words[0] == GET)
		{
			uget = false;
			QString temp = command.section(" ", 1);
			fileName = temp.section("$", 0, 0);
			m_offset = temp.section("$", 1).toLongLong() - 1;
		}
		else
		{
			uget = true;
			m_offset = words[1].toULongLong();
			fileName = command.section(" ", 3);
			m_numbytes = words[2].toLongLong();;
		}
		
		m_transfer->setFileName(fileName);
		
		if(m_transfer->fileName() == "MyList.DcLst" || m_transfer->fileName() == "MyList.bz2" || m_transfer->fileName() == "files.xml.bz2")
			m_dcLst = true;
		else
			m_dcLst = false;

		if(!m_dcLst && !m_gotSlot)
		{
			if (!Configuration::instance()->getSlot())
			{
				m_stream << MAXEDOUT << "|";
				m_stream.flush();
				m_error = "No slots";
			
				emit result(TransferFailed);
				return;
			}
			else
				m_gotSlot = true;
		}
		
		FileListBuilder* builder = FileListBuilder::instance();
		if (m_transfer->fileName() == "MyList.DcLst")
			m_length = builder->huffmanList().length();
		else if(m_transfer->fileName() == "MyList.bz2")
			m_length = builder->bzList().length();
		else if(m_transfer->fileName() == "files.xml.bz2")
			m_length = builder->xmlBZList().length();
		else
		{
			QString fileName = Configuration::instance()->sharedFilename(m_transfer->fileName());
			m_length = QFileInfo(fileName).size();
			m_file.setFileName(fileName);
			if (m_file.exists())
			{
				m_file.open(QIODevice::ReadOnly);
				m_file.seek(m_offset);
			}
		}
		
		if (m_length <= 0)
		{
			if(words[0] == GET)
				m_stream << "$Error File Not Available|";
			else
				m_stream << "$Failed File Not Available|";

			m_error = "File not found";
			emit result(TransferFailed);
			qDebug() << "File not found";
			m_stream.flush();
			return;
		}
		
		if(!uget)
		{
			m_stream << FILELENGTH << " " << m_length << "|";
			m_numbytes = m_length - m_offset;
		}
		else
		{
			if(m_numbytes < 0)
				m_stream << "$Sending|"; // Undocumented, unless you count the source as doc
			else
				m_stream << "$Sending " << m_numbytes << "|";

			if(m_numbytes + m_offset > m_length || m_numbytes < 0)
				m_numbytes = m_length - m_offset;

			m_sendPos = m_offset;
			m_stream.flush();
			sendSomeData();
		}
		
		m_stream.flush();

		qDebug() << "Uploading" << m_transfer->fileName() << "from position" << m_offset << "(length" << m_numbytes << ")";
	}
	else if (words[0] == FILELENGTH)
	{
		m_length = words[1].toUInt();
		m_offset = 1;
		
		m_stream << SEND << "|";
		m_stream.flush();
		
		m_buffer.clear();
		setState(Transferring);
		
		qDebug() << "Opening file";
		
		if (!m_dcLst)
		{
			QString trimmedFileName = m_transfer->fileName();
			trimmedFileName.replace('\\', '/');
			int pos = trimmedFileName.lastIndexOf('/');
			if (pos != -1)
				trimmedFileName = m_transfer->fileName().mid(pos+1);
			
			m_file.setFileName(m_transfer->destination() + QDir::separator() + trimmedFileName);

			if(m_file.exists() && (m_offset == 1))
				m_file.remove();
			
			m_file.open(QIODevice::WriteOnly | QIODevice::Append);
		}
		m_bytesRead = 0;
	}
	else if (words[0] == SEND)
	{
		setState(Transferring);
		m_sendPos = m_offset;
		sendSomeData();
	}
	else
	{
		qDebug() << words[0] << "not understood";
	}
}

void MagicalSheep::negotiateDirection()
{
	if(m_weWant)
	{
		if(m_theyWant && (m_ourRandom < m_theirRandom))
			m_dataDirection = Uploading;
		else
			m_dataDirection = Downloading;
	}
	else
		m_dataDirection = Uploading;
	
	 qDebug() << "We are " << ((m_dataDirection == Uploading) ? "Uploading" : "Downloading");
	
	if (m_dataDirection == Downloading)
	{
		QString userName = m_transfer->userName();
		
		delete m_transfer;
		m_transfer = m_server->firstFileQueuedFrom(userName);
		m_transfer->baaaaa(this);
	}
}

void MagicalSheep::startDownload()
{
	if (m_transfer->isFileList())
	{
		if (m_supportsBZList)
			m_transfer->setFileName("MyList.bz2");
		else if (m_supportsXmlBZList)
			m_transfer->setFileName("files.xml.bz2");
		else
			m_transfer->setFileName("MyList.DcLst");
	}
	
	m_stream << "$Get " << m_transfer->fileName() << "$1|";
	m_stream.flush();
}

void MagicalSheep::sendSomeData()
{
	if (m_socket->state() != QAbstractSocket::ConnectedState)
	{
		qDebug() << "Sending failed (connection closed)";
		emit result(TransferFailed);
		return;
	}
	
	if (m_sendPos >= m_numbytes + m_offset)
	{
		qDebug() << "Sending finished (" << m_sendPos << m_length << ")";
		emit result(TransferSucceeded);
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

//	qDebug() << "Slots in use:" << slotsInUse;
//	qDebug() << "Upload Speed:" << uploadSpeed;
	
	int uploadBytes = uploadSpeed * 1024;
	if(uploadBytes > m_numbytes)
		uploadBytes = m_numbytes;
	
	if (m_transfer->fileName() == "MyList.DcLst")
		data = FileListBuilder::instance()->huffmanList().mid(m_sendPos, uploadBytes);
	else if(m_transfer->fileName() == "MyList.bz2")
		data = FileListBuilder::instance()->bzList().mid(m_sendPos, uploadBytes);
	else if(m_transfer->fileName() == "files.xml.bz2")
		data = FileListBuilder::instance()->xmlBZList().mid(m_sendPos, uploadBytes);
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


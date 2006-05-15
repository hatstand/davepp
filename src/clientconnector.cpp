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
 : Client(server),
 	extendedClient(false),
	supportsBZList(false),
	supportsXmlBZList(false),
	gotSlot(false)
{
	m_socket = new QTcpSocket(this);
	m_timer = new QTimer(this);
	
	connect(m_socket, SIGNAL(hostFound()), SLOT(socketHostFound()));
	connect(m_socket, SIGNAL(connected()), SLOT(socketConnected()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(readyRead()), SLOT(socketReadyRead()));
	connect(m_socket, SIGNAL(bytesWritten(qint64)), SLOT(socketBytesWritten(qint64)));
	connect(m_socket, SIGNAL(disconnected()), SLOT(socketDisconnected()));

	connect(m_timer, SIGNAL(timeout()), SLOT(sendSomeData()));
	m_timer->setSingleShot(true);
	
	m_stream.setDevice(m_socket);
	
	changeState(Idle);
}


ClientConnector::~ClientConnector()
{
	if(gotSlot)
		Configuration::instance()->revokeSlot();
}

void ClientConnector::connectToClient(QString hostName, quint16 port)
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
	
	m_stream << MYNICK << " " << m_server->me()->nick << "|";
	m_stream << "$Lock EXTENDEDPROTOCOLsomething Pk=DavePlusPlus|";
	m_stream.flush();
}

void ClientConnector::socketError(QAbstractSocket::SocketError err)
{
/*	if((err == QAbstractSocket::SocketTimeoutError || err == QAbstractSocket::ConnectionRefusedError) && state() != Failed) // Timeout, might be NAT'ed, let's try the Hub's IP
	{
		qDebug() << "Socket connection failed. Getting Remote IP";
		
		changeState(Failed);
		connect(m_server, SIGNAL(gotUserIP(QString, QString)), SLOT(gotUserIP(QString, QString)));
		m_server->getUserIP(m_nick);

		m_timer->disconnect();
		connect(m_timer, SIGNAL(timeout()), SLOT(endTransfer()));
		m_timer->start(3000);

		return;
	} */ // Lets fix other bugs first

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

	endTransfer();
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
			if(m_lock.startsWith("EXTENDEDPROTOCOL"))
				extendedClient = true;
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
			if(extendedClient)
			{
				// XmlBZList implies support for UGetBlock
				m_stream << SUPPORTS << " BZList XmlBZList |";
//				m_stream << SUPPORTS << " BZList |";
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
				sendSomeData();
			}
			
			m_stream.flush();

			qDebug() << "Uploading" << m_fileName << "from position" << m_offset << "(length" << m_numbytes << ")";
		}
		else if (words[0] == SEND)
		{
			changeState(Transferring);
			m_sendPos = m_offset;
//			qDebug() << "**** Send" << m_offset << m_sendPos;
			sendSomeData();
		}
		else
		{
			qDebug() << words[0] << "not understood";
			changeState(Failed);
			endTransfer();
		}
	}
}

void ClientConnector::sendSomeData()
{
	// qDebug() << "SendSomeData()";
	if (m_socket->state() != QAbstractSocket::ConnectedState)
	{
		qDebug() << "Sending failed (connection closed)";
		changeState(Failed);
		endTransfer();
		return;
	}
	
	if (m_sendPos >= m_numbytes + m_offset - 1)
	{
		qDebug() << "Sending finished (" << m_sendPos << m_fileLength << ")";
		changeState(Success);
		endTransfer();
		return;
	}
	
	QByteArray data;
	int uploadSpeed;
	
	int slotsInUse = Configuration::instance()->slotsInUse(); 
	if(slotsInUse > 0)
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
	
	if (m_fileName == "MyList.DcLst")
		data = FileListBuilder::instance()->huffmanList().mid(m_sendPos, uploadBytes);
	else if(m_fileName == "MyList.bz2")
		data = FileListBuilder::instance()->bzList().mid(m_sendPos, uploadBytes);
	else if(m_fileName == "files.xml.bz2")
		data = FileListBuilder::instance()->xmlBZList().mid(m_sendPos, uploadBytes);
	else
		data = m_file.read(uploadBytes);
	
	m_sendPos += m_socket->write(data);
	m_sendTimer.start();
	m_socket->flush();
	
	// qDebug() << "Writing" << data.size() << "bytes";
}

void ClientConnector::socketBytesWritten(qint64 num)
{
	// qDebug() << "Bytes written" << num;
	if (state() != Transferring)
		return;
	
	m_speedCount += num;
	
	if (m_socket->bytesToWrite() > 0)
		return;
	
	emit progress(m_sendPos - m_offset + 1, m_numbytes);
	
	if ((Configuration::instance()->uploadSpeed() != 0) && (m_sendTimer.elapsed() < 1000))
	{
		m_timer->start(1000 - m_sendTimer.elapsed());
//		QTimer::singleShot(1000 - m_sendTimer.elapsed(), this, SLOT(sendSomeData()));
	}
	else
		sendSomeData();
}

void ClientConnector::socketDisconnected()
{
	qDebug() << "Disconnected";

	if(m_sendPos < m_offset + m_numbytes)
	{
		changeState(Failed);
		endTransfer();
	}
	else
	{
		changeState(Success);
		endTransfer();
	}
}

void ClientConnector::gotUserIP(QString host, QString nick)
{
	if(state() == Failed && nick == m_nick)
	{
		m_timer->stop();
		m_timer->disconnect();
		connect(m_timer, SIGNAL(timeout()), SLOT(sendSomeData()));
		connectToClient(host, m_port);
	}
}

void ClientConnector::endTransfer()
{
	qDebug() << "Ending Transfer";
	if(state() == Success)
		emit result(TransferSucceeded);
	else
		emit result(TransferFailed);

	m_socket->disconnect();
	
	m_socket->flush();
	m_socket->abort();
	deleteLater();
}

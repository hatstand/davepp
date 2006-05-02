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
#include "server.h"
#include "clientconnector.h"
#include "clientlistener.h"
#include "utilities.h"
#include "filelist.h"
#include "user.h"
#include "client.h"
#include "configuration.h"
#include "filelistbuilder.h"
#include "searchreturner.h"

#include <QStringList>
#include <QUdpSocket>
#include <QDebug>

Server::Server(QObject *parent)
	: QObject(parent), m_state(NotConnected)
{
	m_socket = new QTcpSocket(this);
	//udp_socket = new QSocketDevice(QSocketDevice::Datagram, QSocketDevice::IPv4, 0);
	connect(m_socket, SIGNAL(hostFound()), SLOT(socketHostFound()));
	connect(m_socket, SIGNAL(connected()), SLOT(socketConnected()));
	connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
	connect(m_socket, SIGNAL(readyRead()), SLOT(socketReadyRead()));
	connect(m_socket, SIGNAL(bytesWritten(qint64)), SLOT(socketBytesWritten(qint64)));
	connect(m_socket, SIGNAL(disconnected()), SLOT(socketDisconnected()));
	
	//myAddress = m_socket->localAddress();
	
	m_stream.setDevice(m_socket);
	
	Configuration* config = Configuration::instance();
	connect(config, SIGNAL(nickChanged(QString)), SLOT(nickChanged(QString)));
	m_me = new User(this, config->nick());
	m_me->setInterest(config->description());
	m_me->speed = config->connSpeedString();
	m_me->email = config->email();
	
	FileListBuilder* builder = FileListBuilder::instance();
	m_me->setShareSize(builder->totalSize());
	connect(builder, SIGNAL(finished()), SLOT(fileListUpdateFinished()));
}


Server::~Server()
{
	disconnectFromHub(); // Gracefully disconnect
}

void Server::setHost(QString host, int port)
{
	m_host = host;
	m_port = port;
}

void Server::open()
{
	qDebug() << "Connecting...";

	qDebug() << m_host;
	changeState(LookingUpHost);
	m_socket->connectToHost(m_host, m_port);
}

void Server::socketHostFound()
{
	//printf("Host found\n");
	qDebug() << "Host found";
	changeState(Connecting);
}

void Server::socketConnected()
{
	//printf("Connected\n");
	qDebug() << "Connected";
	changeState(LoggingIn);
}

void Server::socketError(QAbstractSocket::SocketError err)
{
	changeState(NotConnected);
	qDebug() << "Error" << err << m_socket->errorString();
	emit error(m_socket->errorString());
}

void Server::socketReadyRead()
{
	QString data = m_socket->readAll();
//	qDebug() << "RAW:#" << data << "#";
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
		m_buffer += data;
	}
}

void Server::socketBytesWritten(qint64 num)
{
	qDebug() << num << " bytes written";
}

void Server::parseCommand(QString command)
{
	if(command == "")
		return;

	qDebug() << "Command: " << command;
	if (command.startsWith("$"))
	{
		QStringList words = command.split(" ");
		if (words[0] == "$HubName")
		{
			m_hubName = command.mid(9);
		}
		else if(words[0] == "$Lock")
		{
			QString lock = words[1];
				
			qDebug() << "Sending key:" << Utilities::lockToKey(lock);
			m_stream << "$Key " << Utilities::lockToKey(lock) << "|";
			m_stream << "$ValidateNick " << m_me->nick << "|";
			m_stream.flush();

		}
		else if (words[0] == "$ValidateDenide")
		{
			qWarning() << "Nick already requested or reserved";
			m_socket->disconnectFromHost();
		}
		else if (words[0] == "$Hello")
		{
			if (words[1] == m_me->nick)
			{
				qDebug() << "Logged in";
				changeState(Connected);

				qDebug() << "Sending login info";
				m_stream << "$Version 1,0091|";
				m_stream.flush();
				sendInfo();
				getNickList();
			}
			
			// Remove the user if he already exists
			userQuit(words[1]);
			
			// Create the user
			User* user = new User(this, words[1]);
			m_users << user;
			if (words[1] == m_me->nick)
				user->me = true;
			emit userJoined(user);
		}
		else if(words[0] == "$NickList")
		{
			QStringList nicks = words[1].split("$$", QString::SkipEmptyParts);
			qDebug() << "Nicks:";
			foreach (QString nick, nicks)
			{
				// Skip if the user already exists
				User* user = getUser(nick);
				if (user != NULL)
					continue;
				
				// Create the user
				user = new User(this, nick);
				m_users << user;
				if (nick == m_me->nick)
					user->me = true;
				emit userJoined(user);
			}
			getInfoAll();
		}
		else if(words[0] == "$OpList")
		{
			QStringList ops = words[1].split("$$", QString::SkipEmptyParts);
			qDebug() << "Ops:";
			foreach (QString nick, ops)
			{
				User* user = getUser(nick);
				if (user != NULL)
				{
					if (user->op)
						continue;
					user->op = true;
					emit userInfoChanged(user);
					break;
				}
				
				user = new User(this, nick);
				user->op = true;
				m_users << user;
				if (nick == m_me->nick)
					user->me = true;
				emit userJoined(user);
			}
		}
		else if(words[0] == "$MyINFO")
		{
			QString therest = command.section(' ', 2);
			QString nick = therest.split(' ')[0];
			QString others = therest.section(nick + " ", 1);
			QStringList otherList = others.split('$');
			
			User* user = getUser(nick);
			bool newUser = false;
			if (user == NULL)
			{
				newUser = true;
				user = new User(this, nick);
				if (nick == m_me->nick)
					user->me = true;
				m_users << user;
			}
			
			user->setInterest(otherList[0]);
			user->speed = otherList[2].trimmed();
			user->email = otherList[3];
			user->setShareSize(otherList[4].toLongLong());

			if (newUser)
				emit userJoined(user);
			else
				emit userInfoChanged(user);
		}
		else if(words[0] == "$Quit")
		{
			userQuit(words[1]);
		}
		else if(words[0] == "$To:")
		{
			QString othernick = words[3];
			QString message = decodeChatMessage(command.section('$',2));

			emit chatMessage(othernick, message, true);
		}
		else if(words[0] == "$ConnectToMe")
		{
			QString host = words[2].section(':', 0, 0);
			quint16 port = words[2].section(':', 1).toUShort();
			
			qDebug() << "New connector" << (uintptr_t)this;
			ClientConnector* client = new ClientConnector(this);
			client->connectToClient(host, port);
			emit uploadRequest(client);
		}
		else if(words[0] == "$UserIP")
		{
			emit gotUserIP(words[1], words[2]);
		}
		else if(words[0] == "$Search")
		{
			QStringList temp = words[1].split(":");

			QString searchstring = words[2];
			bool sizeRestricted;
			bool isMaxSize;
			if(searchstring.startsWith("F"))
			{
				sizeRestricted = false;
				isMaxSize = false;
			}
			else
				sizeRestricted = true;

			temp = searchstring.split("?");
			quint64 size = temp[2].toULongLong();
			int datatype = temp[3].toInt();
			QString pattern = temp[4];
			
			if(words[1].startsWith("Hub")) // Passive request
			{
				SearchReturner* x = new SearchReturner(this, temp[0], sizeRestricted, isMaxSize, size, datatype, pattern);
				// Cross thread signal/slot
				connect(x, SIGNAL(passiveSearchResult(QString)), this, SLOT(passiveSearchResult(QString)), Qt::QueuedConnection);
			}
			else
			{
				QHostAddress* requestor = new QHostAddress(temp[0]);
				quint16 port = temp[1].toUShort();
				new SearchReturner(this, *requestor, port, sizeRestricted, isMaxSize, size, datatype, pattern);
			}
		}
		else if (words[0].startsWith("$HubIsFull"))
		{
			changeState(HubFull);
		}
		else if (words[0].startsWith("$UserIP"))
		{
			emit gotUserIP(words[1], words[2]);
		}
		else
			qDebug() << words[0] << "not understood";
		
		m_lastCommand = words;
	}
	else if (command.startsWith("<"))
	{
		QRegExp regExp("<([^>]*)>( ?)(.*)");
		if (!regExp.exactMatch(command))
			return;
		
		QString nick = regExp.cap(1);
		QString message = decodeChatMessage(regExp.cap(3));
		
		emit chatMessage(nick, message, false);
	}
}

void Server::getNickList()
{
	m_stream << "$GetNickList|";
	m_stream.flush();
}

void Server::getInfo(QString othernick)
{
	m_stream << "$GetINFO " << othernick << " " << m_me->nick << "|";
	m_stream.flush();
}

void Server::getInfoAll()
{
	foreach (User* user, m_users)
	{
		getInfo(user->nick);
	}
}

User* Server::getUser(QString nick)
{
	//for (QList<User*>::iterator it = m_users.begin() ; it != m_users.end() ; ++it)
	foreach (User* user, m_users)
	{
		if (user->nick == nick)
			return user;
	}
	return NULL;
}

void Server::userQuit(QString nick)
{
	User* user = getUser(nick);
	if (user == NULL)
		return;
	
	m_users.removeAll(user);
	emit userQuit(user);
	delete user;
}

void Server::sendInfo()
{
	m_stream << "$MyINFO $ALL " << m_me->nick << " " << m_me->interest << "$ $" << m_me->speed << "$" << m_me->email << "$" << QString::number(m_me->shareSize) << "$|";
	m_stream.flush();
}

void Server::socketDisconnected()
{
	//printf("Connection closed\n");
	qDebug() << "Connection closed";
	changeState(NotConnected);
	
	foreach (User* user, m_users)
	{
		m_users.removeAll(user);
		emit userQuit(user);
		delete user;
	}
}

void Server::sendMessage(QString message, QString othernick)
{
	if (othernick.isNull())
	{
		m_stream << "<" << m_me->nick << "> " << encodeChatMessage(message) << "|";
	}
	else
	{
		m_stream << "$To:" << othernick << " From:" << m_me->nick << " $<" << m_me->nick << "> ;" << encodeChatMessage(message) << "|";
	}
	m_stream.flush();
}

void Server::disconnectFromHub()
{
	if(m_socket->isValid())
	{
		m_stream << "$Quit " << m_me->nick << "|";
		m_stream.flush();
	
		m_socket->close();
	}
}

void Server::searchHub(quint16 port, QString search, bool sizerestricted, bool isminimumsize, quint64 size, int datatype)
{
	char sizerestrictedchar = 'F';
	char isminimumsizechar = 'F';
	if(sizerestricted)
		sizerestrictedchar = 'T';
	
	if(isminimumsize)
		isminimumsizechar = 'T';
	
	m_stream << "$Search " << m_socket->localAddress().toString() << ":" << port << " " << sizerestrictedchar << "?" << isminimumsizechar << "?" << size << "?" << datatype << "?" << search << "|";
	m_stream.flush();
}


void Server::udpsocketReadyRead()
{
	//Do Nothing
}

void Server::changeState(ConnectionState newState)
{
	m_state = newState;
	emit stateChanged(newState);
}

Server::ConnectionState Server::state()
{
	return m_state;
}

QString Server::decodeChatMessage(QString message)
{
	QString ret = message;
	ret.replace("&#124;", "|");
	ret.replace("&#36;", "$");
	return ret;
}

QString Server::encodeChatMessage(QString message)
{
	QString ret = message;
	ret.replace("|", "&#124;");
	ret.replace("$", "&#36;");
	return ret;
}

ClientListener* Server::browseFiles(QString nick)
{
	qDebug() << "Browse" << nick;
	
	ClientListener* listener = new ClientListener(this, getUser(nick));
	listener->listenForClients(1234);
	
	connect(listener, SIGNAL(stateChanged(int)), SLOT(listenerStateChanged(int)));
	connect(listener, SIGNAL(result(int)), SLOT(listenerResult(int)));
	
	m_stream << "$ConnectToMe " << nick << " " << m_socket->localAddress().toString() << ":1234|";
	m_stream.flush();
	
	return listener;
}

ClientListener* Server::downloadFile(QString nick, QString filename)
{
	qDebug() << "Attempting to download" << filename;
	
	ClientListener* listener = new ClientListener(this, getUser(nick), filename, Configuration::instance()->downloadDir());
	quint16 realPort = listener->listenForClients(1234);
	
	connect(listener, SIGNAL(stateChanged(int)), SLOT(listenerStateChanged(int)));
	connect(listener, SIGNAL(result(int)), SLOT(listenerResult(int)));
	
	m_stream << "$ConnectToMe " << nick << " " << m_socket->localAddress().toString() << ":" << realPort << "|";
	m_stream.flush();
	
	return listener;
}

void Server::listenerStateChanged(int state)
{
	qDebug() << "Listener state changed to" << state;
}

void Server::listenerResult(int result)
{
	ClientListener* listener = (ClientListener*) sender();
	if (result == ClientListener::TransferSucceeded)
	{
		if (listener->isFileList())
			emit fileListUpdated(listener->user());
	}
}

QString Server::ip()
{
	return m_socket->peerAddress().toString();
}

quint16 Server::port()
{
	return m_port;
}

void Server::fileListUpdateFinished()
{
	m_me->setShareSize(FileListBuilder::instance()->totalSize());
	if (state() == Connected)
		sendInfo();
}

void Server::getUserIP(QString nick)
{
	m_stream << "$UserIP " << nick << "|";
	m_stream.flush();
}

void Server::nickChanged(QString newnick)
{
	m_me->nick = newnick;
}

void Server::passiveSearchResult(QString result)
{
//	qDebug() << "Sending passive search result";
	m_stream << result; 
	m_stream.flush();
}

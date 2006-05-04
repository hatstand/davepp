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
#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTextStream>
#include <QUrl>
#include <QStringList>
#include <QList>
#include <QListIterator>

class User;
class Client;
class ClientListener;
class Configuration;
class ClientConnector;

/**
@author David Sansome
*/
class Server : public QObject
{
	Q_OBJECT
	
public:
	Server(QObject *parent = 0);
	~Server();
	
	void setHost(QString host, int port);
	void open();
	
	User* getUser(QString nick);
	void sendMessage(QString message, QString othernick = QString::null);
	ClientListener* browseFiles(QString nick);
	ClientListener* downloadFile(QString nick, QString filename);
	void disconnectFromHub();
	void searchHub(quint16 port, QString search, bool sizerestricted = FALSE, bool isminimumsize = TRUE, quint64 size = 0, int datatype = 1);
	QString hubName() {return m_hubName;}
	QString ip();
	quint16 port();
	User* me() {return m_me;}
	int userCount() {return m_users.count();}
	QListIterator<User*> users() {return QListIterator<User*>(m_users);}
	
	enum ConnectionState
	{
		NotConnected,
		LookingUpHost,
		Connecting,
		LoggingIn,
		Connected,
		HubFull
	};
	
	ConnectionState state();
	
	void getUserIP(QString nick); // void return as result is emitted

private:
	void processChatCommand(QString nick, QString command, bool priv);
	void parseCommand(QString command);
	void searchResponse(QString othernick, QHostAddress ip, int port, bool sizerestricted, bool isminimumsize, int size, int datatype);
	void changeState(ConnectionState newState);
	void userQuit(QString nick);
	void getInfo(QString othernick);
	void getInfoAll();
	void getNickList();

	QString m_host;
	int m_port;
	QTcpSocket* m_socket;
	//QSocketDevice* udp_socket;
	QTextStream m_stream;
	ConnectionState m_state;
	
	QString m_hubName;
	QStringList m_lastCommand;

	User* m_me;
	
	QString m_buffer;

	QList<User*> m_users;
	
private slots:
	void socketHostFound();
	void socketConnected();
	void socketError(QAbstractSocket::SocketError err);
	void socketReadyRead();
	void socketBytesWritten(qint64 num);
	void socketDisconnected();
	void listenerResult(int result);
	void listenerStateChanged(int state);
	void fileListUpdateFinished();
	void nickChanged(QString newnick);
	void passiveSearchResult(QString result);
	void sendInfo();
	
signals:
	void stateChanged(int newState);
	void userJoined(User* user);
	void userInfoChanged(User* user);
	void userQuit(User* user);
	void chatMessage(QString from, QString message, bool priv);
	void error(QString message);
	void fileListUpdated(User* user);
	void uploadRequest(ClientConnector* connector);
	void gotUserIP(QString nick, QString host);
};

#endif

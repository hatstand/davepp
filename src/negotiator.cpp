#include "negotiator.h"

#include "commands.h"
#include "user.h"
#include "utilities.h"
#include "uploader.h"
#include "downloader.h"

#include <QStringList>
#include <QDebug>

Negotiator::Negotiator(Server* server):
	m_server(server),
	m_download(false),
	m_client(NULL)
{
}

Negotiator::Negotiator(Server* server, QString filename, quint64 offset, quint64 length):
	m_server(server),
	m_remoteFile(filename),
	m_remoteOffset(offset),
	m_remoteLength(length),
	m_download(true),
	m_client(NULL)
{
}

Negotiator::Negotiator(Server* server, bool dcLst):
	m_server(server),
	m_download(true),
	m_client(NULL)
{
}

quint16 Negotiator::listenForClient()
{
	qDebug() << "Listening for client";
	QTcpServer* thingy = new QTcpServer(this);
	connect(thingy, SIGNAL(newConnection()), SLOT(newConnection()));
	thingy->listen();

	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), SLOT(deleteLater()));
	m_timer->start(10000);

	return thingy->serverPort();
}

void Negotiator::connectToClient(QHostAddress host, quint16 port)
{
	m_sock = new QTcpSocket(this);
	connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
	connect(m_sock, SIGNAL(readyRead()), SLOT(socketReadyRead()));
	connect(m_sock, SIGNAL(connected()), SLOT(socketConnected()));
	m_sock->connectToHost(host, port);
}

void Negotiator::socketConnected()
{
	qDebug() << "Client connected";
	m_first = true;

	m_stream.setDevice(qobject_cast<QTcpSocket*>(sender()));
	m_stream << MYNICK << " " << m_server->me()->nick << "|";
	m_stream << "$Lock EXTENDEDPROTOCOLsomething Pk=DavePlusPlus|";
	m_stream.flush();
}

void Negotiator::newConnection()
{
	m_first = false;

	m_timer->disconnect();
	m_timer->stop();

	QTcpServer* thingy = qobject_cast<QTcpServer*>(sender());
	m_sock = thingy->nextPendingConnection();
	thingy->close();

	connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
	connect(m_sock, SIGNAL(readyRead()), SLOT(socketReadyRead()));
	m_stream.setDevice(m_sock);
}

void Negotiator::socketError(QAbstractSocket::SocketError err)
{
	qDebug() << "Negotiator socket error:" << err;
	qDebug() << "Giving up";

	deleteLater();
}

void Negotiator::socketReadyRead()
{
	QString data = m_sock->readAll();
	data = m_buffer + data;
	if(data.endsWith("|"))
	{
		QStringList commands = data.split("|");
		for(QStringList::iterator it = commands.begin(); it != commands.end(); ++it)
		{
			QString command = *it;
			parseClientCommand(command);
		}
		m_buffer = "";
	}
	else
	{
		m_buffer += data;
	}
}

void Negotiator::parseClientCommand(QString command)
{
	qDebug() << "CLIENT Command:" << command;
	if(command.startsWith("$"))
	{
		QStringList words = command.split(" ");
		if(words[0] == MYNICK)
		{
			m_nick = words[1];
		}
		else if(words[0] == "$Lock")
		{
			m_lock = words[1];
			if(m_lock.startsWith("EXTENDEDPROTOCOL"))
				m_extended = true;

			if(!m_first)
			{
				m_stream << MYNICK << " " << m_server->me()->nick << "|";
				m_stream << "$Lock EXTENDEDPROTOCOLsomething Pk=DavePlusPlus|";
				if(m_extended)
					m_stream << SUPPORTS << " BZList XmlBZList |";
				
				m_stream << DIRECTION;
				if(m_download)
					m_stream << " Download ";
				else
					m_stream << " Upload ";

				m_random = Utilities::randomDirection();
				m_stream << m_random << "|";
				m_stream << "$Key " << Utilities::lockToKey(m_lock) << "|";
				m_stream.flush();
			}
		}
		else if(words[0] == DIRECTION)
		{
			if(words[1] == "Download")
				m_clientDownload = true;
			else
				m_clientDownload = false;

			// Both want to download
			// Random number battle
			if(m_clientDownload && m_download)
			{
				if(m_random > words[1].toInt())
				{
					qDebug() << "Other client wants to download too, we won";
					m_downloadFirst = true;
				}
				else
				{
					qDebug() << "Other client wants to download too, they won";
					m_downloadFirst = false;
				}
			}
			else if(m_download)
				m_downloadFirst = true;
			else
				m_downloadFirst = false;
		}
		else if(words[0] == SUPPORTS)
		{
			if(!m_extended)
			{
				qWarning() << "ODD CLIENT! Not sure if it is extended or not!";
				m_extended = true;
			}
			
			if(words.contains("BZList"))
				m_supportsBZList = true;
			
			if(words.contains("XmlBZList"))
				m_supportsXmlBZList = true;

			if(m_download && m_remoteFile.isEmpty())
			{
				if(m_supportsXmlBZList)
					m_remoteFile = "files.xml.bz2";
				else if(m_supportsBZList)
					m_remoteFile = "MyList.bz2";
				else
					m_remoteFile = "MyList.DcLst";

				m_remoteLength = 0;
				m_remoteOffset = 0;
			}
		}
		else if(words[0] == "$Key")
		{
			if(m_first)
			{
				if(m_extended)
					m_stream << SUPPORTS << " BZList XmlBZList |";

				m_random = Utilities::randomDirection();
				m_stream << DIRECTION;
				if(m_download)
					m_stream << " Download ";
				else
					m_stream << " Upload ";

				m_stream << m_random << "|";
				m_stream << "$Key " << Utilities::lockToKey(m_lock) << "|";
				m_stream.flush();

			}

			if(m_downloadFirst)
			{
				m_stream << "$Get " << m_remoteFile << "$" << m_remoteOffset + 1 << "|";
				m_stream.flush();
			}
		}
		else if(words[0] == GET || words[0] == UGETBLOCK)
		{
			if(words[0] == GET)
				m_uget = false;
			else 
				m_uget = true;

			if(!m_uget)
			{
				QString temp = command.section(" ", 1);
				m_localFile = temp.section("$", 0, 0);
				m_localOffset = temp.section("$", 1).toULongLong() - 1;
				m_localLength = 0;
			}
			else
			{
				m_localOffset = words[1].toULongLong();
				m_localFile = command.section(" ", 3);
				qint64 temp = words[2].toLongLong();
				if(temp < 0)
					m_localLength = 0;
				else
					m_localLength = words[2].toULongLong();

				doUpload();
			}
		}
		else if(words[0] == SEND)
		{
			doUpload();
		}
		else if(words[0] == FILELENGTH)
		{
			m_remoteLength = words[1].toULongLong();
			m_stream << SEND << "|";
			doDownload();
		}
		else
		{
			qDebug() << "CLIENT command not understood";
		}
	}
}

void Negotiator::doUpload()
{
	m_sock->disconnect();
	m_client = new Uploader(m_server, m_sock, m_localFile, m_localOffset, m_localLength, m_uget);
	emit clientReady(m_client);
}

void Negotiator::doDownload()
{
	m_sock->disconnect();
	m_client = new Downloader(m_server, m_sock, m_remoteFile, m_remoteOffset, m_remoteLength);
	emit clientReady(m_client);
}



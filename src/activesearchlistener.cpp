#include "activesearchlistener.h"
#include <QHostAddress>
#include <QUdpSocket>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QList>

ActiveSearchListener::ActiveSearchListener(QObject* parent)
 : QObject(parent)
{
	m_socket = new QUdpSocket(this);

	quint16 port = 40000;
	while(!m_socket->bind(QHostAddress::Any, port))
		++port;

	connect(m_socket, SIGNAL(readyRead()), SLOT(socketReadyRead()));

	count = 0;
	timer = new QTimer(this);
	timer->start(1000);
	connect(timer, SIGNAL(timeout()), SLOT(timeout()));
}

ActiveSearchListener::~ActiveSearchListener()
{

}

void ActiveSearchListener::socketReadyRead()
{
	while (m_socket->hasPendingDatagrams())
	{
		QByteArray datagram;
		datagram.resize(m_socket->pendingDatagramSize());
		QHostAddress sender;
		quint16 senderPort;

		m_socket->readDatagram(datagram.data(), datagram.size(),
								&sender, &senderPort);

		m_buffer += datagram;
	}

	if(m_buffer.endsWith("|"))
	{
		QStringList commands = m_buffer.split("|");
		for ( QStringList::iterator it = commands.begin(); it != commands.end(); ++it )
		{
			QString command = (*it);
			parseCommand(command);
		}
		m_buffer = "";
	}
}

void ActiveSearchListener::parseCommand(QString command)
{
	//qDebug() << "SEARCH RESULT:" << command;
	
	if(command.startsWith("$SR"))
	{
		QRegExp re("^([0-9]*)/([0-9]*)\\x0005([^ ]*) \\(([^)]*)\\)$");
		int index = re.lastIndexIn(command.section(' ', -2, -1));
		if (index == -1)
		{
			qDebug() << "NO MATCH" << command;
			return;
		}
		
		int openslots = re.cap(1).toInt();
		int totalslots = re.cap(2).toInt();
		QString hash = re.cap(3);
		QString hubaddress = re.cap(4);
		
		QString nick = command.section(' ', 1, 1);
		QString file = command.section(' ', 2, -3);
		quint64 size = file.section('\05', 1, 1).toLongLong();
		file = file.section('\05', 0, 0);

		results.append(new SearchResult(nick, file, size, openslots, totalslots, hash, hubaddress));
	}
	else
		qDebug() << "Command not understood";
}

QList<SearchResult*> ActiveSearchListener::getList()
{
	return results;
}


void ActiveSearchListener::timeout()
{
	count++;
	
	if(!results.empty())
	{
		emit resultArrived(results);
		results.clear();
	}

}

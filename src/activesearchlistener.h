#ifndef ACTIVESEARCHLISTENER_H
#define ACTIVESEARCHLISTENER_H

#include "client.h"
#include "server.h"
#include "searchresult.h"

#include <QUdpSocket>
#include <QList>
#include <QObject>
#include <QTimer>


// Listens for the results of an active search initiated by *us*

class ActiveSearchListener : public QObject
{
Q_OBJECT

public:
	ActiveSearchListener(QObject* parent = 0);
	~ActiveSearchListener();
	QList<SearchResult*> getList();
	quint16 port() {return m_socket->localPort();}
	
private:
	void parseCommand(QString command);
	
	QUdpSocket* m_socket;
	QList<SearchResult*> results;
	Server* m_server;
	QString m_buffer;

	QTimer* timer;

signals:
	void resultArrived(QList<SearchResult*> results);

private slots:
	void timeout();
	void socketReadyRead();
};

#endif

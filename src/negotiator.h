#ifndef NEGOTIATOR_H
#define NEGOTIATOR_H

#include <QTcpSocket>
#include <QTextStream>
#include <QTimer>

#include "server.h"
#include "newclient.h"

class Negotiator: public QObject
{
	Q_OBJECT

	public:
	// Upload
	Negotiator(Server* server);
	// Download
	Negotiator(Server* server, QString filename, quint64 offset, quint64 length);
	// Download dclst
	Negotiator(Server* server, bool dclst);
	quint16 listenForClient();
	void connectToClient(QHostAddress host, quint16 port);
	NewClient* getClient() {return m_client;}

	private:
	void setup(QTcpSocket* sock, bool first);
	void doUpload();
	void doDownload();
	void parseClientCommand(QString command);

	private slots:
	void socketReadyRead();
	void socketConnected();
	void newConnection();
	void socketError(QAbstractSocket::SocketError err);

	signals:
	void clientReady(NewClient* client);

	private:
	Server* m_server;
	QTcpSocket* m_sock;
	QTextStream m_stream;
	NewClient* m_client;
	// We connected to them
	bool m_first;
	// We want to download
	bool m_download;
	// They want to download
	bool m_clientDownload;
	// We won the random number battle
	bool m_downloadFirst;
	// Random number used for negotiating who goes first
	int m_random;
	QString m_buffer;
	// Their lock
	QString m_lock;
	// The file we want
	QString m_remoteFile;
	quint64 m_remoteOffset;
	quint64 m_remoteLength;
	// The file they want
	QString m_localFile;
	quint64 m_localOffset;
	quint64 m_localLength;
	// Downloader is using UGetBlock
	bool m_uget;
	// Their nick
	QString m_nick;
	// They are an extended client
	bool m_extended;
	bool m_supportsBZList;
	bool m_supportsXmlBZList;

	QTimer* m_timer;
};

#endif

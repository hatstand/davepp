#ifndef SEARCHRETURNER_H
#define SEARCHRETURNER_H

#include <QHostAddress>
#include <QUdpSocket>
#include <QThread>
#include <QMutex>
#include "filelist.h"
#include "filelistbuilder.h"
#include "server.h"

class SearchReturner: public QThread
{
	Q_OBJECT
		 
	public:
	SearchReturner(Server* server, QHostAddress client, quint16 port, bool sizeRestricted, bool isMaxSize, quint64 size, int datatype, QString pattern);
	SearchReturner(Server* server, QString client, bool sizeRestricted, bool isMaxSize, quint64 size, int datatype, QString pattern);
	~SearchReturner();

	private:
	void begin();
	void run();
	void SearchDescend(FileNode* current);
	void SubmitResult(FileNode* node);
	
	FileList* m_list;
	FileListBuilder* m_builder;
	QUdpSocket* m_sock;
	QMutex* list_mutex;
	QHostAddress m_client;
	QString m_passiveClient;
	quint16 m_port;
	bool m_sizeRestricted;
	bool m_isMaxSize;
	quint64 m_size;
	int m_datatype;
	QString m_pattern;
	Server* m_server;

	QRegExp m_typeRegex;
	QRegExp m_regex;
	bool m_folders;
	bool isPassive;

	// RegEx for file types
	static const QString audio;
	static const QString compressed;
	static const QString docs; // TODO: Add OOo + Mac fileformats
	static const QString execs;
	static const QString pictures;
	static const QString videos;

signals:
	void passiveSearchResult(QString result);
	
};


#endif

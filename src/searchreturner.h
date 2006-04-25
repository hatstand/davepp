#ifndef SEARCHRETURNER_H
#define SEARCHRETURNER_H

#include <QHostAddress>
#include <QUdpSocket>
#include <QThread>
#include <QMutex>
#include "filelist.h"
#include "server.h"

class SearchReturner: public QThread
{
	Q_OBJECT
		 
	public:
	SearchReturner(Server* server, QHostAddress client, quint16 port, bool sizeRestricted, bool isMaxSize, quint64 size, int datatype, QString pattern);
	~SearchReturner();

	private:
	void run();
	void SearchDescend(FileNode* current);
	void SubmitResult(FileNode* node);
	
	FileList* m_list;
	QUdpSocket* m_sock;
	QMutex* list_mutex;
	QHostAddress m_client;
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

	// RegEx for file types
	QString audio;
	QString compressed;
	QString docs; // TODO: Add OOo + Mac fileformats
	QString execs;
	QString pictures;
	QString videos;
	
};


#endif

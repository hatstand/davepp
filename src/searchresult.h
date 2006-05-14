#ifndef SEARCHRESULT_H
#define SEARCHRESULT_H

#include <QString>

class SearchResult
{
	public:
		SearchResult(QString nick, QString filename, quint64 size, int openslots, int totalslots, QString hash, QString hubaddress);
		  
		QString nick;
		QString filename;
		quint64 size;
		QString nicesize;
		int openslots;
		int totalslots;
		QString hubaddress;
		QString hash;

		QString niceFilename() { return filename.section('\\', -1); }
};

#endif

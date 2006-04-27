#ifndef ZCOMPRESS_H
#define ZCOMPRESS_H

#include <QFile>

#include <zlib.h>

class ZCompress
{
		  
public:	
	ZCompress(QFile* file, quint64 chunkSize);
	~ZCompress();
	char* getMoreData();
		  
	bool isOK();

private:
	QFile* file;
	quint64 chunkSize;
	bool isok;

	// ZLib stuff
	int ret, flush;
	unsigned have;
	z_stream strm;
	char* in;
	char* out;

};

#endif

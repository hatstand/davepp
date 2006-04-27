#include "zcompress.h"

#include <zlib.h>

ZCompress::ZCompress(QFile* file, quint64 chunkSize) : file(file), chunkSize(chunkSize), isok(false)
{
	in = (char*)malloc(sizeof(char) * chunkSize);
		  
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);

	if(ret == Z_OK)
		isok = true;
}

ZCompress::~ZCompress()
{
	free(in);
}

char* ZCompress::getMoreData()
{
//	do
	{	
		ret = file->read(in, chunkSize);
		strm.avail_in = ret;
		if(ret < 0)
		{
			deflateEnd(&strm);
			return NULL;
		}

		flush = file->atEnd() ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = (Bytef*)in;

		do
		{
			out = (char*)malloc(sizeof(char) * chunkSize);

			strm.avail_out = chunkSize;
			strm.next_out = (Bytef*)out;

			ret = deflate(&strm, flush);
			Q_ASSERT(ret != Z_STREAM_ERROR);

			have = chunkSize - strm.avail_out;

		} while (strm.avail_out == 0);
		
		Q_ASSERT(strm.avail_in == 0);
	}	
// while (flush != Z_FINISH);

	Q_ASSERT(ret == Z_STREAM_END);

	deflateEnd(&strm);
	
	return out; 
}

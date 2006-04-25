#include "searchresult.h"
#include "utilities.h"

SearchResult::SearchResult(QString nick, QString filename, quint64 size, int openslots, int totalslots, QString hash, QString hubaddress):
		  nick(nick),
		  filename(filename),
		  size(size),
		  openslots(openslots),
		  totalslots(totalslots),
		  hash(hash),
		  hubaddress(hubaddress)
{
	nicesize = Utilities::niceSize(size);
}

#ifndef FILELIST_H
#define FILELIST_H

#include "filenode.h"
#include <QIODevice>

class FileNode;

class FileList
{
public:
	FileList(QTextStream* stream);
	FileList(FileNode* root);
	~FileList();
	void print();
	
	FileNode* root() {return m_root;}
	
	void calculateTotalSize();
	quint64 totalSize();
	void setTotalSize(quint64 size);

private:
	FileNode* m_root;
	quint64 m_totalSize;
};

#endif


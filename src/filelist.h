#ifndef FILELIST_H
#define FILELIST_H

#include "filenode.h"
#include <QIODevice>
#include <QDomDocument>

class FileNode;

class FileList
{
public:
	FileList(QTextStream* stream);
	FileList(FileNode* root);
	FileList(QByteArray data);
	~FileList();
	void print();
	
	FileNode* root() {return m_root;}
	
	void calculateTotalSize();
	quint64 totalSize();
	void setTotalSize(quint64 size);
	QString toString();
	QByteArray toAscii();
	QDomDocument toXml();
	QByteArray toUtf8();

private:
	FileNode* m_root;
	quint64 m_totalSize;
	QString writeNodeToDcList(FileNode* node, QString indent);
	void writeNodeToXmlList(FileNode* node, QDomElement* root, QDomDocument doc);
};

#endif


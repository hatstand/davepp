#ifndef FILENODE_H
#define FILENODE_H

#include <QList>
#include <QString>
#include <QTextStream>
#include <QTreeWidgetItem>
#include <QDir>

#include "utilities.h"

class FileNode
{
public:
	FileNode(FileNode* p, QString name);
	FileNode(FileNode* p, QString name, quint64 size);
	FileNode(FileNode* p, QDir dir, QString name);
	~FileNode();
	void print(int indent);
	
	QString name() {return m_name;}
	bool isDir() {return m_isDir;}
	quint64 size() {return m_size;}
	QString niceSize() {return Utilities::niceSize(m_size);}
	QList<FileNode*> children() {return m_children;}
	FileNode* parent() {return m_parent;}
	
	quint64 totalSize();

private:
	QString m_name;
	bool m_isDir;
	quint64 m_size;
	QList<FileNode*> m_children;
	FileNode* m_parent;
};


#endif



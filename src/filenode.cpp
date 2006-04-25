#include "filenode.h"

#include <QString>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QDir>

#include <assert.h>

FileNode::FileNode(FileNode* p, QString name)
 : m_name(name), m_isDir(true), m_size(0), m_parent(p)
{
	if (p != NULL)
		p->m_children.append(this);
}

FileNode::FileNode(FileNode* p, QString name, quint64 size)
 : m_name(name), m_isDir(false), m_size(size), m_parent(p)
{
	if (p != NULL)
		p->m_children.append(this);
}

FileNode::FileNode(FileNode* p, QDir dir, QString name) :
	m_name(name), m_isDir(true), m_size(0), m_parent(p)
{
	foreach(QString temp, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase))
	{
		new FileNode(this, QDir(dir.path() + "/" + temp), temp);
	}

	foreach(QFileInfo file, dir.entryInfoList(QDir::Files, QDir::Name | QDir::IgnoreCase))
	{
		new FileNode(this, file.fileName(), file.size());
	}

	if(p != NULL)
		p->m_children.append(this);
}

FileNode::~FileNode()
{
	foreach(FileNode* child, m_children)
	{
		assert(child != NULL);
		delete(child);
	}
}

void FileNode::print(int indent)
{
	qDebug() << m_name << indent;
	/*for(std::list<fileNode*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->print();
	}

	qDebug() << ind << m_name << m_size;
	*/
	foreach(FileNode* child, m_children)
	{
		child->print(indent+1);
	}
	/*

	return w;*/
}

quint64 FileNode::totalSize()
{
	if (!isDir())
		return m_size;
	
	quint64 tot = 0;
	foreach (FileNode* child, m_children)
	{
		if (child->isDir())
			tot += child->totalSize();
		else
			tot += child->size();
	}
	
	return tot;
}

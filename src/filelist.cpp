#include "filelist.h"
#include "filenode.h"
#include "xmllistparser.h"

#include <QIODevice>
#include <QFile>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QXmlInputSource>

#include <assert.h>

FileList::FileList(QTextStream* stream)
{
	m_root = new FileNode(NULL, "<root>");
	
	int depth = 0;
	FileNode* parent = m_root;
	while (!stream->atEnd())
	{
		QString line = stream->readLine();
		QStringList l = line.split('|');
	
		int indent = l[0].count('\t');
		while (depth > indent) // Down 1 level
		{
			parent = parent->parent();
			depth--;
		}
		
		QString name = l[0].trimmed();
		if (l.count() == 2) // File without children
		{
			quint64 size = l[1].toLongLong();
			new FileNode(parent, name, size);
		}
		else // Directory with children
		{
			depth++;
			parent = new FileNode(parent, name);
		}
	}
}

FileList::FileList(FileNode* root)
{
	m_root = root;
}

FileList::FileList(QByteArray data)
{
	m_root = new FileNode(NULL, "<root>");

	XmlListParser handler(m_root);
	QXmlInputSource source;
	source.setData(data);
	QXmlSimpleReader reader;
	reader.setContentHandler(&handler);

	reader.parse(source);
}

void FileList::print()
{
	m_root->print(0);
}

FileList::~FileList()
{
	assert(m_root != NULL);
	delete(m_root);
}

void FileList::calculateTotalSize()
{
	m_totalSize = m_root->totalSize();
}

quint64 FileList::totalSize()
{
	return m_totalSize;
}

void FileList::setTotalSize(quint64 size)
{
	m_totalSize = size;
}

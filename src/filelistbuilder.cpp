/***************************************************************************
 *   Copyright (C) 2006 by David Sansome   *
 *   david@linux   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "filelistbuilder.h"
#include "filelist.h"
#include "filenode.h"
#include "configuration.h"

#include <QBuffer>
#include <QDebug>

FileListBuilder* fileListBuilderInstance = NULL;

FileListBuilder::FileListBuilder(Configuration* config)
 : m_config(config), m_list(NULL), doc()
{
	fileListBuilderInstance = this;
}


FileListBuilder::~FileListBuilder()
{
}

FileListBuilder* FileListBuilder::instance()
{
	return fileListBuilderInstance;
}

void FileListBuilder::run()
{
	QMap<QString, QString> sharedDirs(m_config->sharedDirs());
	FileNode* root = new FileNode(NULL, "<root>");

	QDomElement begin = doc.createElement("FileListing");
	begin.setAttribute("Version", "1");
	begin.setAttribute("Generator", "Dave++");
	doc.appendChild(begin);
	
	uint totalSteps = 4 + sharedDirs.count();
	uint step = 0;
	emit progress(step, totalSteps);
	
	QMapIterator<QString, QString> it(sharedDirs);
	while (it.hasNext())
	{
		it.next();
		new FileNode(root, QDir(it.value()), it.key());

//		QDomElement el = doc.createElement("Directory");
//		el.setAttribute("Name", it.value());
//		begin.appendChild(el);

		emit progress(step++, totalSteps);
	}
	
	m_mutex.lock();
	delete m_list;
	m_list = new FileList(root);
	m_list->calculateTotalSize();
	emit progress(step++, totalSteps);
	m_mutex.unlock();
	
	QString dcList;
	
	foreach (FileNode* child, root->children())
	{
		dcList += writeNodeToDcList(child, "");
		writeNodeToXmlList(child, &begin);
	}
	
	QByteArray huffmanList = Utilities::encodeList(dcList.toAscii());
	emit progress(step++, totalSteps);
	QByteArray BZList = Utilities::encodeBZList(dcList.toAscii());
	emit progress(step++, totalSteps);
//	qDebug() << doc.toString();
	QByteArray XmlBZList = Utilities::encodeBZList(doc.toByteArray());
	emit progress(step++, totalSteps);
	m_mutex.lock();
	m_huffmanList = huffmanList;
	m_BZList = BZList;
	m_XmlBZList = XmlBZList;
	m_mutex.unlock();
}

FileList* FileListBuilder::list()
{
	m_mutex.lock();
	FileList* ret = m_list;
	m_mutex.unlock();
	return ret;
}

QByteArray FileListBuilder::huffmanList()
{
	m_mutex.lock();
	QByteArray ret = m_huffmanList;
	m_mutex.unlock();
	return ret;
} 

QByteArray FileListBuilder::bzList()
{
	m_mutex.lock();
	QByteArray ret = m_BZList;
	m_mutex.unlock();
	return ret;
}

QByteArray FileListBuilder::xmlBZList()
{
	m_mutex.lock();
	QByteArray ret = m_XmlBZList;
	m_mutex.unlock();
	return ret;
}

QString FileListBuilder::writeNodeToDcList(FileNode* node, QString indent)
{
	if (!node->isDir())
	{
		return indent + node->name() + "|" + QString::number(node->size()) + "\r\n";
	}
	else
	{
		QString ret = indent + node->name() + "\r\n";
		foreach (FileNode* child, node->children())
		{
			ret += writeNodeToDcList(child, indent + "\t");
		}
		return ret;
	}
}

void FileListBuilder::writeNodeToXmlList(FileNode* node, QDomElement* root)
{
	if(!node->isDir())
	{
		QDomElement el = doc.createElement("File");
		el.setAttribute("Size", node->size());
		el.setAttribute("Name", node->name());
		root->appendChild(el);
	}
	else
	{
		QDomElement el = doc.createElement("Directory");
		el.setAttribute("Name", node->name());
		root->appendChild(el);

		foreach (FileNode* child, node->children())
		{
			writeNodeToXmlList(child, &el);
		}
	}
}

quint64 FileListBuilder::totalSize()
{
	if (m_list == NULL)
		return 0;
	return m_list->totalSize();
}




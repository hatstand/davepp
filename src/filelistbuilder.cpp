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
#include <QMutexLocker>

FileListBuilder* fileListBuilderInstance = NULL;

FileListBuilder::FileListBuilder(Configuration* config)
 : m_config(config), m_list(NULL), doc()
{
	fileListBuilderInstance = this;
	m_ready = false;
	m_mutex = new QMutex();
	ignoreSaved = false;
}


FileListBuilder::~FileListBuilder()
{
	delete(m_mutex);
}

FileListBuilder* FileListBuilder::instance()
{
	return fileListBuilderInstance;
}

void FileListBuilder::run()
{
	m_ready = false;
	// TODO: Add hash of byteArray to settings
	uint totalSteps = 3;
	uint step = 0;

	if(!ignoreSaved)
	{
		QByteArray savedBZXml = m_config->getSavedXmlList();
		if(!savedBZXml.isEmpty()) // Yay! We found a saved file list
		{
			QMutexLocker locker(m_mutex);
			emit progress(++step, totalSteps);
			qDebug() << "Using saved XML";
			m_list = new FileList(Utilities::decodeBZList(savedBZXml));
			emit progress(++step, totalSteps);
			m_list->calculateTotalSize();
			emit progress(++step, totalSteps);

			m_XmlBZList = savedBZXml;
		
			m_ready = true;
			return;
		}
	}

	qDebug() << "Creating new filelist";

	QMap<QString, QString> sharedDirs(m_config->sharedDirs());
	FileNode* root = new FileNode(NULL, "<root>");

	totalSteps = 4 + sharedDirs.count();
	emit progress(step++, totalSteps);

	
	QMapIterator<QString, QString> it(sharedDirs);
	while (it.hasNext())
	{
		it.next();
		new FileNode(root, QDir(it.value()), it.key());

		emit progress(step++, totalSteps);
	}
	
	{
		QMutexLocker locker(m_mutex);
		delete m_list; // Remove old filelist
		emit progress(step++, totalSteps);

		// Generate File List in memory
		m_list = new FileList(root);
		emit progress(step++, totalSteps);
		m_list->calculateTotalSize();
	}
	emit progress(step++, totalSteps);

	// Don't bother creating useful lists until they're requested
	// BZList is rarely requested for example, so why bother?
	m_huffmanList.clear();
	m_BZList.clear();
	m_XmlBZList.clear();
	
	Configuration::instance()->setFileListDirty(false);
	
	m_ready = true;

	ignoreSaved = false;
}

FileList* FileListBuilder::list()
{
	QMutexLocker locker(m_mutex);
	FileList* ret = m_list;
	return ret;
}

QByteArray FileListBuilder::huffmanList()
{
	QMutexLocker locker(m_mutex);

	if(m_huffmanList.isEmpty())
		m_huffmanList = Utilities::encodeList(m_list->toAscii());

	QByteArray ret = m_huffmanList;
	return ret;
} 

QByteArray FileListBuilder::bzList()
{
	QMutexLocker locker(m_mutex);

	if(m_BZList.isEmpty())
		m_BZList = Utilities::encodeBZList(m_list->toAscii());

	QByteArray ret = m_BZList;
	return ret;
}

QByteArray FileListBuilder::xmlBZList()
{
	QMutexLocker locker(m_mutex);

	if(m_XmlBZList.isEmpty())
		m_XmlBZList = Utilities::encodeBZList(m_list->toXml().toByteArray());

	QByteArray ret = m_XmlBZList;
	return ret;
}

quint64 FileListBuilder::totalSize()
{
	if (m_list == NULL)
		return 0;
	return m_list->totalSize();
}

void FileListBuilder::regenList()
{
	qDebug() << "Regen list";
	ignoreSaved = true;
	start();
}

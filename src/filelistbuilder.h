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
#ifndef FILELISTBUILDER_H
#define FILELISTBUILDER_H

#include <QThread>
#include <QBuffer>
#include <QMutex>
#include <QDomElement>

class Configuration;
class FileList;
class FileNode;

class FileListBuilder : public QThread
{
	Q_OBJECT
public:
	FileListBuilder(Configuration* config);
	~FileListBuilder();
	
	FileList* list();
	QByteArray huffmanList();
	QByteArray bzList();
	QByteArray xmlBZList();
	quint64 totalSize();
	
	void lock() { m_mutex.lock(); }
	void unlock() { m_mutex.unlock(); }
	
	static FileListBuilder* instance();

	bool isReady() {return m_ready;}

signals:
	void progress(uint value, uint total);
	void finished();
	
private:
	void run();
	QString writeNodeToDcList(FileNode* node, QString indent);
	void writeNodeToXmlList(FileNode* node, QDomElement* doc);
	
	Configuration* m_config;
	FileList* m_list;
	QByteArray m_huffmanList;
	QByteArray m_BZList;
	QByteArray m_XmlBZList;
	QMutex m_mutex;
	QDomDocument doc;

	bool m_ready;
};

#endif

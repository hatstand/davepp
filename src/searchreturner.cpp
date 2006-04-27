#include "searchreturner.h"
#include "filelistbuilder.h"
#include "filenode.h"
#include "user.h"

#include <QDebug>
#include <QUdpSocket>

SearchReturner::SearchReturner(Server* server, QHostAddress client, quint16 port, bool sizeRestricted, bool isMaxSize, quint64 size, int datatype, QString pattern)
		  : m_server(server), m_client(client), m_port(port), m_sizeRestricted(sizeRestricted), m_isMaxSize(isMaxSize), m_size(size), m_datatype(datatype), m_pattern(pattern), m_typeRegex(), m_regex()
{
//	qDebug() << "Search started";

		  
	audio = "\\.(mp3)|(mp2)|(wav)|(au)|(rm)|(mid)|(sm)$";
	compressed = "\\.(zip)|(arj)|(rar)|(lzh)|(gz)|(z)|(arc)|(pak)|(tar)|(bz2)$";
	docs = "\\.(doc)|(txt)|(wri)|(pdf)|(ps)|(tex)|(dvi)$";
	execs = "\\.(pm)|(exe)|(bat)|(com)$";
	pictures = "\\.(gif)|(jpg)|(jpeg)|(bmp)|(pcx)|(png)|(wmf)|(psd)|(svg)$";
	videos = "\\.(mpg)|(mpeg)|(avi)|(asf)|(mov)$";
		  
	FileListBuilder* bob = FileListBuilder::instance();
	m_list = bob->m_list;

	if(m_list == NULL)
	{
		qWarning() << "No filelist present";
		return;
	}
	
	list_mutex = &(bob->m_mutex);

	m_folders = false;

	//Construct regular expression from pattern string
	QStringList elements = m_pattern.split("$");
	if(!elements.isEmpty())
	{
		QString temp = "";
		foreach(QString element, elements)
		{
			temp += "(";
			temp += element;
			temp += ")|";
		}
		temp.chop(1);

		m_regex.setPattern(temp);
	}
	
	switch(m_datatype)
	{
		case 2:
			m_typeRegex.setPattern(audio);
			break;
		case 3:
			m_typeRegex.setPattern(compressed);
			break;
		case 4:
			m_typeRegex.setPattern(docs);
			break;
		case 5:
			m_typeRegex.setPattern(execs);
			break;
		case 6:
			m_typeRegex.setPattern(pictures);
			break;
		case 7:
			m_typeRegex.setPattern(videos);
			break;
		case 8:
			m_folders = true;
			break;
		default:
			break;
	}

	start();
}


SearchReturner::~SearchReturner()
{
}

void SearchReturner::run()
{
//	qDebug() << "Thread started";
	m_sock = new QUdpSocket();
  
	list_mutex->lock();

	FileNode* root = m_list->root();

	SearchDescend(root);

	list_mutex->unlock();
}

void SearchReturner::SearchDescend(FileNode* current)
{
	foreach(FileNode* child, current->children())
	{
		if(child->isDir())
		{
			SearchDescend(child);
			if(m_folders)
				SubmitResult(child);
		}
		else if(m_folders)
			continue;
			

		// If we care about size...
		if(m_sizeRestricted)
		{
			if(child->size() < m_size)
			{
				if(m_isMaxSize)
					continue; // Smaller than minimum
			}
			else if(!m_isMaxSize)
				continue; // Larger than maximum
		}
		
		// We have a datatype
		if(m_typeRegex.isEmpty())
		{
			if(!child->name().contains(m_typeRegex))
				continue; // Doesn't match the current datatype
		}

		// Final filename pattern match
		if(child->name().contains(m_regex))
			SubmitResult(child);
	}
}

void SearchReturner::SubmitResult(FileNode* node)
{
//	qDebug() << "Found Result!";
//	QUdpSocket* m_sock = new QUdpSocket();

	QString path = node->name();
	FileNode* parent = node->parent();
	while ((parent) && (parent->name() != "<root>"))
	{
		path = parent->name() + "\\" + path;
		parent = parent->parent();
	}

	QString thingy = "$SR " + m_server->me()->nick + " " + path + "\05" + QString::number(node->size()) + " " + "1/1" + "\05" + m_server->hubName() + " (" + m_server->ip() + ":" + QString::number(m_server->port()) + ")|";
	QByteArray bleh = thingy.toLatin1();
//	qDebug() << bleh;
//	qDebug() << m_sock->writeDatagram(bleh, bleh.size(), m_client, m_port);
//	qDebug() << "alive";
	m_sock->flush();
}


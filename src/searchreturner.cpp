#include "searchreturner.h"
#include "filelistbuilder.h"
#include "filenode.h"
#include "user.h"

#include <QDebug>
#include <QUdpSocket>

const QString SearchReturner::audio = "\\.(mp3)|(mp2)|(wav)|(au)|(rm)|(mid)|(sm)$";
const QString SearchReturner::compressed = "\\.(zip)|(arj)|(rar)|(lzh)|(gz)|(z)|(arc)|(pak)|(tar)|(bz2)$";
const QString SearchReturner::docs = "\\.(doc)|(txt)|(wri)|(pdf)|(ps)|(tex)|(dvi)$";
const QString SearchReturner::execs = "\\.(pm)|(exe)|(bat)|(com)$";
const QString SearchReturner::pictures = "\\.(gif)|(jpg)|(jpeg)|(bmp)|(pcx)|(png)|(wmf)|(psd)|(svg)$";
const QString SearchReturner::videos = "\\.(mpg)|(mpeg)|(avi)|(asf)|(mov)$";

SearchReturner::SearchReturner(Server* server, QHostAddress client, quint16 port, bool sizeRestricted, bool isMaxSize, quint64 size, int datatype, QString pattern)
		  : m_server(server), m_client(client), m_port(port), m_sizeRestricted(sizeRestricted), m_isMaxSize(isMaxSize), m_size(size), m_datatype(datatype), m_pattern(pattern), m_typeRegex(), m_regex(), isPassive(false)
{
	begin();
}

SearchReturner::SearchReturner(Server* server, QString client, bool sizeRestricted, bool isMaxSize, quint64 size, int datatype, QString pattern)
		  : m_server(server), m_passiveClient(client), m_sizeRestricted(sizeRestricted), m_isMaxSize(isMaxSize), m_size(size), m_datatype(datatype), m_pattern(pattern), m_typeRegex(), m_regex(), isPassive(true)
{
//	qDebug() << "Passive Search";
	begin();
}

void SearchReturner::begin()
{
	m_builder = FileListBuilder::instance();
	m_list = m_builder->list();

	if(m_list == NULL)
	{
		qWarning() << "No filelist present";
		return;
	}
	
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
		case 9: // TTH: unimplemented
//			deleteLater(); // Really bad idea it seems
			return; 
		default:
//			deleteLater();
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
	if(!isPassive)
		m_sock = new QUdpSocket();
  
	m_builder->lock();

	FileNode* root = m_list->root();

	SearchDescend(root);

	m_builder->unlock();
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
	QString path = node->name();
	FileNode* parent = node->parent();
	while ((parent) && (parent->name() != "<root>"))
	{
		path = parent->name() + "\\" + path;
		parent = parent->parent();
	}

	QString thingy = "$SR " + m_server->me()->nick + " " + path + "\05" + QString::number(node->size()) + " " + "1/1" + "\05" + m_server->hubName() + " (" + m_server->ip() + ":" + QString::number(m_server->port()) + ")";
//	qDebug() << bleh;
	if(isPassive)
	{
		thingy += "\05" + m_passiveClient + "|";
		emit passiveSearchResult(thingy);
//		qDebug() << "Passive result emitted";
	}
	else
	{
		thingy += "|";
		QByteArray bleh = thingy.toLatin1();
		m_sock->writeDatagram(bleh, bleh.size(), m_client, m_port);
		m_sock->flush();
	}
}


#include "xmllistparser.h"
#include "filenode.h"

#include <QDebug>

XmlListParser::XmlListParser(FileNode* root) :
		  m_root(root)
{

}

bool XmlListParser::startDocument()
{
	parent = m_root;

	return true;
}

bool XmlListParser::startElement(const QString&, const QString& localName, const QString&, const QXmlAttributes& atts)
{
	//qDebug() << localName;
		  
	if(localName == "Directory")
	{
		QString name = atts.value("", "Name");
		parent = new FileNode(parent, name);
	}
	else if(localName == "File")
	{
		QString name = atts.value("", "Name");
		quint64 size = atts.value("", "Size").toULongLong();
		new FileNode(parent, name, size);
	}

	return true;
}

bool XmlListParser::endElement(const QString&, const QString& localName, const QString&)
{
	//qDebug() << "/" << localName;

	if(localName == "Directory")
	{
		parent = parent->parent();
	}

	return true;
}

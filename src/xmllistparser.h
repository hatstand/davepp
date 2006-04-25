#ifndef XMLLISTPARSER_H
#define XMLLISTPARSER_H

#include <QXmlDefaultHandler>

#include "filenode.h"

class XmlListParser : public QXmlDefaultHandler
{
	public:
		XmlListParser(FileNode* root);
			  
		bool startDocument();
		bool startElement(const QString&, const QString& localName, const QString&, const QXmlAttributes& atts);
		bool endElement(const QString&, const QString& localName, const QString&);

	private:
		FileNode* m_root;
		FileNode* parent;
};

#endif

#ifndef TIGERTREE_H
#define TIGERTREE_H

#include <QFile>
#include <QVector>

#include "tigerhash.h"

class TigerTree
{
	public:
	TigerTree(QFile* file);
	~TigerTree();
	
	QByteArray toByteArray();

	private:
	struct Node
	{
		Node* left;
		Node* right;
		TigerHash* hash;

		~Node()
		{
			Q_ASSERT(hash); // Nodes shouldn't be created without a hash
			if(!(left && !right)) // Right most node could be sharing a ptr
				delete(hash);
		}
	};

	QVector<Node**> hashTree;
};

#endif

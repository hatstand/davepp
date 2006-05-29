#include "tigertree.h"

#include <QDebug>
#include <QVector>

TigerTree::TigerTree(QFile* file)
{
	qint64 size = file->size();
	if(size <= 0)
	{
		qWarning() << "Attempted to hash file with size <= 0";
		return;
	}

	if(!file->isReadable())
		file->open(QIODevice::ReadOnly);

	int bottomLevelSize = size / 1024;
	if(size % 1024)
		bottomLevelSize++;

	Node** bottomLevel = new Node*[bottomLevelSize];

	for(int i = 0; i < bottomLevelSize; ++i)
	{
		bottomLevel[i] = new Node();
		bottomLevel[i]->hash = new TigerHash(file->read(1024));
		bottomLevel[i]->left = NULL;
		bottomLevel[i]->right = NULL;
	}
	hashTree << bottomLevel;
	file->close();

	if(bottomLevelSize < 2)
		return;

	int prevLevelSize = bottomLevelSize;
	int levelSize;
	Node** prevLevel = bottomLevel;
	Node** level;

	do
	{
		levelSize = prevLevelSize / 2 + prevLevelSize % 2;

		level = new Node*[levelSize];
		for(int i = 0; i < prevLevelSize / 2; ++i)
		{
			level[i] = new Node();
			//qDebug() << "Adding:" << 2*i << "and" << 2*i+1 << prevLevelSize;
			level[i]->hash = new TigerHash(prevLevel[2*i]->hash->toByteArray().append(prevLevel[2*i+1]->hash->toByteArray()));
			level[i]->left = prevLevel[2*i];
			level[i]->right = prevLevel[2*i+1];
		}
		if(prevLevelSize % 2)
		{
			level[levelSize-1] = new Node();
			level[levelSize-1]->hash = prevLevel[prevLevelSize-1]->hash;
			level[levelSize-1]->left = NULL;
			level[levelSize-1]->right = prevLevel[prevLevelSize-1];
		}

		prevLevelSize = levelSize;
		hashTree << level;
		prevLevel = level;

	} while(levelSize > 1);
}

TigerTree::~TigerTree()
{
	foreach(Node** i, hashTree)
		delete(i);
}

QByteArray TigerTree::toByteArray()
{
	return (*(hashTree.last()))->hash->toByteArray();
}

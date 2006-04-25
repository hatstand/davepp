/***************************************************************************
 *   Copyright (C) 2006 by John Maguire	*
 *   john.maguire@gmail.com	*
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

#ifndef USER_H
#define USER_H

class Server;
class FileList;

#include <QString>
#include <QObject>

/**
@author John Maguire
 */
class User
{
public:
	User(Server* server, QString nick);
	~User();
	
	void setShareSize(qint64 s);
	void setInterest(QString i);
	void setFileList(FileList* f);
	
	QString nick;
	bool op;
	bool me;
	
	QString interest;
	QString speed;
	QString email;
	qint64 shareSize;
	QString niceShareSize;
	
	int freeSlots;
	int totalSlots;
	
	void* listItem;
	Server* server;

	FileList* fileList;
};

#endif

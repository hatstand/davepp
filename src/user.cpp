/***************************************************************************
 *   Copyright (C) 2006 by John Maguire   *
 *   john.maguire@gmail.com   *
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
#include "user.h"
#include "server.h"
#include <QString>
#include <QDebug>
#include "utilities.h"
#include "filelist.h"

User::User(Server* server, QString nick)
 : server(server), nick(nick), op(false), me(false), shareSize(0), totalSlots(0), freeSlots(0), fileList(NULL)
{}

void User::setShareSize(qint64 s)
{
	shareSize = s;
	niceShareSize = Utilities::niceSize(s);
}

void User::setInterest(QString i)
{
	interest = i;
	QRegExp regExp("\\{(\\d+)/(\\d+)\\}");
	if (regExp.indexIn(i) == -1)
	{
		totalSlots = -1;
		freeSlots = -1;
	}
	else
	{
		freeSlots = regExp.cap(1).toInt();
		totalSlots = regExp.cap(2).toInt();
	}
}

void User::setFileList(FileList* f)
{
	if (fileList != NULL)
		delete fileList;
	fileList = f;	
}

User::~User()
{
	delete(fileList);
}


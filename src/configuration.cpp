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
#include "configuration.h"

#include <QDebug>
#include <QDir>
#include "filelistbuilder.h"


// Icky Windows stuff
#ifdef Q_OS_WIN32
	#include <windows.h>
	#include <stdio.h>
	typedef BOOL (WINAPI *PGETDISKFREESPACEEX)(LPCSTR,
						    PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);
// Nice Linux stuff
#elif defined Q_OS_LINUX
	#include <sys/statvfs.h>
#endif

Configuration* configurationInstance = NULL;

Configuration::Configuration(QObject* parent)
 : QObject(parent), m_fileListDirty(false)
{
	configurationInstance = this;
	
	m_settings = new QSettings("DavePlusPlus", "DavePlusPlus", this);
	
	int shareCount = m_settings->beginReadArray("SharedDirs");
	qDebug() << "Share count" << shareCount;
	for (int i=0 ; i<shareCount ; ++i)
	{
		m_settings->setArrayIndex(i);
		
		shareAdd(m_settings->value("Name").toString(), m_settings->value("Path").toString());
	}
	m_settings->endArray();
	
	m_settings->beginGroup("General");
	m_uploadSpeed = m_settings->value("UploadSpeed", 0).toInt();
	m_downloadSpeed = m_settings->value("DownloadSpeed", 0).toInt();
	m_numSlots = m_settings->value("Slots", 6).toInt();
	m_nick = m_settings->value("Nick", "Nick").toString();
	m_email = m_settings->value("Email", "enoon@erewhon.com").toString();
	m_description = m_settings->value("Description", "Dave++").toString();
	m_connSpeed = m_settings->value("ConnSpeed", 2).toInt();
	m_downloadDir = m_settings->value("DownloadDir", QDir::homePath()).toString();
	m_settings->endGroup();

	
	m_slotsInUse = 0;
	m_connectedHubs = 0;

	qint64 free = getFreeSpace();
	qDebug() << m_settings->fileName();
}


Configuration::~Configuration()
{
	save();
}

Configuration* Configuration::instance()
{
	return configurationInstance;
}


void Configuration::shareAdd(QString name, QString dir)
{
	lock();
	m_sharedDirs[name] = dir;
	m_fileListDirty = true;
	unlock();
}

void Configuration::sharesClear()
{
	lock();
	m_sharedDirs.clear();
	m_fileListDirty = true;
	unlock();
}

QMap<QString, QString> Configuration::sharedDirs()
{
	lock();
	QMap<QString, QString> ret = m_sharedDirs;
	unlock();
	
	return ret;
}

QString Configuration::sharedFilename(QString name) const
{
	QString share = name.section('\\', 0, 0);
	if (!m_sharedDirs.contains(share))
		return QString::null;
	QString path = m_sharedDirs.value(share);
	QString ret = path + "/" + name.section('\\', 1, -1).replace('\\', '/');
	
	qDebug() << "From" << name << "to" << ret;
	return ret;
}

void Configuration::save()
{
	m_settings->beginWriteArray("SharedDirs");
	int i = 0;
	QMapIterator<QString, QString> it(m_sharedDirs);
	while (it.hasNext())
	{
		it.next();
		m_settings->setArrayIndex(i++);
		m_settings->setValue("Name", it.key());
		m_settings->setValue("Path", it.value());
	}
	m_settings->endArray();
	
	m_settings->beginGroup("General");
	m_settings->setValue("UploadSpeed", m_uploadSpeed);
	m_settings->setValue("DownloadSpeed", m_downloadSpeed);
	m_settings->setValue("Slots", m_numSlots);
	m_settings->setValue("Nick", m_nick);
	m_settings->setValue("Email", m_email);
	m_settings->setValue("Description", m_description);
	m_settings->setValue("ConnSpeed", m_connSpeed);
	m_settings->setValue("DownloadDir", m_downloadDir);
	m_settings->endGroup();

	m_settings->beginGroup("FilelistInfo");
	m_settings->setValue("XmlFileList", FileListBuilder::instance()->xmlBZList());
	m_settings->endGroup();
	
	m_settings->sync();
}

void Configuration::saveFile(QString localpath, QString remotenick, QString remotepath, QString hub, quint64 bytesDone)
{
	m_settings->beginGroup("Files");
	m_settings->beginGroup(localpath);
	m_settings->setValue("RemoteNick", remotenick);
	m_settings->setValue("RemotePath", remotepath);
	m_settings->setValue("Hub", hub);
	m_settings->setValue("BytesDone", bytesDone);
	m_settings->endGroup();
	m_settings->endGroup();
}

QString Configuration::connSpeedString() const
{
	switch (connSpeed())
	{
		case 0: return "Modem";
		case 1: return "DSL";
		case 2: return "Cable";
		case 3: return "Satellite";
		case 4: return "LAN(T1)";
		case 5: return "LAN(T3)";
	}
	return QString::null;
}

void Configuration::setNick(QString nick)
{
	if(nick != m_nick)
	{
		m_nick = nick;
		emit nickChanged(nick);
	}
}

qint64 Configuration::getFreeSpace() const
{
#ifdef Q_OS_WIN32buggered
	LPCSTR pszDrive;
	PGETDISKFREESPACEEX pGetDiskFreeSpaceEx;
	qint64 i64FreeBytesToCaller, i64TotalBytes, i64FreeBytes;

	DWORD dwSectPerClust,
			dwBytesPerSect,
			dwFreeClusters,
			dwTotalClusters;

	BOOL fResult;

	fResult = pGetDiskFreeSpaceEx(pszDrive,
							(PULARGE_INTEGER) & i64FreeBytesToCaller,
							(PULARGE_INTEGER) & i64TotalBytes,
							(PULARGE_INTEGER) & i64FreeBytes);
	
	if(fResult)
	{
		return i64FreeBytesToCaller;
	}
	else
		return -1;
#elif defined Q_OS_LINUX // statvfs should be available on most unices
	struct statvfs results;
	statvfs(m_downloadDir, &results);
	return (qint64)(results.f_bavail * results.f_bsize);
#else
	return -1; // No idea. Must be weird not to be POSIX or Win32
#endif
}

bool Configuration::slotsLeft() const
{
	if(m_slotsInUse < m_numSlots)
		return true;
	else
		return false;
}

bool Configuration::getSlot()
{
	lock();
	if(m_slotsInUse < m_numSlots)
	{
		++m_slotsInUse;
		unlock();
		emit infoChanged();
		return true;
	}
	else
	{
		unlock();
		return false;
	}
}

void Configuration::revokeSlot()
{
	lock();
	--m_slotsInUse;
	unlock();
	emit infoChanged();
}

void Configuration::hubConnected()
{
	++m_connectedHubs;
	emit infoChanged();
}

void Configuration::hubDisconnected()
{
	--m_connectedHubs;
	emit infoChanged();
}

void Configuration::setNumSlots(int s)
{
	if(s != m_numSlots)
	{
		m_numSlots = s;
		emit infoChanged();
	}
}

QString Configuration::niceSpeed(int x) const
{
	if(x < 0)
		return QString::number(0);
	else if(x < 1024)
		return QString::number(x) + "K";
	else if(x < 1024*1024)
		return QString::number(x/1024) + "M";
	else if(x < 1024*1024*1024) // GB!
		return QString::number(x/(1024*1024)) + "G";

	return "Really Fast";
}

QString Configuration::niceDownloadSpeed() const
{
	return niceSpeed(m_downloadSpeed);
}

QString Configuration::niceUploadSpeed() const
{
	return niceSpeed(m_uploadSpeed);
}

QByteArray Configuration::getSavedXmlList()
{
	m_settings->beginGroup("FilelistInfo");
	QByteArray list = m_settings->value("XmlFileList").toByteArray();
	m_settings->endGroup();

	return list;
}

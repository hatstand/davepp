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
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>
#include <QMutex>
#include <QMap>
#include <QSettings>

#define VERSION 0.01

class Configuration : public QObject
{
	Q_OBJECT
		  
public:
	Configuration(QObject* parent);
	~Configuration();
	
	static Configuration* instance();
	
	QString downloadDir() const {return m_downloadDir;}
	void setDownloadDir(QString downloadDir) {m_downloadDir = downloadDir;}
	
	void shareAdd(QString name, QString dir);
	void sharesClear();
	QMap<QString, QString> sharedDirs();
	QString sharedFilename(QString name) const;
	
	bool isFileListDirty() const { return m_fileListDirty; }
	void setFileListDirty(bool dirty) { m_fileListDirty = dirty; }
	
	int uploadSpeed() const {return m_uploadSpeed;}
	QString niceUploadSpeed() const ;
	void setUploadSpeed(int speed) {m_uploadSpeed = speed;}

	int downloadSpeed() const {return m_downloadSpeed;}
	QString niceDownloadSpeed() const;
	void setDownloadSpeed(int speed) {m_downloadSpeed = speed;}
	
	int numSlots() const {return m_numSlots;}
	void setNumSlots(int s);
	
	QString nick() const {return m_nick;}
	void setNick(QString nick);
	
	QString email() const {return m_email;}
	void setEmail(QString email) {m_email = email;}
	
	QString description() const {return m_description;}
	void setDescription(QString description) {m_description = description;}
	
	int connSpeed() const {return m_connSpeed;}
	QString connSpeedString() const;
	void setConnSpeed(int connSpeed) {m_connSpeed = connSpeed;}
	
	QString tag() const {return m_tag;}
	
	// Private?
	void lock() {m_mutex.lock();}
	void unlock() {m_mutex.unlock();}
	
	QSettings* settings() { return m_settings; }
	
	void save();
	void saveFile(QString localpath, QString remotenick, QString remotepath, QString hub, quint64 bytesDone);
	
	// Stuff that isn't saved
	int slotsInUse() const {return m_slotsInUse;}
	void setSlotsInUse(int s) {m_slotsInUse = s;}
	bool slotsLeft() const;
	bool getSlot();
	void revokeSlot();
	int slotsFree() const { return m_numSlots - m_slotsInUse; }

	int connectedHubs() const { return m_connectedHubs; };
	void hubConnected();
	qint64 getFreeSpace() const;

	QByteArray getSavedXmlList();

	int numBars;
	
private:
	QString niceSpeed(int x) const;

	QString m_downloadDir;
	QMap<QString, QString> m_sharedDirs;
	int m_uploadSpeed;
	int m_downloadSpeed;
	int m_numSlots;
	
	QString m_nick;
	QString m_email;
	QString m_description;
	int m_connSpeed;

	QString m_tag;
	
	bool m_fileListDirty;
	QMutex m_mutex;
	QSettings* m_settings;
	
	// Stuff that isn't saved
	int m_slotsInUse;
	int m_connectedHubs;

private slots:
	void hubDisconnected();

signals:
	void nickChanged(QString newnick);
	void infoChanged();

};

#endif

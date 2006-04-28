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
#ifndef TRANSFERLISTITEM_H
#define TRANSFERLISTITEM_H

#include <QProgressBar>
#include <Q3ListView>

class Client;
class ClientConnector;
class User;


class DaveProgressBar : public QProgressBar
{
public:
	DaveProgressBar(QWidget* parent) : QProgressBar(parent) {}
	QString text() const;
	void setText(QString text);
	
private:
	QString m_text;
};



class TransferListItem : public QObject, public Q3ListViewItem
{
	Q_OBJECT
public:
	TransferListItem(Q3ListView* parent);
	TransferListItem(TransferListItem* parent);
	~TransferListItem();
	
	void paintCell(QPainter* painter, const QColorGroup& cg, int column, int width, int align);
	//int height() const;
	int rtti() const {return 1004;}
	
	void setFilelistDownload(User* user);
	void setFileDownload(User* user, QString path);
	void setFileUpload(ClientConnector* connector);
	bool isRunning() {return m_transfer != NULL;}
	bool isTopLevel() {return depth() == 0;}
	bool isTopOfQueue() {return (!isTopLevel() && (depth() != itemAbove()->depth()));}
	bool isBottomOfQueue() {return (!isTopLevel() && ((itemBelow() == NULL) || (depth() != itemBelow()->depth())));}
	bool isUserOffline() {return m_user == NULL;}
	void start();
	void stop();
	void remove();
	void moveToTop();
	void moveToBottom();
	
	enum TransferType
	{
		None = 0,
		DownloadFileList,
		DownloadFile,
		UploadFile
	};
	
	User* user() {return m_user;}
	TransferType type() {return m_type;}
	QString path() {return m_path;}
	
private slots:
	void clientStateChanged(int state);
	void clientResult(int result);
	void clientProgress(uint size, uint totalSize);
	void clientDestroyed();
	void doNextQueued();
	void userQuit(User* user);
	void clientInfoChanged();
	void clientSpeedChanged(quint64 speed);
	
private:
	void swapWith(TransferListItem* item);
	
	TransferType m_type;
	User* m_user;
	QString m_nick;
	Client* m_transfer;
	QString m_path;
	DaveProgressBar* m_progress;
	quint64 m_speed;
	uint m_totalSize;
	uint m_size;
};

#endif

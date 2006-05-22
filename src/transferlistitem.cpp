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
#include "transferlistitem.h"
#include "clientlistener.h"
#include "clientconnector.h"
#include "user.h"
#include "server.h"

#include <QTimer>
#include <QDebug>
#include <QPainter>
#include <QApplication>
#include <QMessageBox>

QString DaveProgressBar::text() const
{
	if (!m_text.isEmpty())
		return m_text;
	return QProgressBar::text();
}

void DaveProgressBar::setText(QString text)
{
	m_text = text;
	setTextVisible(true);
	update();
}


TransferListItem::TransferListItem(Q3ListView* parent)
 : Q3ListViewItem(parent), m_transfer(NULL), QObject(parent), m_speed(0), m_totalSize(0), m_update_time(), m_result(-1)
{
	m_progress = new DaveProgressBar(listView()->viewport());
	
	qDebug() << "Height before" << height();
	setHeight(QApplication::fontMetrics().height()*3 + 1);
	qDebug() << "Height after" << height();

	m_timer = new QTimer(this);
	m_timer->setSingleShot(true);
	connect(m_timer, SIGNAL(timeout()), SLOT(doNextQueued()));

	m_update_time.start();
}

TransferListItem::TransferListItem(TransferListItem* parent)
 : Q3ListViewItem(parent), m_transfer(NULL), QObject(parent), m_speed(0), m_progress(NULL), m_totalSize(0), m_update_time(), m_result(-1)
{
	m_timer = new QTimer(this);
	m_timer->setSingleShot(true);
	connect(m_timer, SIGNAL(timeout()), SLOT(doNextQueued()));

	m_update_time.start();
}

TransferListItem::~TransferListItem()
{
	delete m_progress;
	
	if (m_type == UploadFile)
		delete m_transfer;
}

void TransferListItem::setFilelistDownload(User* user)
{
	m_type = DownloadFileList;
	m_user = user;
	m_nick = user->nick;
	
	connect(user->server, SIGNAL(userQuit(User*)), SLOT(userQuit(User*)));
	
	if (m_progress)
	{
		m_progress->move(3, itemPos() + QApplication::fontMetrics().height()*2);
		m_progress->show();
	} 
}

void TransferListItem::setFileDownload(User* user, QString path)
{
	m_type = DownloadFile;
	m_user = user;
	m_nick = user->nick;
	m_path = path;
	
	connect(user->server, SIGNAL(userQuit(User*)), SLOT(userQuit(User*)));
	
	if (m_progress)
	{
		m_progress->move(3, itemPos() + QApplication::fontMetrics().height()*2);
		m_progress->show();
	} 
}

void TransferListItem::setFileUpload(ClientConnector* connector)
{
	m_type = UploadFile;
	m_transfer = connector;
	connect(connector, SIGNAL(stateChanged(int)), SLOT(clientStateChanged(int)));
	connect(connector, SIGNAL(progress(uint, uint)), SLOT(clientProgress(uint, uint)));
	connect(connector, SIGNAL(destroyed()), SLOT(clientDestroyed()));
	connect(connector, SIGNAL(infoChanged()), SLOT(clientInfoChanged()));
	connect(connector, SIGNAL(speedChanged(quint64)), SLOT(clientSpeedChanged(quint64)));
	connect(connector, SIGNAL(result(int)), SLOT(clientResult(int)));

	clientStateChanged(connector->state());
	
	if (m_progress)
	{
		m_progress->move(3, itemPos() + QApplication::fontMetrics().height()*2);
		m_progress->show();
	} 
}

void TransferListItem::start()
{
	ClientListener* listener;
	if (!m_user)
		return;
	
	switch (m_type)
	{
	case DownloadFileList:
		listener = m_user->server->browseFiles(m_nick);
		break;
	case DownloadFile:
		listener = m_user->server->downloadFile(m_nick, m_path);
		break;
	}
	
	m_transfer = (Client*) listener;
	connect(listener, SIGNAL(stateChanged(int)), SLOT(clientStateChanged(int)));
	connect(listener, SIGNAL(result(int)), SLOT(clientResult(int)));
	connect(listener, SIGNAL(progress(uint, uint)), SLOT(clientProgress(uint, uint)));
	connect(listener, SIGNAL(destroyed()), SLOT(clientDestroyed()));
	connect(listener, SIGNAL(speedChanged(quint64)), SLOT(clientSpeedChanged(quint64)));
}

void TransferListItem::stop()
{
	delete m_transfer;
	m_progress->setText("Paused");
}

void TransferListItem::remove()
{
	delete m_transfer;
	if (firstChild() == NULL)
		deleteLater();
	else
	{
		m_progress->setText("Cancelling...");
		m_timer->start(1000);
	}
}

void TransferListItem::clientResult(int result)
{
	if (m_user == NULL)
		return;
	
	m_result = result;
	
	if (result == Client::TransferFailed)
	{
		m_progress->setText("Error: " + m_transfer->error());
	}
	else
	{
		disconnect(m_transfer);
		m_timer->start(1000);
	}
	
	listView()->repaintItem(this);
}

void TransferListItem::doNextQueued()
{
	TransferListItem* child = (TransferListItem*) firstChild();
	
	if (child == NULL)
	{
		deleteLater();
		return;
	}
	swapWith(child);
	start();
	delete child;
}

void TransferListItem::swapWith(TransferListItem* item)
{
	TransferType oldType = m_type;
	QString oldPath = m_path;
	User* oldUser = m_user;
	
	m_type = item->type();
	m_path = item->path();
	m_user = item->user();
	
	switch(oldType)
	{
	case DownloadFileList:
		item->setFilelistDownload(oldUser);
		break;
	case DownloadFile:
		item->setFileDownload(oldUser, oldPath);
		break;
	}
	
	listView()->repaintItem(item);
}

void TransferListItem::clientStateChanged(int state)
{
	switch(state)
	{
	case Client::NoSlots:
		m_progress->setText("No slots available");
		break;
	case Client::Failed:
		m_progress->setText("Transfer failed");
		break;
	case Client::Success:
		m_progress->setText("Transfer Successful");
		break;
	case Client::WaitingForConnection:
		m_progress->setText("Waiting for peer...");
		break;
	case Client::LookingUpHost:
		m_progress->setText("Looking up host...");
		break;
	case Client::Connecting:
		m_progress->setText("Connecting...");
		break;
	case Client::Handshaking:
		m_progress->setText("Handshaking...");
		break;
	case Client::Transferring:
		m_progress->setText("");
		break;
	case Client::Idle:
		m_progress->setText("Idle");
		break;
	}
}

void TransferListItem::clientProgress(uint size, uint totalSize)
{
	m_progress->setRange(0, totalSize);
	m_progress->setValue(size);
	m_totalSize = totalSize;
	m_size = size;
	if(m_update_time.elapsed() > 100) // 10 fps
	{
		listView()->repaintItem(this);
		m_update_time.restart();
	}
}

void TransferListItem::clientDestroyed()
{
	m_transfer = NULL;
	
	if (m_type == UploadFile)
	{
		m_timer->start(5000);
	}
}

void TransferListItem::paintCell(QPainter* painter, const QColorGroup& cg, int column, int width, int align)
{
	if (column != 0)
		return;
	
	if (listView()->verticalScrollBar()->isVisible())
		width -= listView()->verticalScrollBar()->width();
		
	if (isSelected())
		painter->fillRect(0, 0, width, height(), cg.highlight());
	else
	{
		QColor bgColor;
		if (m_result == Client::TransferFailed)
			bgColor.setRgb(255, 155, 155);
		else if (type() == UploadFile)
			bgColor.setRgb(200, 200, 255);
		else
			bgColor.setRgb(155, 255, 155);
		painter->fillRect(0, 0, width, height(), bgColor);
	}
	
	if (isSelected())
		painter->setPen(cg.highlightedText());
	
	if (m_progress)
	{
		m_progress->resize(width - 6, QApplication::fontMetrics().height());
		
		listView()->moveChild(m_progress, 3, itemPos() + QApplication::fontMetrics().height()*2);
	} 
	
	QFont font = painter->font();
	QFont fontBold = painter->font();
	fontBold.setBold(true);
	QFontMetrics fm(font);
	
	int itemHeight = fm.height();
	
	QRect line1(3, 0, width - 6, itemHeight);
	QRect line2(3, itemHeight, width - 6, itemHeight);
	
	QString line1Text;
	QString line2Text;
	
	if ((m_type == DownloadFile) || (m_type == DownloadFileList))
	{
		line1Text = "Download from " + m_nick;
		if (m_type == DownloadFileList)
			line2Text = "File list";
		else
			line2Text = m_path.section('\\', -1, -1);
	}
	else
	{
		ClientConnector* connector = (ClientConnector*) m_transfer;
		if (!m_nick.isEmpty())
			line1Text = "Upload to " + m_nick;
		else
			line1Text = "Upload";
		
		if (m_path == "MyList.DcLst")
			line2Text = "File list";
		//else if ((connector != NULL) && (!connector->fileName().isEmpty()))
		else
			line2Text = m_path.section('\\', -1, -1);
	}
	
	QString speedText;
	QRect speed;
	if (m_speed != 0)
	{
		speedText = Utilities::niceSize(m_speed);
		speedText += "/s";
		line1.adjust(0, 0, - fm.width(speedText) - 10, 0);
		speed.setTopLeft(QPoint(line1.x() + line1.width() + 10, line1.y()));
		speed.setSize(QSize(fm.width(speedText), line1.height()));
	}
	
	QString sizeText;
	QRect size;
	if (m_totalSize != 0)
	{
		sizeText = (m_size == 0 ? "0 bytes" : Utilities::niceSize(m_size));
		sizeText += "/";
		sizeText += (m_totalSize == 0 ? "0 bytes" : Utilities::niceSize(m_totalSize));
		
		line2.adjust(0, 0, - fm.width(sizeText) - 10, 0);
		size.setTopLeft(QPoint(line2.x() + line2.width() + 10, line2.y()));
		size.setSize(QSize(fm.width(sizeText), line2.height()));
	}
	
	if (Q3ListViewItem::parent())
	{
		line1Text = "Queued: " + line2Text;
		line2Text.clear();
	}
	else
		painter->setFont(fontBold);
	
	painter->drawText(line1, Qt::AlignLeft | Qt::AlignVCenter, line1Text);
	
	if (!line2Text.isEmpty())
	{
		painter->setFont(font);
		painter->drawText(line2, Qt::AlignLeft | Qt::AlignVCenter, line2Text);
		
		if (m_totalSize != 0)
		{
			painter->setFont(font);
			painter->drawText(size, Qt::AlignLeft | Qt::AlignVCenter, sizeText);
		}
	}
	
	if (m_speed != 0)
	{
		painter->setFont(font);
		painter->drawText(speed, Qt::AlignLeft | Qt::AlignVCenter, speedText);
	}
}

void TransferListItem::moveToTop()
{
	TransferListItem* item = (TransferListItem*) Q3ListViewItem::parent();
	if (item->isRunning())
		item = (TransferListItem*) item->firstChild();
	
	swapWith(item);
}

void TransferListItem::moveToBottom()
{
	TransferListItem* item = this;
	if (isTopLevel())
		item = (TransferListItem*) firstChild();
	
	while (item->nextSibling() != NULL)
		item = (TransferListItem*) item->nextSibling();
	
	swapWith(item);
}

void TransferListItem::userQuit(User* user)
{
	if (user == m_user)
	{
		m_user = NULL;
		if (m_progress != NULL)
			m_progress->setText("User left hub");
	}
}

void TransferListItem::clientInfoChanged()
{
	ClientConnector* connector = (ClientConnector*) m_transfer;
	m_nick = connector->nick();
	m_path = connector->fileName();
	listView()->repaintItem(this);
}

void TransferListItem::clientSpeedChanged(quint64 speed)
{
	m_speed = speed;
	listView()->repaintItem(this);
}



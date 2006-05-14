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
#include "resultswidget.h"
#include "user.h"
#include "filelist.h"
#include "filenode.h"
#include "transferlistitem.h"
#include "searchresult.h"
#include "mainwindow.h"
#include "server.h"
#include <Q3ListView>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>

SearchResultListItem::SearchResultListItem(Q3ListView* parent, SearchResult* result)
 : Q3ListViewItem(parent), result(result)
{
	setText(0, result->niceFilename());
		
	if (result->size <= 0)
		setText(1, "(folder)");
	else
		setText(1, result->nicesize);
	setText(2, QString::number(result->openslots) + "/" + QString::number(result->totalslots));
	setText(3, result->nick);
}

int SearchResultListItem::compare(Q3ListViewItem* i, int col, bool ascending) const
{
	if(i->rtti() == 1005)
	{
		SearchResultListItem* other = (SearchResultListItem*)i;
		switch(col)
		{
			case 0:
				return result->filename.toLower().compare(other->result->filename.toLower());
			case 1:
				if(result->size > other->result->size)
					return 1;
				else if(result->size == other->result->size)
					return 0;
				else
					return -1;
			case 2:
				return result->openslots - other->result->openslots;
			case 3:
				return result->nick.toLower().compare(other->result->nick.toLower());
			default:
				return 0;
		}
	}
	else
		return Q3ListViewItem::compare(i, col, ascending);
}


ResultsWidget::ResultsWidget(MainWindow* parent, User* user, Q3ListView* transferList)
 : QWidget(NULL), Ui::UIResultsWidget(), m_user(user), m_transferList(transferList), m_mainWindow(parent), m_isSearchTab(false)
{
	setupUi(this);
	
	connect(listView, SIGNAL(contextMenuRequested(Q3ListViewItem*, const QPoint&, int)), SLOT(contextMenuRequested(Q3ListViewItem*, const QPoint&, int)));
	connect(listView, SIGNAL(doubleClicked(Q3ListViewItem*)), SLOT(download(Q3ListViewItem*)));
	listView->addColumn("Name");
	listView->addColumn("Size");
	listView->header()->setStretchEnabled(true, 0);
	
	m_contextMenu = new QMenu(this);
	m_downloadAction = m_contextMenu->addAction("Download", this, SLOT(downloadSelected()));
	
	searchFrame->hide();
}

ResultsWidget::ResultsWidget(MainWindow* parent, Q3ListView* transferList)
 : QWidget(NULL), Ui::UIResultsWidget(), m_user(NULL), m_transferList(transferList), m_mainWindow(parent), m_isSearchTab(true)
{
	setupUi(this);
	
	connect(listView, SIGNAL(contextMenuRequested(Q3ListViewItem*, const QPoint&, int)), SLOT(contextMenuRequested(Q3ListViewItem*, const QPoint&, int)));
	connect(listView, SIGNAL(doubleClicked(Q3ListViewItem*)), SLOT(download(Q3ListViewItem*)));
	listView->addColumn("Name");
	listView->addColumn("Size");
	listView->addColumn("Slots");
	listView->addColumn("User");
	listView->header()->hide();
	listView->setRootIsDecorated(false);
	listView->header()->setStretchEnabled(true, 0);
	
	m_contextMenu = new QMenu(this);
	m_downloadAction = m_contextMenu->addAction("Download", this, SLOT(downloadSelected()));
	m_contextMenu->addAction("Browse files", this, SLOT(browseFiles()));
	
	connect(searchButton, SIGNAL(clicked()), SLOT(search()));
	connect(sizeRestrictedBox, SIGNAL(currentIndexChanged(int)), SLOT(sizeRestrictionChanged(int)));
	connect(searchBox, SIGNAL(returnPressed()), SLOT(search()));
	
	optionsFrame->hide();
}


ResultsWidget::~ResultsWidget()
{
}

void ResultsWidget::updateList()
{
	listView->clear();
	
	if (m_user != NULL)
	{
		FileNode* rootNode = m_user->fileList->root();
		updateList(NULL, rootNode);
	}
}

void ResultsWidget::updateList(Q3ListViewItem* parentItem, FileNode* parentNode)
{
	foreach(FileNode* node, parentNode->children())
	{
		Q3ListViewItem* item;
		if (parentItem == NULL)
			item = new Q3ListViewItem(listView);
		else
			item = new Q3ListViewItem(parentItem);
		
		item->setText(0, node->name());
		if (node->size() > 0)
			item->setText(1, node->niceSize());
		//item->setOpen(true);
		
		updateList(item, node);
	}
}

void ResultsWidget::contextMenuRequested(Q3ListViewItem* item, const QPoint& pos, int col)
{
	m_contextMenu->popup(pos);
}

void ResultsWidget::downloadSelected()
{
	Q3ListViewItemIterator it(listView);
	while (it.current())
	{
		if (it.current()->isSelected())
			download(it.current());
		++it;
	}
}

void ResultsWidget::download(Q3ListViewItem* item)
{
	QString path;
	User* user;
	
	if (isSearchTab())
	{
		SearchResultListItem* i = (SearchResultListItem*) item;
		// Nasty hack
		Server* server = m_mainWindow->getServer(i->result->hubaddress, i->result->nick);
		if (server == NULL)
		{
			QMessageBox::warning(this, "Error", "This user is offline", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
			return;
		}
		
		path = i->result->filename;
		user = server->getUser(i->result->nick);
	}
	else
	{
		path = item->text(0);
		Q3ListViewItem* parent = item->parent();
		while (parent)
		{
			path = parent->text(0) + "\\" + path;
			parent = parent->parent();
		}
		
		user = m_user;
	}
	
	if (user == NULL)
		return;
	
	Q3ListViewItemIterator it(m_transferList);
	while (it.current())
	{
		TransferListItem* i = (TransferListItem*) it.current();
		
		if ((i->user() == user) && (i->type() == TransferListItem::DownloadFile))
		{
			TransferListItem* transfer = new TransferListItem(i);
			transfer->setFileDownload(user, path);
			i->setOpen(true);
			return;
		}
		
		++it;
	}
	
	TransferListItem* transfer = new TransferListItem(m_transferList);
	transfer->setFileDownload(user, path);
	transfer->start();
}

void ResultsWidget::search()
{
	bool sizeRestricted = (sizeRestrictedBox->currentIndex() != 0);
	bool isMin = (sizeRestrictedBox->currentIndex() == 2);
	
	quint64 size = sizeBox->value();
	switch (sizeTypeBox->currentIndex())
	{
		case 1: size *= 1024; break;
		case 2: size *= 1024*1024; break;
		case 3: size *= 1024*1024*1024; break;
	}
	
	int dataType;
	switch(dataTypeBox->currentIndex())
	{
		case 0: dataType = 1; break;
		case 1: dataType = 7; break;
		case 2: dataType = 2; break;
		case 3: dataType = 3; break;
		case 4: dataType = 6; break;
		case 5: dataType = 5; break;
		case 6: dataType = 4; break;
		case 7: dataType = 8; break;
	}
	
	listView->clear();
	
	emit startSearch(searchBox->text(), sizeRestricted, isMin, size, dataType);
}

void ResultsWidget::resultArrived(QList<SearchResult*> results)
{
	listView->header()->show();
	
	foreach(SearchResult* result, results)
	{
		new SearchResultListItem(listView, result);
	}
}

void ResultsWidget::sizeRestrictionChanged(int index)
{
	sizeBox->setEnabled(index != 0);
	sizeTypeBox->setEnabled(index != 0);
}

void ResultsWidget::browseFiles()
{
	SearchResultListItem* item = (SearchResultListItem*) listView->currentItem();
	if (item == NULL)
		return;
	
	Server* server = m_mainWindow->getServer(item->result->hubaddress, item->result->nick);
	if (server == NULL)
	{
		QMessageBox::warning(this, "Error", "Unable to determine which hub this user belongs to", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		return;
	}
	
	User* user = server->getUser(item->result->nick);
	if (user == NULL)
		return;
	
	TransferListItem* transfer = new TransferListItem(m_transferList);
	transfer->setFilelistDownload(user);
	transfer->start();
}

void ResultsWidget::setUser(User* user)
{
	m_downloadAction->setEnabled(user != NULL);
	m_user = user;
}





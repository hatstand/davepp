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
#ifndef RESULTSWIDGET_H
#define RESULTSWIDGET_H

#include "ui_resultswidget.h"
#include <QWidget>

class User;
class FileNode;
class SearchResult;
class MainWindow;

class SearchResultListItem : public Q3ListViewItem
{
public:
	SearchResultListItem(Q3ListView* parent, SearchResult* result);
	int compare(Q3ListViewItem* i, int col, bool ascending) const;	
	int rtti() const { return 1005; }
	SearchResult* result;
};

class ResultsWidget : public QWidget, public Ui::UIResultsWidget
{
	Q_OBJECT
public:
	ResultsWidget(MainWindow* parent, User* user, Q3ListView* transferList);
	ResultsWidget(MainWindow* parent, Q3ListView* transferList);
	~ResultsWidget();
	
	bool isSearchTab() {return m_isSearchTab;}
	void updateList();
	User* user() {return m_user;}
	void setUser(User* user);
	
signals:
	void startSearch(QString search, bool sizeRestricted, bool isMin, int size, int dataType);
	
public slots:
	void resultArrived(QList<SearchResult*> results);
	
private slots:
	void contextMenuRequested(Q3ListViewItem* item, const QPoint& pos, int col);
	void downloadSelected();
	void downloadToSelected();
	void download(Q3ListViewItem* item, QString destination = QString::null);
	void search();
	void sizeRestrictionChanged(int index);
	void browseFiles();

private:
	void updateList(Q3ListViewItem* parentWidget, FileNode* parentNode);
	
	QMenu* m_contextMenu;
	QAction* m_downloadAction;
	QAction* m_downloadToAction;
	User* m_user;
	Q3ListView* m_transferList;
	MainWindow* m_mainWindow;
	bool m_isSearchTab;
};

#endif

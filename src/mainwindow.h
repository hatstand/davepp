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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QListWidgetItem>
#include <QList>
#include <QPainter>
#include <QSettings>
#include <QProgressBar>

class Server;
class User;
class ChatWidget;
class ActiveSearchListener;
class SearchResult;
class ResultsWidget;
class ConfigureDialog;
class Configuration;
class FileListBuilder;
class ClientConnector;

class HubDetailsListItem : public Q3ListViewItem
{
public:
	HubDetailsListItem(Q3ListView* parent);
	HubDetailsListItem(Q3ListView* parent, QString m_name, QString hostName, int port);
	
	void paintCell(QPainter* painter, const QColorGroup& cg, int column, int width, int align);
	int height() const;
	//int width(const QFontMetrics & fm, const Q3ListView* lv, int c) const;

	QString name() {return m_name;}
	QString hostName() {return m_hostName;}
	int port() {return m_port;}
	bool autoConnect() {return m_autoConnect;}
	Server* connection() {return m_connection;}
	bool isOpen() {return m_connection != NULL;}
	void setName(QString name) {m_name = name;}
	void setHostName(QString hostName) {m_hostName = hostName;}
	void setPort(int port) {m_port = port;}
	void setAutoConnect(bool autoConnect) {m_autoConnect = autoConnect;}
	void setConnection(Server* connection) {m_connection = connection;}
	
	void load(QSettings* settings);
	void save(QSettings* settings);
	
	int rtti() const {return 1001;}

private:
	QString m_name;
	QString m_hostName;
	int m_port;
	bool m_autoConnect;
	
	Server* m_connection;
};

class UserListItem : public Q3ListViewItem
{
public:
	UserListItem(Q3ListView* parent, User* user);
	
	void paintCell(QPainter* painter, const QColorGroup& cg, int column, int width, int align);
	//int height() const;
	int compare( Q3ListViewItem* i, int col, bool ascending ) const;
	int rtti() const {return 1002;}
	
	User* user() {return m_user;}
	
private:
	User* m_user;
};

class UserHeaderListItem : public Q3ListViewItem
{
public:
	UserHeaderListItem(Q3ListView* parent, Server* server);
	
	void paintCell(QPainter* painter, const QColorGroup& cg, int column, int width, int align);
	//int height() const;
	int compare( Q3ListViewItem* i, int col, bool ascending ) const;
	bool isSelectable() {return false;}
	int rtti() const {return 1003;}
	
	Server* server() {return m_server;}
	
private:
	Server* m_server;
};



class MainWindow : public QMainWindow, public Ui::MainWindow
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();
	
	Server* getServer(QString address, QString nick);
	
private slots:
	void newHubClicked();
	void deleteHubClicked();
	void connectHubClicked();
	void editHubClicked();
	void hubListCurrentChanged(Q3ListViewItem* item);
	void hubDoubleClick(Q3ListViewItem* item);
	void hubRightClick(Q3ListViewItem* item, const QPoint& point);
	void userOptionsClicked();
	void userSortChanged(QAction* action);
	void userRightClick(Q3ListViewItem* item, const QPoint& point);
	void browseUserFiles();
	void hideNoSlots();
	void hideNoFiles();
	void hubTabRightClick(int index, const QPoint& p);
	void hubTabDisconnect();
	void hubTabClose();
	void chatTabClose();
	void resortUserList();
	void newPrivateChat(ChatWidget* widget);
	void startSearch(QString search, bool sizeRestricted, bool isMin, int size, int dataType);
	void configure();
	void configureAccept();
	void resultsTabRightClick(int index, const QPoint& point);
	void resultsNewSearch();
	void resultsClose();
	void resultsCloseAll();
	
	void transferRightClick(Q3ListViewItem*, const QPoint& point);
	void transferResume();
	void transferCancel();
	void transferTop();
	void transferBottom();
	
	void serverStateChanged(int newState);
	void userJoined(User* user);
	void userInfoChanged(User* user);
	void userQuit(User* user);
	void userFileListUpdated(User* user);
	void uploadRequest(ClientConnector* connector);
	
	void updateStatusText();
	void builderProgress(uint value, uint totalSteps);
	
private:
	void autoConnect();
	void connectToHub(HubDetailsListItem* item);
	void updateHiddenUsers();
	void updateHiddenUser(UserListItem* item);
	void saveHubList();
	
private:
	QList<Server*> m_hubs;
	QMenu* m_userListMenu;
	QMenu* m_userContextMenu;
	QMenu* m_hubTabContextMenu;
	QMenu* m_chatTabContextMenu;
	QMenu* m_hubContextMenu;
	QMenu* m_transferContextMenu;
	QMenu* m_resultsContextMenu;
	QAction* m_hideNoSlots;
	QAction* m_hideNoFiles;
	QAction* m_hubTabDisconnect;
	QAction* m_hubConnect;
	QAction* m_hubEdit;
	QAction* m_hubDelete;
	QAction* m_transferPause;
	QAction* m_transferTop;
	QAction* m_transferBottom;
	QAction* m_resultsClose;
	QAction* m_resultsCloseAll;
	ConfigureDialog* m_configure;
	Configuration* m_config;
	
	QLabel* statusText1;
	QLabel* statusText2;
	QProgressBar* statusProgress;
	
	ActiveSearchListener* m_activeSearchListener;
	ResultsWidget* m_currentSearchTab;
	
	FileListBuilder* m_fileListBuilder;
	
	int m_tabIndex;
	bool m_sortingLater;

signals:
	void sortUserList();

};

#endif

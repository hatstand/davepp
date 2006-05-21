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
#include "mainwindow.h"
#include "edithub.h"
#include "hubwidget.h"
#include "privatechatwidget.h"
#include "user.h"
#include "resultswidget.h"
#include "server.h"
#include "transferlistitem.h"
#include "activesearchlistener.h"
#include "configuredialog.h"
#include "configuration.h"
#include "filelistbuilder.h"
#include "filelist.h"

#include <QDebug>
#include <QPainter>
#include <QActionGroup>
#include <QDir>

MainWindow* MainWindow::instance = NULL;

HubDetailsListItem::HubDetailsListItem(Q3ListView* parent)
 : Q3ListViewItem(parent), m_port(4977), m_autoConnect(false), m_connection(NULL)
{
}

HubDetailsListItem::HubDetailsListItem(Q3ListView* parent, QString name, QString hostName, int port)
 : Q3ListViewItem(parent)
{
	setName(name);
	setHostName(hostName);
	setPort(port);
}

void HubDetailsListItem::paintCell(QPainter* painter, const QColorGroup& cg, int column, int width, int align)
{
	if (column != 0)
		return;
		
	if (isSelected())
		painter->fillRect(0, 0, width, height(), cg.highlight());
	else
		painter->fillRect(0, 0, width, height(), cg.base());
	
	if (isSelected())
		painter->setPen(cg.highlightedText());
	
	int itemHeight = height();
	QFontMetrics fm = painter->fontMetrics();
	int yPos = ((itemHeight - fm.height()) / 2) + fm.ascent();
	painter->drawText(3, yPos, m_name);
	
	int x = fm.width(m_name + " ");

	QFont font = painter->font();
	font.setPointSize(font.pointSize() - 2);
	font.setWeight(QFont::Bold);
	painter->setFont(font);
	
	QRect rect(3, 0, listView()->viewport()->width() - 3, itemHeight);
	painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, "(" + m_hostName + ":" + QString::number(m_port) + ")");
}

int HubDetailsListItem::height() const
{
	return QApplication::fontMetrics().height() + 1;
}

void HubDetailsListItem::load(QSettings* settings)
{
	setName(settings->value("Name").toString());
	setHostName(settings->value("HostName", "localhost").toString());
	setPort(settings->value("Port", "4562").toInt());
	setAutoConnect(settings->value("AutoConnect", false).toBool());
}

void HubDetailsListItem::save(QSettings* settings)
{
	settings->setValue("Name", m_name);
	settings->setValue("HostName", m_hostName);
	settings->setValue("Port", m_port);
	settings->setValue("AutoConnect", m_autoConnect);
}



UserListItem::UserListItem(Q3ListView* parent, User* user)
 : Q3ListViewItem(parent), m_user(user)
{
	setHeight(QApplication::fontMetrics().height() + 1);
}

void UserListItem::paintCell(QPainter* painter, const QColorGroup& cg, int column, int width, int align)
{
	if (column != 0)
		return;
		
	if (isSelected())
		painter->fillRect(0, 0, width, height(), cg.highlight());
	else
		painter->fillRect(0, 0, width, height(), cg.base());
	
	if (isSelected())
		painter->setPen(cg.highlightedText());
	
	QFont infoFont = painter->font();
	infoFont.setPointSize(infoFont.pointSize() - 2);
	infoFont.setBold(true);
	QFontMetrics infoFm(infoFont);
	QFontMetrics fm(painter->font());
	
	int itemHeight = height();
	int w = width - 6;
	int infoWidth = infoFm.width("      {0/0}   (no files)");
	
	QString nick = m_user->nick;
	if (fm.width(nick) > w - infoWidth)
	{
		while((fm.width(nick + "...") > w - infoWidth) && (nick.length() > 0))
		{
			nick.chop(1);
		}
		nick += "...";
	}
	
	QRect rect(3, 0, w - infoWidth, itemHeight);
	painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, nick);

	painter->setFont(infoFont);
	
	rect = QRect(3, 0, listView()->viewport()->width() - 6, itemHeight);
	painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, m_user->niceShareSize);
	
	if (m_user->totalSlots != -1)
	{
		QString userSlots = "{" + QString::number(m_user->freeSlots) + "/" + QString::number(m_user->totalSlots) + "}";
		
		rect.setWidth(rect.width() - infoFm.width("(no files)   "));
		infoFont.setPointSize(infoFont.pointSize() +1);
		infoFont.setBold(false);
		painter->setFont(infoFont);
		painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, userSlots);
	}
}

int UserListItem::compare( Q3ListViewItem* i, int col, bool ascending ) const
{
	if (i->rtti() == 1003)
	{
		QString otherName = ((UserHeaderListItem*)i)->server()->hubName();
		return m_user->server->hubName().lower().compare(otherName.lower());
	}
	else if(i->rtti() == 1002)
	{
		QString otherName = ((UserListItem*)i)->user()->server->hubName();
		int compare = m_user->server->hubName().lower().compare(otherName.lower());
		if (compare != 0)
			return compare;
	}
	else
		return 0;
	
	UserListItem* item = (UserListItem*) i;
	User* otherUser = item->user();
	
	int serverCompare = m_user->server->hubName().lower().compare(otherUser->server->hubName().lower());
	if (serverCompare != 0)
		return serverCompare;
	
	if (col == 0)
		return m_user->nick.lower().compare(item->user()->nick.lower());
	else
	{
		if (m_user->shareSize > item->user()->shareSize)
			return -1;
		if (m_user->shareSize == item->user()->shareSize)
			return 0;
		return 1;
	} 
}

UserHeaderListItem::UserHeaderListItem(Q3ListView* parent, Server* server)
 : Q3ListViewItem(parent), m_server(server)
{
	qDebug() << "Height before" << height();
	setHeight(QApplication::fontMetrics().height()*5/3 + 1);
	qDebug() << "Height after" << height();
}

void UserHeaderListItem::paintCell(QPainter* painter, const QColorGroup& cg, int column, int width, int align)
{
	if (column != 0)
		return;
	
	painter->fillRect(0, 0, width, height(), cg.base());
	
	int itemHeight = height();
	QFont font = painter->font();
	font.setPointSize(font.pointSize() + 2);
	font.setWeight(QFont::Bold);
	painter->setFont(font);
	
	QRect rect(3, 0, listView()->viewport()->width() - 6, itemHeight - 3);
	painter->drawText(rect, Qt::AlignLeft | Qt::AlignBottom, m_server->hubName());
	painter->drawLine(0, itemHeight - 3, listView()->viewport()->width(), itemHeight - 3);
}

int UserHeaderListItem::compare( Q3ListViewItem* i, int col, bool ascending ) const
{
	QString otherName;
	if (i->rtti() == 1003)
		otherName = ((UserHeaderListItem*)i)->server()->hubName();
	else if(i->rtti() == 1002)
	{
		if (((UserListItem*)i)->user()->server == m_server)
			return -1;
		otherName = ((UserListItem*)i)->user()->server->hubName();
	}
	else
		return 0;
	
	return m_server->hubName().lower().compare(otherName.lower());
}





MainWindow::MainWindow()
 : Ui::MainWindow(), m_sortingLater(false)
{
	instance = this;

	setupUi(this);
	
	connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(actionConfigure, SIGNAL(triggered()), SLOT(configure()));
	
	connect(newHubButton, SIGNAL(clicked()), SLOT(newHubClicked()));
	connect(deleteHubButton, SIGNAL(clicked()), SLOT(deleteHubClicked()));
	connect(connectHubButton, SIGNAL(clicked()), SLOT(connectHubClicked()));
	connect(hubList, SIGNAL(clicked(Q3ListViewItem*)), SLOT(hubListCurrentChanged(Q3ListViewItem*)));
	connect(hubList, SIGNAL(doubleClicked(Q3ListViewItem*)), SLOT(hubDoubleClick(Q3ListViewItem*)));
	connect(hubList, SIGNAL(contextMenuRequested(Q3ListViewItem*, const QPoint&, int)), SLOT(hubRightClick(Q3ListViewItem*, const QPoint&)));
	connect(userOptionsButton, SIGNAL(clicked()), SLOT(userOptionsClicked()));
	connect(userList, SIGNAL(contextMenuRequested(Q3ListViewItem*, const QPoint&, int)), SLOT(userRightClick(Q3ListViewItem*, const QPoint&)));
	connect(userList, SIGNAL(doubleClicked(Q3ListViewItem*)), SLOT(browseUserFiles()));
	connect(hubTabWidget, SIGNAL(tabRightClicked(int, const QPoint&)), SLOT(hubTabRightClick(int, const QPoint&)));
	connect(transferList, SIGNAL(contextMenuRequested(Q3ListViewItem*, const QPoint&, int)), SLOT(transferRightClick(Q3ListViewItem*, const QPoint&)));
	connect(resultsTabWidget, SIGNAL(tabRightClicked(int, const QPoint&)), SLOT(resultsTabRightClick(int, const QPoint&)));
	connect(hubTabWidget, SIGNAL(currentChanged(int)), SLOT(hubTabCurrentChanged()));
	
	
	// Need to store some of this in a config file
	showMaximized();
	
	m_config = new Configuration(this);
	m_fileListBuilder = new FileListBuilder(m_config);
	connect(m_fileListBuilder, SIGNAL(finished()), SLOT(updateStatusText()), Qt::QueuedConnection);
//	connect(m_fileListBuilder, SIGNAL(finished()), SLOT(restartTransfers()), Qt::QueuedConnection);
	connect(m_fileListBuilder, SIGNAL(progress(uint, uint)), SLOT(builderProgress(uint, uint)), Qt::QueuedConnection);
	m_fileListBuilder->start();
	m_configure = new ConfigureDialog(this, m_config);
	
	connect(actionReloadFileList, SIGNAL(triggered()), m_fileListBuilder, SLOT(regenList()));
	connect(m_configure, SIGNAL(accepted()), SLOT(configureAccept()));
	
	int hubCount = m_config->settings()->beginReadArray("Hubs");
	for (int i=0 ; i<hubCount ; ++i)
	{
		m_config->settings()->setArrayIndex(i);
		
		HubDetailsListItem* item = new HubDetailsListItem(hubList);
		item->load(m_config->settings());
	}
	m_config->settings()->endArray();
	
	QActionGroup* sortGroup = new QActionGroup(this);
	QAction* sortByName = new QAction("Sort by name", sortGroup);
	sortByName->setCheckable(true);
	sortByName->setChecked(true);
	sortByName->setData(0);
	QAction* sortBySize = new QAction("Sort by share size", sortGroup);
	sortBySize->setCheckable(true);
	sortBySize->setData(1);
	connect(sortGroup, SIGNAL(triggered(QAction*)), SLOT(userSortChanged(QAction*)));
	
	m_hideNoFiles = new QAction("Hide users with no files", this);
	m_hideNoFiles->setCheckable(true);
	m_hideNoFiles->setChecked(false);
	connect(m_hideNoFiles, SIGNAL(toggled(bool)), SLOT(hideNoFiles()));
	
	m_hideNoSlots = new QAction("Hide users with no slots", this);
	m_hideNoSlots->setCheckable(true);
	m_hideNoSlots->setChecked(false);
	connect(m_hideNoSlots, SIGNAL(toggled(bool)), SLOT(hideNoSlots()));
	
	m_userListMenu = new QMenu(this);
	m_userListMenu->addAction(sortByName);
	m_userListMenu->addAction(sortBySize);
	m_userListMenu->addSeparator();
	m_userListMenu->addAction(m_hideNoSlots);
	m_userListMenu->addAction(m_hideNoFiles);
	
	m_userContextMenu = new QMenu(this);
	m_userContextMenu->addAction("Browse files", this, SLOT(browseUserFiles()));
	m_userContextMenu->addAction("Chat", this, SLOT(requestNewPrivateChat()));

	m_opUserContextMenu = new QMenu(this);
	m_opUserContextMenu->addAction("Browse files", this, SLOT(browseUserFiles()));
	m_opUserContextMenu->addAction("Chat", this, SLOT(requestNewPrivateChat()));
	m_opUserContextMenu->addSeparator();
	m_opUserContextMenu->addAction("Kick", this, SLOT(kickUser()));
	m_opUserContextMenu->addAction("Ban", this, SLOT(banUser()));
	
	m_hubTabDisconnect = new QAction("Disconnect", this);
	connect(m_hubTabDisconnect, SIGNAL(activated()), SLOT(hubTabDisconnect()));
	
	m_hubTabContextMenu = new QMenu(this);
	m_hubTabContextMenu->addAction(m_hubTabDisconnect);
	m_hubTabContextMenu->addAction("Close tab", this, SLOT(hubTabClose()));
	
	m_chatTabContextMenu = new QMenu(this);
	m_chatTabContextMenu->addAction("Close tab", this, SLOT(chatTabClose()));
	
	m_hubContextMenu = new QMenu(this);
	m_hubEdit = m_hubContextMenu->addAction("Edit hub...", this, SLOT(editHubClicked()));
	m_hubDelete = m_hubContextMenu->addAction("Delete hub", this, SLOT(deleteHubClicked()));
	m_hubConnect = m_hubContextMenu->addAction("Connect", this, SLOT(connectHubClicked()));
	m_hubContextMenu->addSeparator();
	m_hubContextMenu->addAction("New hub...", this, SLOT(newHubClicked()));
	
	m_transferContextMenu = new QMenu(this);
	m_transferPause = m_transferContextMenu->addAction("Pause", this, SLOT(transferResume()));
	m_transferContextMenu->addAction("Cancel", this, SLOT(transferCancel()));
	m_transferContextMenu->addSeparator();
	m_transferTop = m_transferContextMenu->addAction("Move to top", this, SLOT(transferTop()));
	m_transferBottom = m_transferContextMenu->addAction("Move to bottom", this, SLOT(transferBottom()));
	
	m_resultsContextMenu = new QMenu(this);
	m_resultsContextMenu->addAction("New search tab", this, SLOT(resultsNewSearch()));
	m_resultsContextMenu->addSeparator();
	m_resultsClose = m_resultsContextMenu->addAction("Close tab", this, SLOT(resultsClose()));
	m_resultsCloseAll = m_resultsContextMenu->addAction("Close all tabs", this, SLOT(resultsCloseAll()));
	
	userList->addColumn("Name");
	userList->header()->setStretchEnabled(true, 0);
	userList->header()->hide();
	
	hubList->addColumn("Name");
	hubList->header()->setStretchEnabled(true, 0);
	hubList->header()->hide();
	
	transferList->setSortColumn(-1);
	transferList->addColumn("Name");
	transferList->header()->setStretchEnabled(true, 0);
	transferList->header()->hide();
	
	m_activeSearchListener = new ActiveSearchListener(this);
	resultsTabWidget->removeTab(0);
	ResultsWidget* searchTab = new ResultsWidget(this, transferList);
	resultsTabWidget->addTab(searchTab, "Search");
	
	connect(searchTab, SIGNAL(startSearch(QString, bool, bool, int, int)), SLOT(startSearch(QString, bool, bool, int, int)));
	
	statusText1 = new QLabel(statusBar());
	statusBar()->addWidget(statusText1, 2);
	statusText2 = new QLabel(statusBar());
	statusBar()->addWidget(statusText2, 2);
	statusProgress = new QProgressBar(statusBar());
	statusBar()->addWidget(statusProgress, 1);
	statusProgress->hide();
	statusProgress->setTextVisible(true);
	updateStatusText();
	
	autoConnect();
}


MainWindow::~MainWindow()
{
	qDebug() << "Deleting mainwindow";
	Q3ListViewItemIterator it(transferList);
	while(it.current())
	{
		TransferListItem* item = (TransferListItem*)(it.current());
		if(item->type() == TransferListItem::DownloadFile)
		{
			delete(item->client());
		}
		++it;
	}
	qDebug() << "MainWindow destructor loop done";
	delete m_config;
}

MainWindow* MainWindow::getInstance()
{
	if(instance == NULL)
		instance = new MainWindow();

	return instance;
}

void MainWindow::autoConnect()
{
	Q3ListViewItem* item = hubList->firstChild();
	while (item)
	{
		HubDetailsListItem* details = (HubDetailsListItem*) item;
		if (details->autoConnect())
			connectToHub(details);
			
		item = item->nextSibling();
	}
}

void MainWindow::newHubClicked()
{
	EditHub* dialog = new EditHub(this);
	dialog->okButton->setText("Add Hub");
	int result = dialog->exec();
	if (result == QDialog::Rejected)
		return;
	
	HubDetailsListItem* item = new HubDetailsListItem(hubList);
	item->setName(dialog->nameBox->text());
	item->setHostName(dialog->hostBox->text());
	item->setPort(dialog->portBox->value());
	item->setAutoConnect(dialog->autoConnectBox->isChecked());
	
	saveHubList();
	
	hubList->setCurrentItem(item);
	hubList->setSelected(item, true);
	hubListCurrentChanged(item);
}

void MainWindow::editHubClicked()
{
	HubDetailsListItem* item = (HubDetailsListItem*) hubList->currentItem();
	if (item == NULL)
		return;
	
	EditHub* dialog = new EditHub(this);
	dialog->nameBox->setText(item->name());
	dialog->hostBox->setText(item->hostName());
	dialog->portBox->setValue(item->port());
	dialog->autoConnectBox->setChecked(item->autoConnect());

	dialog->okButton->setText("Edit Hub");
	
	int result = dialog->exec();
	if (result == QDialog::Rejected)
		return;
	
	item->setName(dialog->nameBox->text());
	item->setHostName(dialog->hostBox->text());
	item->setPort(dialog->portBox->value());
	item->setAutoConnect(dialog->autoConnectBox->isChecked());
	
	saveHubList();
}

void MainWindow::deleteHubClicked()
{
	Q3ListViewItem* item = hubList->currentItem();
	if (item != NULL)
		delete item;
	
	saveHubList();
	
	hubListCurrentChanged(hubList->currentItem());
}

void MainWindow::connectHubClicked()
{
	HubDetailsListItem* item = (HubDetailsListItem*) hubList->currentItem();
	
	for (int i=1 ; i<hubTabWidget->count() ; ++i)
	{
		if (!hubTabWidget->widget(i)->inherits("HubWidget"))
			continue;
		HubWidget* widget = (HubWidget*) hubTabWidget->widget(i);
		if (widget->detailsListItem() == item)
		{
			if (!widget->isConnected())
				hubTabWidget->setCurrentWidget(widget);
			widget->disconnectPressed();
			return;
		}
	}
	connectToHub(item);
}

void MainWindow::connectToHub(HubDetailsListItem* item)
{
	for (int i=1 ; i<hubTabWidget->count() ; ++i)
	{
		if (!hubTabWidget->widget(i)->inherits("HubWidget"))
			continue;
		HubWidget* widget = (HubWidget*) hubTabWidget->widget(i);
		if (widget->detailsListItem() == item)
		{
			hubTabWidget->setCurrentWidget(widget);
			if (!widget->isConnected())
			{
				widget->server()->setHost(item->hostName(), item->port());
				widget->disconnectPressed();
			}
			return;
		}
	}
	
	Server* server = new Server(this);
	
	connect(server, SIGNAL(stateChanged(int)), SLOT(serverStateChanged(int)));
	connect(server, SIGNAL(userJoined(User*)), SLOT(userJoined(User*)));
	connect(server, SIGNAL(userInfoChanged(User*)), SLOT(userInfoChanged(User*)));
	connect(server, SIGNAL(userQuit(User*)), SLOT(userQuit(User*)));
	connect(server, SIGNAL(fileListUpdated(User*)), SLOT(userFileListUpdated(User*)));
	connect(server, SIGNAL(uploadRequest(ClientConnector*)), SLOT(uploadRequest(ClientConnector*)));
	
	server->setHost(item->hostName(), item->port());
	
	HubWidget* hubWidget = new HubWidget(item, server, userList);
	hubTabWidget->addTab(hubWidget, item->name());
	hubTabWidget->setCurrentWidget(hubWidget);
	
	connect(this, SIGNAL(sortUserList()), SLOT(resortUserList()));
	
	server->open();
	
	m_hubs << server;
}

void MainWindow::hubListCurrentChanged(Q3ListViewItem* item)
{
	qDebug() << "Current changed" << (uintptr_t)item;
	deleteHubButton->setEnabled(item != NULL);
	connectHubButton->setEnabled(item != NULL);
	
	bool connected = false;
	for (int i=1 ; i<hubTabWidget->count() ; ++i)
	{
		if (!hubTabWidget->widget(i)->inherits("HubWidget"))
			continue;
		HubWidget* widget = (HubWidget*) hubTabWidget->widget(i);
		if (widget->detailsListItem() == item)
		{
			if (widget->isConnected())
				connected = true;
			break;
		}
	}
	
	if (connected)
	{
		connectHubButton->setText("Disconnect");
		m_hubConnect->setText("Disconnect");
	}
	else
	{
		connectHubButton->setText("Connect");
		m_hubConnect->setText("Connect");
	}
}

void MainWindow::serverStateChanged(int newState)
{
	hubListCurrentChanged(hubList->currentItem());
}

void MainWindow::userJoined(User* user)
{
	UserListItem* item = new UserListItem(userList, user);
	user->listItem = item;
	
	if (!m_sortingLater)
	{
	//	QTimer::singleShot(0, this, SLOT(resortUserList()));
		emit sortUserList();
		m_sortingLater = true;
	}
	
	for (int i=1 ; i<resultsTabWidget->count() ; ++i)
	{
		if (!resultsTabWidget->widget(i)->inherits("ResultsWidget"))
			continue;
		ResultsWidget* widget = (ResultsWidget*) resultsTabWidget->widget(i);
		if ((widget->user() == NULL) && (resultsTabWidget->tabText(i) == user->nick + " (offline)"))
		{
			resultsTabWidget->setTabText(i, user->nick);
			widget->setUser(user);
		}
	}
	
	updateStatusText();
}

void MainWindow::userInfoChanged(User* user)
{
	UserListItem* item = (UserListItem*) user->listItem;
	updateHiddenUser(item);
	userList->repaintItem(item);
	
	if (!m_sortingLater)
	{
//		QTimer::singleShot(0, this, SLOT(resortUserList()));
		emit sortUserList();
		m_sortingLater = true;
	}
	
	updateStatusText();
}

void MainWindow::userQuit(User* user)
{
	qDebug() << user->nick << "quit";
	
	for (int i=1 ; i<resultsTabWidget->count() ; ++i)
	{
		if (!resultsTabWidget->widget(i)->inherits("ResultsWidget"))
			continue;
		ResultsWidget* widget = (ResultsWidget*) resultsTabWidget->widget(i);
		if (widget->user() != user)
			continue;
		widget->setUser(NULL);
		resultsTabWidget->setTabText(i, user->nick + " (offline)");
		break;
	}
	
	if (((Q3ListViewItem*)user->listItem)->rtti() != 1002)
		return;
	UserListItem* item = (UserListItem*) user->listItem;
	delete item;
	
	updateStatusText();
}

void MainWindow::userOptionsClicked()
{
	int menuWidth = m_userListMenu->sizeHint().width();
	
	QPoint point = userOptionsButton->mapToGlobal(QPoint(0,userOptionsButton->height()));
	point.setX(point.x() - menuWidth + userOptionsButton->width());
	
	m_userListMenu->popup(point);
}

void MainWindow::userSortChanged(QAction* action)
{
	int sortType = action->data().toInt();
	
	if (sortType == 0) // Name
		userList->setSorting(0, true);
	else // Size
		userList->setSorting(1, true);
	
	if (!m_sortingLater)
	{
//		QTimer::singleShot(0, this, SLOT(resortUserList()));
		emit sortUserList();
		m_sortingLater = true;
	}
}

void MainWindow::userRightClick(Q3ListViewItem* item, const QPoint& point)
{
	if ((!item) || (item->rtti() != 1002))
		return;

	UserListItem* bob = (UserListItem*)item;

	if(bob->user()->server->me()->op)
		m_opUserContextMenu->popup(point);
	else
		m_userContextMenu->popup(point);
}

UserListItem* MainWindow::getUserListItem()
{
	if(userList->currentItem()->rtti() != 1002)
		return NULL;
	else
		return (UserListItem*)userList->currentItem();
}

void MainWindow::browseUserFiles()
{
	UserListItem* item = getUserListItem();
	
	if(item != NULL)
	{
		TransferListItem* transfer = new TransferListItem(transferList);
		transfer->setFilelistDownload(item->user());
		transfer->start();
	}
}

void MainWindow::requestNewPrivateChat()
{
	UserListItem* item = getUserListItem();

	if(item != NULL)
	{
		User* user = item->user();
		user->server->getPrivateChat(user->nick);
	}
}

void MainWindow::kickUser()
{
	UserListItem* item = getUserListItem();

	if(item != NULL)
	{
		User* user = item->user();
		user->server->kickUser(user->nick);
	}
}

void MainWindow::banUser()
{
	UserListItem* item = getUserListItem();

	if(item != NULL)
	{
		User* user = item->user();
		// Do nothing. Ban is non-standard
	}
}

void MainWindow::hubDoubleClick(Q3ListViewItem* item)
{
	if (item != NULL)
		connectToHub((HubDetailsListItem*)item);
}

void MainWindow::hubRightClick(Q3ListViewItem* item, const QPoint& point)
{
	bool selectedHub = (hubList->selectedItem() != NULL);
	
	m_hubEdit->setEnabled(selectedHub);
	m_hubDelete->setEnabled(selectedHub);
	m_hubConnect->setEnabled(selectedHub);
	m_hubContextMenu->popup(point);
}

void MainWindow::hideNoFiles()
{
	updateHiddenUsers();
}

void MainWindow::hideNoSlots()
{
	updateHiddenUsers();
}

void MainWindow::updateHiddenUsers()
{
	Q3ListViewItemIterator it(userList);
	while (it.current())
	{
		if (it.current()->rtti() == 1002)
		{
			updateHiddenUser((UserListItem*)it.current());
		}
		++it;
	}
}

void MainWindow::updateHiddenUser(UserListItem* item)
{
	bool visible = true;
	if ((item->user()->shareSize <= 0) && (m_hideNoFiles->isChecked()))
		visible = false;
	else if ((item->user()->freeSlots == 0) && (m_hideNoSlots->isChecked()))
		visible = false;
	
	item->setVisible(visible);
}

void MainWindow::hubTabRightClick(int index, const QPoint& p)
{
	m_tabIndex = index;
	
	if (index == 0)
		return;
	
	if (hubTabWidget->widget(m_tabIndex)->inherits("HubWidget"))
	{
		HubWidget* widget = (HubWidget*) hubTabWidget->widget(m_tabIndex);
		m_hubTabDisconnect->setText(widget->isConnected() ? "Disconnect" : "Connect");
		m_hubTabContextMenu->popup(p);
	}
	else
	{
		m_chatTabContextMenu->popup(p);
	}
}

void MainWindow::hubTabDisconnect()
{
	HubWidget* widget = (HubWidget*) hubTabWidget->widget(m_tabIndex);
	widget->disconnectPressed();
}

void MainWindow::hubTabClose()
{
	HubWidget* widget = (HubWidget*) hubTabWidget->widget(m_tabIndex);
	
	m_hubs.removeAll(widget->server());
	
	hubTabWidget->removeTab(hubTabWidget->indexOf(widget));
	delete widget;
}

void MainWindow::chatTabClose()
{
	PrivateChatWidget* widget = (PrivateChatWidget*) hubTabWidget->widget(m_tabIndex);
	delete widget;
}

void MainWindow::resortUserList()
{
	userList->sort();
	m_sortingLater = false;
}

void MainWindow::newPrivateChat(PrivateChatWidget* widget)
{
	hubTabWidget->addTab(widget, widget->nick() + " (chat)");
}

void MainWindow::privateChatClosed(PrivateChatWidget* widget)
{
	hubTabWidget->removeTab(hubTabWidget->indexOf(widget));
}

void MainWindow::userFileListUpdated(User* user)
{
	for (int i=1 ; i<resultsTabWidget->count() ; ++i)
	{
		if (!resultsTabWidget->widget(i)->inherits("ResultsWidget"))
			continue;
		ResultsWidget* widget = (ResultsWidget*) resultsTabWidget->widget(i);
		if (widget->user() == user)
		{
			widget->updateList();
			resultsTabWidget->setCurrentWidget(widget);
			return;
		}
	}
	
	ResultsWidget* widget = new ResultsWidget(this, user, transferList);
	widget->updateList();
	resultsTabWidget->addTab(widget, user->nick);
	resultsTabWidget->setCurrentWidget(widget);
}

void MainWindow::startSearch(QString search, bool sizeRestricted, bool isMin, int size, int dataType)
{
	Q_ASSERT(m_activeSearchListener != NULL);
	disconnect(m_activeSearchListener, SIGNAL(resultArrived(QList<SearchResult*>)), 0, 0);
	connect(m_activeSearchListener, SIGNAL(resultArrived(QList<SearchResult*>)), sender(), SLOT(resultArrived(QList<SearchResult*>)));
	foreach(Server* server, m_hubs)
	{
		if (server->state() != Server::Connected)
			continue;
		
		server->searchHub(m_activeSearchListener->port(), search, sizeRestricted, isMin, size, dataType);
	}
	
	int index = resultsTabWidget->indexOf((QWidget*)sender());
	if (index != -1)
	{
		resultsTabWidget->setTabText(index, search);
	}
}

Server* MainWindow::getServer(QString address, QString nick)
{
	QString host = address.section(':', 0, 0);
	QRegExp re("[0-9\\.]*");
	qDebug() << "MAINWINDOW" << (uintptr_t)this;
	
	if (re.exactMatch(host) != -1)
	{
		foreach(Server* server, m_hubs)
		{
			qDebug() << address << server->ip() << host;
			if (server->ip() == host)
				return server;
		}
	}
	
	foreach(Server* server, m_hubs)
	{
		if (server->getUser(nick) != NULL)
			return server;
	}
	
	return NULL;
}

void MainWindow::configure()
{
	m_configure->show();
}

void MainWindow::configureAccept()
{
	if (m_config->isFileListDirty())
	{
		m_fileListBuilder->start();
		updateStatusText();
	}
}

void MainWindow::transferRightClick(Q3ListViewItem*, const QPoint& point)
{
	TransferListItem* item = (TransferListItem*) transferList->currentItem();
	
	if (item == NULL)
		return;
	
	m_transferPause->setText(item->isRunning() ? "Pause" : "Resume");
	m_transferPause->setEnabled(item->isTopLevel() && !item->isUserOffline() && (item->type() != TransferListItem::UploadFile));
	m_transferTop->setEnabled(!item->isTopLevel() && !item->isTopOfQueue());
	m_transferBottom->setEnabled((item->isTopLevel() && (item->firstChild() != NULL)) || (!item->isTopLevel() && !item->isBottomOfQueue()));
	m_transferContextMenu->popup(point);
}

void MainWindow::transferResume()
{
	TransferListItem* item = (TransferListItem*) transferList->currentItem();
	if (item->isRunning())
		item->stop();
	else if (item->isTopLevel())
		item->start();
}

void MainWindow::transferCancel()
{
	TransferListItem* item = (TransferListItem*) transferList->currentItem();
	item->remove();
}

void MainWindow::transferTop()
{
	TransferListItem* item = (TransferListItem*) transferList->currentItem();
	if (!item->isTopLevel() && !item->isTopOfQueue())
		item->moveToTop();
}

void MainWindow::transferBottom()
{
	TransferListItem* item = (TransferListItem*) transferList->currentItem();
	if (item->isTopLevel())
	{
		if (item->isRunning())
			item->stop();
		
		item->moveToBottom();
	}
	else if (!item->isBottomOfQueue())
		item->moveToBottom();
}

void MainWindow::resultsTabRightClick(int index, const QPoint& point)
{
	m_tabIndex = index;
	
	m_resultsClose->setEnabled(index != 0);
	m_resultsCloseAll->setEnabled(resultsTabWidget->count() > 1);
	
	m_resultsContextMenu->popup(point);
}

void MainWindow::resultsNewSearch()
{
	ResultsWidget* searchTab = new ResultsWidget(this, transferList);
	resultsTabWidget->addTab(searchTab, "Search");
	resultsTabWidget->setCurrentWidget(searchTab);
	
	connect(searchTab, SIGNAL(startSearch(QString, bool, bool, int, int)), SLOT(startSearch(QString, bool, bool, int, int)));
}

void MainWindow::resultsClose()
{
	if (m_tabIndex == 0)
		return;
	
	delete resultsTabWidget->widget(m_tabIndex);
	//resultsTabWidget->removeTab(m_tabIndex);
}

void MainWindow::resultsCloseAll()
{
	for (int i=resultsTabWidget->count()-1 ; i>0 ; --i)
	{
		delete resultsTabWidget->widget(i);
		//resultsTabWidget->removeTab(i);
	}
}

void MainWindow::saveHubList()
{
	m_config->settings()->beginWriteArray("Hubs");
	
	int i = 0;
	Q3ListViewItem* item = hubList->firstChild();
	while (item)
	{
		m_config->settings()->setArrayIndex(i++);
		((HubDetailsListItem*)item)->save(m_config->settings());
		
		item = item->nextSibling();
	}
	
	m_config->settings()->endArray();
	m_config->settings()->sync();
}

void MainWindow::uploadRequest(ClientConnector* connector)
{
	TransferListItem* transfer = new TransferListItem(transferList);
	transfer->setFileUpload(connector);
}

void MainWindow::updateStatusText()
{
	int hubCount = 0;
	int userCount = 0;
	quint64 sizeCount = 0;
	
	foreach(Server* hub, m_hubs)
	{
		if (hub->state() != Server::Connected)
			continue;
		hubCount++;
		QListIterator<User*> it = hub->users();
		while (it.hasNext())
		{
			User* user = it.next();
			userCount++;
			sizeCount+=user->shareSize;
		}
	}
	
	if (hubCount == 0)
		statusText1->setText(" Not connected to any hubs");
	else
	{
		QString niceSize = sizeCount == 0 ? "0 bytes" : Utilities::niceSize(sizeCount);
		statusText1->setText(" Connected to " + QString::number(hubCount) + " hub" + (hubCount != 1 ? "s" : "") + " with " + QString::number(userCount) + " user" + (userCount != 1 ? "s" : "") + " sharing " + niceSize);
	}
	
	if (!m_fileListBuilder->isReady())
	{
		statusText2->setText(" Building filelist...");
		statusProgress->show();
	}
	else
	{
		quint64 shareSize = 0;
		FileList* list = m_fileListBuilder->list();
		if (list != NULL)
			shareSize = list->totalSize();
		
		if (shareSize == 0)
			statusText2->setText(" You are not sharing any files");
		else
			statusText2->setText(" You are sharing " + Utilities::niceSize(shareSize));
		statusProgress->hide();
	}
}

void MainWindow::builderProgress(uint value, uint totalSteps)
{
	statusProgress->setRange(0, totalSteps);
	statusProgress->setValue(value);
}

void MainWindow::hubTabCurrentChanged()
{
	QWidget* w = hubTabWidget->currentWidget();
	if (!w->inherits("ChatWidget"))
		return;
	
	((ChatWidget*)w)->setInputBoxFocus();
}

void MainWindow::restartTransfers()
{
	qDebug() << "Restarting transfers";
	// Only need to do this once
	disconnect(FileListBuilder::instance(), SIGNAL(finished()), this, SLOT(restartTransfers()));
	
	QSettings* settings = m_config->settings();

	settings->beginGroup("Files");
	QStringList files = settings->childGroups();
	foreach(QString path, files)
	{
		qDebug() << "Found saved download";
		settings->beginGroup(path);
		QString hub = settings->value("Hub").toString();
		QString remotenick = settings->value("RemoteNick").toString();
		QString remotepath = settings->value("RemotePath").toString();
		quint64 bytesdone = settings->value("BytesDone").toULongLong();
		settings->endGroup();
		Server* s = findServer(hub); // Try our local server list

		if(s == NULL)
		{
			Q3ListViewItemIterator it(hubList); // Try the saved hub list
			while(it.current())
			{
				HubDetailsListItem* item = (HubDetailsListItem*)(it.current());
				if(item->name() == hub)
				{
					s = item->connection();
					if(!item->isOpen())
					{
						s = new Server(this);
						s->setHost(item->hostName(), item->port());
						m_hubs << s;
						item->setConnection(s);
					}
					break;
				}
			}
			qWarning() << "Couldn't find hub for saved download";
			return;
		}

		settings->endGroup();

		if(s->state() == Server::NotConnected)
		{
			s->open();
		}
	
		qDebug() << "Resuming download (MainWindow)";
		s->resumeDownload(remotenick, remotepath, path, bytesdone);
	}
}

Server* MainWindow::findServer(QString name)
{
	foreach(Server* s, m_hubs)
	{
		if(s->hubName() == name)
			return s;
	}

	return NULL;
}

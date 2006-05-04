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
#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QTimer>

class HubWidget;
#include <ui_hubwidget.h>

class ChatWidget : public QWidget, public Ui::UIHubWidget
{
	Q_OBJECT
public:
	ChatWidget(HubWidget* hub, QString nick);
	~ChatWidget();
	
	HubWidget* hub() {return m_hub;}
	QString nick() {return m_nick;}
	void printTime();

private:
	HubWidget* m_hub;
	QString m_nick;
	QTimer* m_timer;

private slots:
	void sendPressed();
};

#endif

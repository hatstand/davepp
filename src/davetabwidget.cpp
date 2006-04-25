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
#include "davetabwidget.h"

#include <QMouseEvent>
#include <QDebug>

DaveTabBar::DaveTabBar(QWidget* parent)
 : QTabBar(parent)
{
}

int DaveTabBar::indexAtPos(const QPoint& p)
{
	if (tabRect(currentIndex()).contains(p))
		return currentIndex();
	for (int i = 0; i < count(); ++i)
		if (tabRect(i).contains(p))
			return i;
	return -1;
}


void DaveTabBar::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::RightButton)
	{
		int index = indexAtPos(e->pos());
		if (index != -1)
		{
			emit tabRightClicked(index, mapToGlobal(e->pos()));
			return;
		}
	}
	QTabBar::mousePressEvent(e);
}


DaveTabWidget::DaveTabWidget(QWidget* parent)
 : QTabWidget(parent)
{
	DaveTabBar* bar = new DaveTabBar(this);
	setTabBar(bar);
	
	connect(bar, SIGNAL(tabRightClicked(int, const QPoint&)), SIGNAL(tabRightClicked(int, const QPoint&)));
}


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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QPainter>
#include <QApplication>

#include "mainwindow.h"

// Test harness for encodeList
#include <QFile>
#include <QTemporaryFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include "utilities.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	MainWindow* w = new MainWindow();
	w->setAttribute(Qt::WA_DeleteOnClose);
	w->show();
	
	return app.exec();
}

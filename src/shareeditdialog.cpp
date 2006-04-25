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
#include "shareeditdialog.h"

#include <QtGui/QFileDialog>
#include <QtCore/QDir>

ShareEditDialog::ShareEditDialog(QWidget* parent)
 : QDialog(parent), Ui::UIShareEditDialog()
{
	setupUi(this);
	connect(browseButton, SIGNAL(pressed()), SLOT(browse()));
}


ShareEditDialog::~ShareEditDialog()
{
}

void ShareEditDialog::browse()
{
	QString fileName = QFileDialog::getExistingDirectory(this, "Choose a folder", QDir::homeDirPath());
	
	if (fileName.isEmpty())
		return;
	
	folderBox->setText(fileName);
	while (fileName.endsWith(QDir::separator()))
		fileName.chop(1);
		
	nameBox->setText(fileName.section(QDir::separator(), -1, -1));
}



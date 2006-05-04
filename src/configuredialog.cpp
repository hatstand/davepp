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
#include "configuredialog.h"
#include "shareeditdialog.h"
#include "configuration.h"

#include <QDebug>

ConfigureDialog::ConfigureDialog(QWidget* parent, Configuration* config)
 : QDialog(parent), Ui::UIConfigureDialog(), m_config(config), m_shareListDirty(false)
{
	setupUi(this);
	
	connect(shareAddButton, SIGNAL(clicked()), SLOT(shareAdd()));
	connect(shareRemoveButton, SIGNAL(clicked()), SLOT(shareRemove()));
	connect(shareEditButton, SIGNAL(clicked()), SLOT(shareEdit()));
	
	shareList->addColumn("Name");
	shareList->addColumn("Folder");
}


ConfigureDialog::~ConfigureDialog()
{
}


void ConfigureDialog::shareAdd()
{
	ShareEditDialog* dialog = new ShareEditDialog(this);
	if (dialog->exec() == QDialog::Rejected)
		return;
	
	if (dialog->folderBox->text().isEmpty() || dialog->nameBox->text().isEmpty())
		return;
	
	Q3ListViewItem* item = new Q3ListViewItem(shareList);
	item->setText(0, dialog->nameBox->text());
	item->setText(1, dialog->folderBox->text());
	
	m_shareListDirty = true;
}

void ConfigureDialog::shareRemove()
{
	Q3ListViewItem* item = shareList->currentItem();
	if (item == NULL)
		return;
	
	m_shareListDirty = true;
	
	delete item;
}

void ConfigureDialog::shareEdit()
{
	Q3ListViewItem* item = shareList->currentItem();
	if (item == NULL)
		return;
	
	ShareEditDialog* dialog = new ShareEditDialog(this);
	dialog->folderBox->setText(item->text(1));
	dialog->nameBox->setText(item->text(0));
	if (dialog->exec() == QDialog::Rejected)
		return;
	
	if (dialog->folderBox->text().isEmpty() || dialog->nameBox->text().isEmpty())
		return;
	
	m_shareListDirty = true;
	
	item->setText(0, dialog->nameBox->text());
	item->setText(1, dialog->folderBox->text());
}

void ConfigureDialog::accept()
{
	if (m_shareListDirty)
	{
		m_config->sharesClear();
		
		Q3ListViewItemIterator it(shareList);
		while (it.current())
		{
			m_config->shareAdd(it.current()->text(0), it.current()->text(1));
			++it;
		}
	}
	m_config->setUploadSpeed(uploadSpeedBox->value());
	m_config->setNumSlots(slotsBox->value());
	m_config->setNick(nickBox->text());
	m_config->setEmail(emailBox->text());
	m_config->setDescription(descriptionBox->text());
	m_config->setConnSpeed(connSpeedBox->currentIndex());
	m_config->setDownloadDir(downloadDirBox->text());
	m_config->save();
	
	QDialog::accept();
}

void ConfigureDialog::show()
{
	shareList->clear();
	QMapIterator<QString, QString> it(m_config->sharedDirs());
	while (it.hasNext())
	{
		it.next();
		Q3ListViewItem* item = new Q3ListViewItem(shareList);
		item->setText(0, it.key());
		item->setText(1, it.value());
	}
	
	uploadSpeedBox->setValue(m_config->uploadSpeed());
	slotsBox->setValue(m_config->numSlots());
	
	nickBox->setText(m_config->nick());
	emailBox->setText(m_config->email());
	descriptionBox->setText(m_config->description());
	connSpeedBox->setCurrentIndex(m_config->connSpeed());
	downloadDirBox->setText(m_config->downloadDir());
	
	QDialog::show();
}

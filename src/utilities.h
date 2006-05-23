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
#ifndef UTILITIES_H
#define UTILITIES_H

#include <QString>
#include <QFile>
#include <QByteArray>

struct hufencode
{
	quint32 bit_len;
	quint32 bits;
};

struct hufnode
{
	quint32 occur;
	hufnode* left;
	hufnode* right;
	uchar val;
};

// This class is a wrapper around QByteArray
// It allows a user to append bits to the end of a string of bits
// The byte array returned by toByteArray is in the correct format
// to be used in a DcList, ie:
//  * Each byte is ordered from LSB to MSB
//  * Bits are aligned to byte boundaries
class BitArray
{
public:
	BitArray();
	
	void append(quint32 bits, quint32 length);
	void clear();
	QByteArray toByteArray();
	
private:
	QByteArray data;
	quint32 lastByte;
	int lastByteLength;
};

namespace Utilities
{
	QString lockToKey(QString lock);
	
	int reservedchar(unsigned char c);
	char* dcmakekey(const char *lock);
	
	QByteArray decodeList(QByteArray inputData);
	QByteArray decodeBZList(QByteArray inputData);

	QByteArray encodeList(QByteArray inputData);
	QByteArray encodeBZList(QByteArray inputData);

	quint32 get_bit(QByteArray data, quint32* cur_pos);
	quint32 get_bits(QByteArray data, quint32* cur_pos, qint32 nb_bit);
	void useHufNode(hufencode tblenc[256], hufnode* node, quint32 bits_len, quint32 bits);
	QByteArray* add_bits(QByteArray* data, quint32* bit_pos, quint32 pattern, quint32 pattern_length);
	QByteArray* add_bit(QByteArray* data, quint32* bit_pos, uchar bit_value);
	void freeHufNode(hufnode* node);
	//bool huf_insert_glist(hufnode* a, hufnode* b);
	QString niceSize(quint64 size);
	int randomDirection();
}

#endif

/*
 * Copyright (C) 2001-2005 Jacek Sieka, arnetheduck on gmail point com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

// Yoinked from DC++

#ifndef TIGER_HASH_H
#define TIGER_HASH_H

#include <QByteArray>

class TigerHash {
public:
	/** Hash size in bytes */
	enum { HASH_SIZE = 24 };

	TigerHash() : pos(0) 
	{
		res[0]=quint64(0x0123456789ABCDEF);
		res[1]=quint64(0xFEDCBA9876543210);
		res[2]=quint64(0xF096A5B4C3B2E187);
	}

	TigerHash(QByteArray chunk);

	~TigerHash() {}

	/** Calculates the Tiger hash of the data. */
	void update(const void* data, quint32 len);
	/** Call once all data has been processed. */
	quint64* finalize();

	quint64* getResult() { return res; }
	QByteArray toByteArray();
private:
	enum { BLOCK_SIZE = 512/8 };
	/** 512 bit blocks for the compress function */
	quint8 tmp[512/8];
	/** State / final hash value */
	quint64 res[3];
	/** Total number of bytes compressed */
	quint64 pos;
	/** S boxes */
	static quint64 table[];

	void tigerCompress(const quint64* data, quint64 state[3]);
};

#endif // !defined(TIGER_HASH_H)

/**
 * @file
 * $Id: TigerHash.h,v 1.9 2006/02/19 16:19:06 arnetheduck Exp $
 */

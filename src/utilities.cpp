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
#include "utilities.h"

#include <QByteArray>
#include <QDebug>

#include <set>
#include <iostream>

#include <bzlib.h>

using namespace Utilities;

QString Utilities::lockToKey(QString lock)
{
	char* key = dcmakekey(lock.toAscii().data());
	QString ret = QString::fromAscii(key);
	delete(key);
	return ret;
}

/* Taken from Dolda Connect -- Licensed under the GPL */
int Utilities::reservedchar(unsigned char c)
{
    return((c == 0) || (c == 5) || (c == 124) || (c == 96) || (c == 126) || (c == 36));
}

char* Utilities::dcmakekey(const char *lock)
{
    int i, len, offset;
    char *buf, *key;
    char save;
    
    /* Step 1: Compute key */
//    buf = (char*) malloc(strlen(lock));
	 buf = new char[strlen(lock)];
    save = 5;
    len = 0;
    for(i = 0; lock[i]; i++) {
        buf[i] = lock[i] ^ save;
        buf[i] = ((buf[i] & 0x0F) << 4) | ((buf[i] & 0xF0) >> 4);
        save = lock[i];
        if((i != 0) && reservedchar(buf[i]))
            len += 10;
        else
            len++;
    }
    buf[0] ^= buf[i - 1];
    if(reservedchar(buf[0]))
        len += 10;
    else
        len++;
    
    /* Step 2: Quote reserved characters */
    key = (char*)malloc(len + 1);
    offset = 0;
    for(i = 0; lock[i] != 0; i++) {
        if(reservedchar(buf[i]))
            offset += sprintf(key + offset, "/%%DCN%03i%%/", buf[i]);
        else
            key[offset++] = buf[i];
    }
    key[offset] = 0;
    delete(buf);
    
    /* Observe: The caller will have to free the memory */
    return(key);
}


quint32 Utilities::get_bit(QByteArray data, quint32* cur_pos)
{
	quint32 out;

	out = ((quint32)data[(*cur_pos)/8] >> ((*cur_pos) & 7)) & 1;

	++(*cur_pos);

	return out;
}

quint32 Utilities::get_bits(QByteArray data, quint32* cur_pos, qint32 nb_bit)
{
	int i;
	quint32 res = 0;

	for(i = 0; i < nb_bit; ++i)
	{
		res = (res << 1) | get_bit(data, cur_pos);
	}

	return res;
}

QByteArray Utilities::decodeList(QByteArray inputData)
{
	qDebug() << "Decoding List...";

	QByteArray output;

	// These bytes must be there
	if(inputData.size() < 4 || inputData.at(0) != 'H' || inputData.at(1) != 'E' || inputData.at(2) != '3' || inputData.at(3) != 0xD)
	{
		qDebug() << "Decompression Error";
		return NULL;
	}
	
	// Ripped from DCLib and cross-platform-ified
	// http://dcgui.berlios.de
			
	int pos;
	qint32 nb_output;
	qint32 nb_couple;
	qint32 max_len = 0;
	qint32 ttl_len = 0;

	quint32 offset_pattern;
	quint32 offset_encoded;

	nb_output = ((qint32)inputData[8]) & 255;
	nb_output <<= 8;
	nb_output |= ((qint32)inputData[7]) & 255;
	nb_output <<= 8;
	nb_output |= ((qint32)inputData[6]) & 255;
	nb_output <<= 8;
	nb_output |= ((qint32)inputData[5]) & 255;

	nb_couple = inputData[9];
	nb_couple += ((((qint32)inputData[10]) & 255) << 8);

	for(pos = 0; pos < nb_couple; ++pos)
	{
		qint32 v;

		v = ((qint32)inputData[12 + pos*2]) & 255;
		
		if (v > max_len)
			max_len = v;

		ttl_len += v;
	}

	QMap<int, int> decode_array;

	offset_pattern=8 * (11 + nb_couple * 2);     /* position of the pattern block, it is just after the list of couples */
	offset_encoded=offset_pattern + ((ttl_len + 7) &~ 7);

	for(pos = 0; pos < nb_couple; ++pos)
	{
		quint32 v_len;
		quint32 value;

		v_len = ((qint32)inputData[12 + pos * 2]) & 255;

		value = get_bits(inputData, &offset_pattern, v_len);

		decode_array[(1 << v_len) + value] = inputData[11 + pos * 2];
	}

	int l = 0;

	while(output.size() != nb_output)
	{
		quint32 cur_val;
		quint32 nb_bit_val;

		cur_val = get_bit(inputData, &offset_encoded);
		nb_bit_val = 1;

		while(!decode_array.contains((1 << nb_bit_val) + cur_val))
		{
			cur_val = (cur_val << 1) | get_bit(inputData, &offset_encoded);
			++nb_bit_val;
		}

		output += decode_array[(1 << nb_bit_val) + cur_val];

		++l;
	}

	int i;
	quint8 parity = 0;

	for(i = 0; i < output.size(); ++i)
		parity ^= output[i];

	qDebug() << "Decoded List";

	return output;
}

void Utilities::useHufNode(hufencode tbl_enc[256], hufnode* node, quint32 bit_len, quint32 bits)
{
	if(node->left != NULL)
	{
		useHufNode(tbl_enc, node->left, bit_len + 1, bits*2);
		useHufNode(tbl_enc, node->right, bit_len + 1, bits*2 + 1);
	}
	else
	{
		qint32 idx = ((qint32)node->val) & 255;
		tbl_enc[idx].bit_len = bit_len;
		tbl_enc[idx].bits = bits;
		//qDebug() << idx << bit_len << QString::number(bits, 2);
	}

}

void Utilities::freeHufNode(hufnode* node)
{
	if(node == NULL)
		return;
	
	if(node->left != NULL)
		freeHufNode(node->left);
	if(node->right != NULL)
		freeHufNode(node->right);

	delete(node);
}

class huf_insert_glist
{
	public:

	const bool operator() (const hufnode* a, const hufnode* b) const
	{
			  
		if(a->occur < b->occur)
			return true;
		else if(a->occur > b->occur)
			return false;

		if(a->left == NULL && b->left == NULL)
			return true;
	
		if(a->left == NULL)
			return true;

		return false;
	}
};

QByteArray Utilities::encodeList(QByteArray inputData)
{
	using namespace std;
	using namespace Utilities;
	
	quint32 occur[256];

	qint32 i;
	multiset<hufnode*, huf_insert_glist>* pre_tree = new multiset<hufnode*, huf_insert_glist>;

	hufnode* root_huf;
	qint32 nb_val = 0;

	if(inputData.length() == 0)
		return QByteArray();

	// Initialise the table of character occurances
	memset(occur, 0, sizeof(occur));

	// Count the occurrences of each character value of the input data
	for(i = 0; i < inputData.length(); ++i)
	{
		occur[( ((qint32)inputData[i]) & 255 )]++;
	}
	
	// Make a huffman node for each character value, and insert the nodes into a list
	for(i = 0; i < 256; ++i)
	{
		if(occur[i] != 0)
		{
			
			hufnode* nw;
			nw = new hufnode();
			nw->occur = occur[i];
			nw->left = NULL;
			nw->right = NULL;
			nw->val = (uchar)i;
			
			//qDebug() << "Number" <<nb_val << "Value" << nw->val << "Count" <<nw->occur;

			pre_tree->insert(nw);

			nb_val++;
		}
	}

	// Build a tree of character values, those occuring least on the bottom,
	// and those occuring more often nearer the top of the tree.
	multiset<hufnode*, huf_insert_glist>::iterator it = pre_tree->begin();
	while(pre_tree->size() > 1)
	{
		hufnode* nw = new hufnode();

		nw->left = *it;
		pre_tree->erase(it);
		++it;
		
		nw->right = *it;
		pre_tree->erase(it);

		nw->occur = nw->left->occur + nw->right->occur;
		nw->val = 0;

		pre_tree->insert(nw);
		it = pre_tree->begin();
	}

	root_huf = *it;

	std::cout << root_huf->occur << std::endl;
	pre_tree->erase(it);

	hufencode tbl_enc[256];

	memset(tbl_enc, 0, sizeof(tbl_enc));

	// useHufNode is a recursive function that calculates the bit pattern
	// associated with each character value
	// It stores this information in tbl_enc
	useHufNode(tbl_enc, root_huf, 0, 0);

	QByteArray data;

	// Generate the header
	{
		uchar he3_header[] = {'H', 'E', '3', 0xD,
				  						0, // parity
										0,0,0,0, // num of bytes to produce
										0,0}; // number of couples

		uchar parity = 0;

		// Calculate parity
		for(i = 0; i < inputData.length(); ++i)
		{
			parity ^= inputData[i];
		}

		// Parity
		he3_header[4] = (uchar)(parity & 255);
		
		// Size of the input data
		he3_header[5] = (uchar)(inputData.length() & 255);
		he3_header[6] = (uchar)((inputData.length() >> 8) & 255);
		he3_header[7] = (uchar)((inputData.length() >> 16) & 255);
		he3_header[8] = (uchar)((inputData.length() >> 24) & 255);
		
		// Number of couples
		he3_header[9] = (uchar)(nb_val & 255);
		he3_header[10] = (uchar)((nb_val >> 8) & 255);
		//qDebug() << "Num couples" << nb_val << QString::number(nb_val, 16);
		
		// Insert this header into the output data array
		for(i = 0; i < 11; ++i)
		{
			data.append(he3_header[i]);
		}
		
	}

	// Output the occur table to the file
	for(i = 0; i < 256; ++i)
	{
		if(occur[i] != 0)
		{
			uchar ent[2];

			ent[0] = (uchar)i;
			ent[1] = (uchar)tbl_enc[i].bit_len;

			data.append(ent[0]); // Value
			data.append(ent[1]); // Bit length
		}
	}
	
	// Create the bitstream containing the bit codes for the occur table
	BitArray bitArray;
	for(i = 0; i < 256; ++i)
	{
		if(occur[i] == 0)
			continue;
		//qDebug() << "Appending no" << i;
		bitArray.append(tbl_enc[i].bits, tbl_enc[i].bit_len);
	}
	data.append(bitArray.toByteArray());
	
	// Create the bitstream containing the input data
	bitArray.clear();
	for(i = 0; i < inputData.length(); ++i)
	{
		quint8 byteValue = inputData[i];
		//qDebug() << "Appending byte" << byteValue << QString::number(tbl_enc[byteValue].bits,2) << "(" << tbl_enc[byteValue].bit_len << ")";
		bitArray.append(tbl_enc[byteValue].bits, tbl_enc[byteValue].bit_len);
	}
	data.append(bitArray.toByteArray());
	
	
	freeHufNode(root_huf);

	delete(pre_tree);

	return data;
}


namespace niceSizeDetails {

// we make single digits to have a little more precision
QString numerify(quint64 size, quint64 quotient)
{
	quint64 scaled = size / quotient;
	if(scaled >= 10)
		return QString::number(scaled);
	else
	{
		double scaled2 = double(size) / double(quotient);
		return QString::number(scaled2, 'f', 1);
	}
}
}

QString Utilities::niceSize(quint64 size)
{
	using namespace niceSizeDetails;
	if (size == 0)
		return "(no files)";
	else if (size < 1024)
		return QString::number(size) + " bytes";
	else if (size < 1024*1024)
		return numerify(size, 1024) + " KiB";
	else if (size < 1024*1024*1024)
		return numerify(size, 1024*1024) + " MiB";
	
	return numerify(size, 1024*1024*1024) + " GiB";
}

BitArray::BitArray()
{
	clear();
}

void BitArray::clear()
{
	data.clear();
	lastByte = 0;
	lastByteLength = 0;
}

void BitArray::append(quint32 bits, quint32 length)
{
	//qDebug() << "Adding" << QString::number(bits, 2) << "(" << length << ")";
	
	// Reverse the order of the bits
	quint32 modBits = 0;
	quint32 temp = bits;
	for (quint32 i=0 ; i<length ; ++i)
	{
		modBits |= ((temp & 1) << (length-1-i));
		temp >>= 1;
	}
	
	// Shift the bits along a bit, so we can shove them on
	// the MSB end of lastByte
	modBits <<= lastByteLength;
	
	// Shove the bits on the MSB end of lastByte
	lastByte |= modBits;
	lastByteLength += length;
	
	//qDebug() << "Last byte:" << QString::number(lastByte, 2) << "(" << lastByteLength << ")";
	
	// If we've managed to fill a byte, move it into the bytearray
	while (lastByteLength >= 8)
	{
		// Get the byte
		quint8 byteToAppend = lastByte & 0xFF;
		
		// Remove those 8 bits from lastByte
		lastByte >>= 8;
		lastByteLength -= 8;
		
		//qDebug() << "Appending" << QString::number(byteToAppend, 2);
		//qDebug() << "Last byte:" << QString::number(lastByte, 2) << "(" << lastByteLength << ")";
		
		// Append the byte
		data.append(byteToAppend);
	}
}

QByteArray BitArray::toByteArray()
{
	QByteArray ret = data;
	
	// We can't just return data, as there might still be some bits
	// that havn't been moved to the bytearray
	
	if (lastByteLength > 0)
	{
		// Make a mask to get rid of the bits of lastByte we don't want
		quint8 mask = 0xFF;
		mask >>= (8-lastByteLength);
		
		// Apply the mask and add the byte to ret
		quint8 maskedByte = lastByte & mask;
		ret.append(maskedByte);
	}
	
	return ret;
}

QByteArray Utilities::decodeBZList(QByteArray inputData)
{
	char* source = inputData.data();
	unsigned int sourceLen = inputData.size();

	char* dest = NULL;
	unsigned int destLen = sourceLen;

	while(true)
	{

	destLen = destLen * 2;
	dest = (char*)realloc(dest, sizeof(char) * destLen);

	int rc = BZ2_bzBuffToBuffDecompress(dest, &destLen, source, sourceLen, 0, 0);

	QByteArray* list;
	
	switch(rc)
	{
			case BZ_OK:
			list = new QByteArray(dest, destLen);
			free(dest);
			return *list;

			case BZ_OUTBUFF_FULL:
			qDebug() << "BZ2: Output buffer too small";
			break;
		
			case BZ_UNEXPECTED_EOF:
			qWarning() << "BZ2: EOF on compressed list";
			free(dest);
			return NULL;

			case BZ_MEM_ERROR:
			qWarning() << "BZ2: Insufficient Memory";
			free(dest);
			return NULL;

			case BZ_DATA_ERROR:
			qWarning() << "BZ2: Data integrity error in compressed list";
			free(dest);
			return NULL;

			case BZ_PARAM_ERROR:
			qWarning() << "BZ2: Parameters error";
			free(dest);
			return NULL;

			case BZ_DATA_ERROR_MAGIC:
			qWarning() << "BZ2: No magic here";
			free(dest);
			return NULL;

			case BZ_CONFIG_ERROR:
			qWarning() << "BZ2: Library miscompiled";
			free(dest);
			return NULL;

			default:
			qWarning() << "BZ2: Unknown error";
			free(dest);
			return NULL;
		}
	
	}
}

QByteArray Utilities::encodeBZList(QByteArray inputData)
{
	char* source = inputData.data();
	unsigned int sourceLen = inputData.size();

	unsigned int destLen = ((double)sourceLen * 1.01) + 600;
	char* dest = (char*)malloc(sizeof(char) * destLen);

	int rc = BZ2_bzBuffToBuffCompress(dest, &destLen, source, sourceLen, 4, 0, 30);

	QByteArray* list;
	
	switch(rc)
	{
		case BZ_CONFIG_ERROR:
		qWarning() << "BZ2: Library miscompiled";
		free(dest);
		return NULL;

		case BZ_PARAM_ERROR:
		qWarning() << "BZ2: parameters error";
		free(dest);
		return NULL;

		case BZ_MEM_ERROR:
		qWarning() << "BZ2: Insufficient memory";
		free(dest);
		return NULL;

		case BZ_OUTBUFF_FULL:
		qWarning() << "BZ2: Output buffer full - Shouldn't happen";
		free(dest);
		return NULL;

		case BZ_OK:
		list = new QByteArray(dest, destLen);
		free(dest);
		return *list;
	}
}

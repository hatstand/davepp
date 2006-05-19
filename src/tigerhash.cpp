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
// which was yoinked from ref implementation

#include "tigerhash.h"
#include <algorithm>
using namespace std;

#define PASSES 3

#define t1 (table)
#define t2 (table+256)
#define t3 (table+256*2)
#define t4 (table+256*3)

#define save_abc \
	aa = a; \
	bb = b; \
	cc = c;

#define round(a,b,c,x,mul) \
	c ^= x; \
	a -= t1[(quint8)(c)] ^ \
	t2[(quint8)(((quint32)(c))>>(2*8))] ^ \
	t3[(quint8)((c)>>(4*8))] ^ \
	t4[(quint8)(((quint32)((c)>>(4*8)))>>(2*8))] ; \
	b += t4[(quint8)(((quint32)(c))>>(1*8))] ^ \
	t3[(quint8)(((quint32)(c))>>(3*8))] ^ \
	t2[(quint8)(((quint32)((c)>>(4*8)))>>(1*8))] ^ \
	t1[(quint8)(((quint32)((c)>>(4*8)))>>(3*8))]; \
	b *= mul;

#define pass(a,b,c,mul) \
	round(a,b,c,x0,mul) \
	round(b,c,a,x1,mul) \
	round(c,a,b,x2,mul) \
	round(a,b,c,x3,mul) \
	round(b,c,a,x4,mul) \
	round(c,a,b,x5,mul) \
	round(a,b,c,x6,mul) \
	round(b,c,a,x7,mul)

#define key_schedule \
	x0 -= x7 ^ quint64(0xA5A5A5A5A5A5A5A5ull); \
	x1 ^= x0; \
	x2 += x1; \
	x3 -= x2 ^ ((~x1)<<19); \
	x4 ^= x3; \
	x5 += x4; \
	x6 -= x5 ^ ((~x4)>>23); \
	x7 ^= x6; \
	x0 += x7; \
	x1 -= x0 ^ ((~x7)<<19); \
	x2 ^= x1; \
	x3 += x2; \
	x4 -= x3 ^ ((~x2)>>23); \
	x5 ^= x4; \
	x6 += x5; \
	x7 -= x6 ^ quint64(0x0123456789ABCDEFull);

#define feedforward \
	a ^= aa; \
	b -= bb; \
	c += cc;

#define compress \
	save_abc \
	for(pass_no=0; pass_no<PASSES; pass_no++) { \
	if(pass_no != 0) {key_schedule} \
	pass(a,b,c,(pass_no==0?5:pass_no==1?7:9)); \
	tmpa=a; a=c; c=b; b=tmpa;} \
	feedforward

#define tiger_compress_macro(str, state) \
{ \
	register quint64 a, b, c, tmpa; \
	quint64 aa, bb, cc; \
	register quint64 x0, x1, x2, x3, x4, x5, x6, x7; \
	int pass_no; \
	\
	a = state[0]; \
	b = state[1]; \
	c = state[2]; \
	\
	x0=str[0]; x1=str[1]; x2=str[2]; x3=str[3]; \
	x4=str[4]; x5=str[5]; x6=str[6]; x7=str[7]; \
	\
	compress; \
	\
	state[0] = a; \
	state[1] = b; \
	state[2] = c; \
}

TigerHash::TigerHash(QByteArray chunk) :pos(0)
{
	res[0]=quint64(0x0123456789ABCDEFull);
	res[1]=quint64(0xFEDCBA9876543210ull);
	res[2]=quint64(0xF096A5B4C3B2E187ull);

	update((void*)chunk.data(), chunk.length());
	finalize();
}

QByteArray TigerHash::toByteArray()
{
	char bob[48];

	sprintf(bob, "%08X%08X%08X%08X%08X%08X",
		(quint32)(res[0] >> 32),
		(quint32)res[0],
		(quint32)(res[1] >> 32),
		(quint32)res[1],
		(quint32)(res[2] >> 32),
		(quint32)res[2]);

	return QByteArray(bob);
}


/* The compress function is a function. Requires smaller cache?    */
void TigerHash::tigerCompress(const quint64 *str, quint64 state[3]) {
	tiger_compress_macro(((const quint64*)str), ((quint64*)state));
}

void TigerHash::update(const void* data, quint32 length) {
	quint32 tmppos = (quint32)(pos & BLOCK_SIZE-1);
	const quint8* str = (const quint8*)data;
	// First empty tmp buffer if possible
	if(tmppos > 0) {
		quint32 n = min(length, BLOCK_SIZE-tmppos);
		memcpy(tmp + tmppos, str, n);
		str += n;
		pos += n;
		length -= n;

		if((tmppos + n) == BLOCK_SIZE) {
			tigerCompress((quint64*)tmp, res);
			tmppos = 0;
		}
	}

	// So, now either tmp is empty or all data has been consumed...
	Q_ASSERT(length == 0 || tmppos == 0);

	// Process the bulk of data
	while(length>=BLOCK_SIZE) {
		tigerCompress((quint64*)str, res);
		str += BLOCK_SIZE;
		pos += BLOCK_SIZE;
		length -= BLOCK_SIZE;
	}

	// Copy the rest to the tmp buffer
	memcpy(tmp, str, length);
	pos += length;
}

quint64* TigerHash::finalize() {
	quint32 tmppos = (quint32)(pos & BLOCK_SIZE-1);
	// Tmp buffer always has at least one pos, otherwise it would have
	// been processed in update()

	tmp[tmppos++] = 0x01;

	if(tmppos > (BLOCK_SIZE - sizeof(quint64))) 
	{
		memset(tmp + tmppos, 0, BLOCK_SIZE - tmppos);
		tigerCompress(((quint64*)tmp), res);
		memset(tmp, 0, BLOCK_SIZE);
	}
	else 
	{
		memset(tmp + tmppos, 0, BLOCK_SIZE - tmppos - sizeof(quint64));
	}

	((quint64*)(&(tmp[56])))[0] = pos<<3;
	tigerCompress((quint64*)tmp, res);
	return getResult();
}

quint64 TigerHash::table[4*256] = {
		quint64(0x02AAB17CF7E90C5Eull)   /*    0 */,    quint64(0xAC424B03E243A8ECull)   /*    1 */,
		quint64(0x72CD5BE30DD5FCD3ull)   /*    2 */,    quint64(0x6D019B93F6F97F3Aull)   /*    3 */,
		quint64(0xCD9978FFD21F9193ull)   /*    4 */,    quint64(0x7573A1C9708029E2ull)   /*    5 */,
		quint64(0xB164326B922A83C3ull)   /*    6 */,    quint64(0x46883EEE04915870ull)   /*    7 */,
		quint64(0xEAACE3057103ECE6ull)   /*    8 */,    quint64(0xC54169B808A3535Cull)   /*    9 */,
		quint64(0x4CE754918DDEC47Cull)   /*   10 */,    quint64(0x0AA2F4DFDC0DF40Cull)   /*   11 */,
		quint64(0x10B76F18A74DBEFAull)   /*   12 */,    quint64(0xC6CCB6235AD1AB6Aull)   /*   13 */,
		quint64(0x13726121572FE2FFull)   /*   14 */,    quint64(0x1A488C6F199D921Eull)   /*   15 */,
		quint64(0x4BC9F9F4DA0007CAull)   /*   16 */,    quint64(0x26F5E6F6E85241C7ull)   /*   17 */,
		quint64(0x859079DBEA5947B6ull)   /*   18 */,    quint64(0x4F1885C5C99E8C92ull)   /*   19 */,
		quint64(0xD78E761EA96F864Bull)   /*   20 */,    quint64(0x8E36428C52B5C17Dull)   /*   21 */,
		quint64(0x69CF6827373063C1ull)   /*   22 */,    quint64(0xB607C93D9BB4C56Eull)   /*   23 */,
		quint64(0x7D820E760E76B5EAull)   /*   24 */,    quint64(0x645C9CC6F07FDC42ull)   /*   25 */,
		quint64(0xBF38A078243342E0ull)   /*   26 */,    quint64(0x5F6B343C9D2E7D04ull)   /*   27 */,
		quint64(0xF2C28AEB600B0EC6ull)   /*   28 */,    quint64(0x6C0ED85F7254BCACull)   /*   29 */,
		quint64(0x71592281A4DB4FE5ull)   /*   30 */,    quint64(0x1967FA69CE0FED9Full)   /*   31 */,
		quint64(0xFD5293F8B96545DBull)   /*   32 */,    quint64(0xC879E9D7F2A7600Bull)   /*   33 */,
		quint64(0x860248920193194Eull)   /*   34 */,    quint64(0xA4F9533B2D9CC0B3ull)   /*   35 */,
		quint64(0x9053836C15957613ull)   /*   36 */,    quint64(0xDB6DCF8AFC357BF1ull)   /*   37 */,
		quint64(0x18BEEA7A7A370F57ull)   /*   38 */,    quint64(0x037117CA50B99066ull)   /*   39 */,
		quint64(0x6AB30A9774424A35ull)   /*   40 */,    quint64(0xF4E92F02E325249Bull)   /*   41 */,
		quint64(0x7739DB07061CCAE1ull)   /*   42 */,    quint64(0xD8F3B49CECA42A05ull)   /*   43 */,
		quint64(0xBD56BE3F51382F73ull)   /*   44 */,    quint64(0x45FAED5843B0BB28ull)   /*   45 */,
		quint64(0x1C813D5C11BF1F83ull)   /*   46 */,    quint64(0x8AF0E4B6D75FA169ull)   /*   47 */,
		quint64(0x33EE18A487AD9999ull)   /*   48 */,    quint64(0x3C26E8EAB1C94410ull)   /*   49 */,
		quint64(0xB510102BC0A822F9ull)   /*   50 */,    quint64(0x141EEF310CE6123Bull)   /*   51 */,
		quint64(0xFC65B90059DDB154ull)   /*   52 */,    quint64(0xE0158640C5E0E607ull)   /*   53 */,
		quint64(0x884E079826C3A3CFull)   /*   54 */,    quint64(0x930D0D9523C535FDull)   /*   55 */,
		quint64(0x35638D754E9A2B00ull)   /*   56 */,    quint64(0x4085FCCF40469DD5ull)   /*   57 */,
		quint64(0xC4B17AD28BE23A4Cull)   /*   58 */,    quint64(0xCAB2F0FC6A3E6A2Eull)   /*   59 */,
		quint64(0x2860971A6B943FCDull)   /*   60 */,    quint64(0x3DDE6EE212E30446ull)   /*   61 */,
		quint64(0x6222F32AE01765AEull)   /*   62 */,    quint64(0x5D550BB5478308FEull)   /*   63 */,
		quint64(0xA9EFA98DA0EDA22Aull)   /*   64 */,    quint64(0xC351A71686C40DA7ull)   /*   65 */,
		quint64(0x1105586D9C867C84ull)   /*   66 */,    quint64(0xDCFFEE85FDA22853ull)   /*   67 */,
		quint64(0xCCFBD0262C5EEF76ull)   /*   68 */,    quint64(0xBAF294CB8990D201ull)   /*   69 */,
		quint64(0xE69464F52AFAD975ull)   /*   70 */,    quint64(0x94B013AFDF133E14ull)   /*   71 */,
		quint64(0x06A7D1A32823C958ull)   /*   72 */,    quint64(0x6F95FE5130F61119ull)   /*   73 */,
		quint64(0xD92AB34E462C06C0ull)   /*   74 */,    quint64(0xED7BDE33887C71D2ull)   /*   75 */,
		quint64(0x79746D6E6518393Eull)   /*   76 */,    quint64(0x5BA419385D713329ull)   /*   77 */,
		quint64(0x7C1BA6B948A97564ull)   /*   78 */,    quint64(0x31987C197BFDAC67ull)   /*   79 */,
		quint64(0xDE6C23C44B053D02ull)   /*   80 */,    quint64(0x581C49FED002D64Dull)   /*   81 */,
		quint64(0xDD474D6338261571ull)   /*   82 */,    quint64(0xAA4546C3E473D062ull)   /*   83 */,
		quint64(0x928FCE349455F860ull)   /*   84 */,    quint64(0x48161BBACAAB94D9ull)   /*   85 */,
		quint64(0x63912430770E6F68ull)   /*   86 */,    quint64(0x6EC8A5E602C6641Cull)   /*   87 */,
		quint64(0x87282515337DDD2Bull)   /*   88 */,    quint64(0x2CDA6B42034B701Bull)   /*   89 */,
		quint64(0xB03D37C181CB096Dull)   /*   90 */,    quint64(0xE108438266C71C6Full)   /*   91 */,
		quint64(0x2B3180C7EB51B255ull)   /*   92 */,    quint64(0xDF92B82F96C08BBCull)   /*   93 */,
		quint64(0x5C68C8C0A632F3BAull)   /*   94 */,    quint64(0x5504CC861C3D0556ull)   /*   95 */,
		quint64(0xABBFA4E55FB26B8Full)   /*   96 */,    quint64(0x41848B0AB3BACEB4ull)   /*   97 */,
		quint64(0xB334A273AA445D32ull)   /*   98 */,    quint64(0xBCA696F0A85AD881ull)   /*   99 */,
		quint64(0x24F6EC65B528D56Cull)   /*  100 */,    quint64(0x0CE1512E90F4524Aull)   /*  101 */,
		quint64(0x4E9DD79D5506D35Aull)   /*  102 */,    quint64(0x258905FAC6CE9779ull)   /*  103 */,
		quint64(0x2019295B3E109B33ull)   /*  104 */,    quint64(0xF8A9478B73A054CCull)   /*  105 */,
		quint64(0x2924F2F934417EB0ull)   /*  106 */,    quint64(0x3993357D536D1BC4ull)   /*  107 */,
		quint64(0x38A81AC21DB6FF8Bull)   /*  108 */,    quint64(0x47C4FBF17D6016BFull)   /*  109 */,
		quint64(0x1E0FAADD7667E3F5ull)   /*  110 */,    quint64(0x7ABCFF62938BEB96ull)   /*  111 */,
		quint64(0xA78DAD948FC179C9ull)   /*  112 */,    quint64(0x8F1F98B72911E50Dull)   /*  113 */,
		quint64(0x61E48EAE27121A91ull)   /*  114 */,    quint64(0x4D62F7AD31859808ull)   /*  115 */,
		quint64(0xECEBA345EF5CEAEBull)   /*  116 */,    quint64(0xF5CEB25EBC9684CEull)   /*  117 */,
		quint64(0xF633E20CB7F76221ull)   /*  118 */,    quint64(0xA32CDF06AB8293E4ull)   /*  119 */,
		quint64(0x985A202CA5EE2CA4ull)   /*  120 */,    quint64(0xCF0B8447CC8A8FB1ull)   /*  121 */,
		quint64(0x9F765244979859A3ull)   /*  122 */,    quint64(0xA8D516B1A1240017ull)   /*  123 */,
		quint64(0x0BD7BA3EBB5DC726ull)   /*  124 */,    quint64(0xE54BCA55B86ADB39ull)   /*  125 */,
		quint64(0x1D7A3AFD6C478063ull)   /*  126 */,    quint64(0x519EC608E7669EDDull)   /*  127 */,
		quint64(0x0E5715A2D149AA23ull)   /*  128 */,    quint64(0x177D4571848FF194ull)   /*  129 */,
		quint64(0xEEB55F3241014C22ull)   /*  130 */,    quint64(0x0F5E5CA13A6E2EC2ull)   /*  131 */,
		quint64(0x8029927B75F5C361ull)   /*  132 */,    quint64(0xAD139FABC3D6E436ull)   /*  133 */,
		quint64(0x0D5DF1A94CCF402Full)   /*  134 */,    quint64(0x3E8BD948BEA5DFC8ull)   /*  135 */,
		quint64(0xA5A0D357BD3FF77Eull)   /*  136 */,    quint64(0xA2D12E251F74F645ull)   /*  137 */,
		quint64(0x66FD9E525E81A082ull)   /*  138 */,    quint64(0x2E0C90CE7F687A49ull)   /*  139 */,
		quint64(0xC2E8BCBEBA973BC5ull)   /*  140 */,    quint64(0x000001BCE509745Full)   /*  141 */,
		quint64(0x423777BBE6DAB3D6ull)   /*  142 */,    quint64(0xD1661C7EAEF06EB5ull)   /*  143 */,
		quint64(0xA1781F354DAACFD8ull)   /*  144 */,    quint64(0x2D11284A2B16AFFCull)   /*  145 */,
		quint64(0xF1FC4F67FA891D1Full)   /*  146 */,    quint64(0x73ECC25DCB920ADAull)   /*  147 */,
		quint64(0xAE610C22C2A12651ull)   /*  148 */,    quint64(0x96E0A810D356B78Aull)   /*  149 */,
		quint64(0x5A9A381F2FE7870Full)   /*  150 */,    quint64(0xD5AD62EDE94E5530ull)   /*  151 */,
		quint64(0xD225E5E8368D1427ull)   /*  152 */,    quint64(0x65977B70C7AF4631ull)   /*  153 */,
		quint64(0x99F889B2DE39D74Full)   /*  154 */,    quint64(0x233F30BF54E1D143ull)   /*  155 */,
		quint64(0x9A9675D3D9A63C97ull)   /*  156 */,    quint64(0x5470554FF334F9A8ull)   /*  157 */,
		quint64(0x166ACB744A4F5688ull)   /*  158 */,    quint64(0x70C74CAAB2E4AEADull)   /*  159 */,
		quint64(0xF0D091646F294D12ull)   /*  160 */,    quint64(0x57B82A89684031D1ull)   /*  161 */,
		quint64(0xEFD95A5A61BE0B6Bull)   /*  162 */,    quint64(0x2FBD12E969F2F29Aull)   /*  163 */,
		quint64(0x9BD37013FEFF9FE8ull)   /*  164 */,    quint64(0x3F9B0404D6085A06ull)   /*  165 */,
		quint64(0x4940C1F3166CFE15ull)   /*  166 */,    quint64(0x09542C4DCDF3DEFBull)   /*  167 */,
		quint64(0xB4C5218385CD5CE3ull)   /*  168 */,    quint64(0xC935B7DC4462A641ull)   /*  169 */,
		quint64(0x3417F8A68ED3B63Full)   /*  170 */,    quint64(0xB80959295B215B40ull)   /*  171 */,
		quint64(0xF99CDAEF3B8C8572ull)   /*  172 */,    quint64(0x018C0614F8FCB95Dull)   /*  173 */,
		quint64(0x1B14ACCD1A3ACDF3ull)   /*  174 */,    quint64(0x84D471F200BB732Dull)   /*  175 */,
		quint64(0xC1A3110E95E8DA16ull)   /*  176 */,    quint64(0x430A7220BF1A82B8ull)   /*  177 */,
		quint64(0xB77E090D39DF210Eull)   /*  178 */,    quint64(0x5EF4BD9F3CD05E9Dull)   /*  179 */,
		quint64(0x9D4FF6DA7E57A444ull)   /*  180 */,    quint64(0xDA1D60E183D4A5F8ull)   /*  181 */,
		quint64(0xB287C38417998E47ull)   /*  182 */,    quint64(0xFE3EDC121BB31886ull)   /*  183 */,
		quint64(0xC7FE3CCC980CCBEFull)   /*  184 */,    quint64(0xE46FB590189BFD03ull)   /*  185 */,
		quint64(0x3732FD469A4C57DCull)   /*  186 */,    quint64(0x7EF700A07CF1AD65ull)   /*  187 */,
		quint64(0x59C64468A31D8859ull)   /*  188 */,    quint64(0x762FB0B4D45B61F6ull)   /*  189 */,
		quint64(0x155BAED099047718ull)   /*  190 */,    quint64(0x68755E4C3D50BAA6ull)   /*  191 */,
		quint64(0xE9214E7F22D8B4DFull)   /*  192 */,    quint64(0x2ADDBF532EAC95F4ull)   /*  193 */,
		quint64(0x32AE3909B4BD0109ull)   /*  194 */,    quint64(0x834DF537B08E3450ull)   /*  195 */,
		quint64(0xFA209DA84220728Dull)   /*  196 */,    quint64(0x9E691D9B9EFE23F7ull)   /*  197 */,
		quint64(0x0446D288C4AE8D7Full)   /*  198 */,    quint64(0x7B4CC524E169785Bull)   /*  199 */,
		quint64(0x21D87F0135CA1385ull)   /*  200 */,    quint64(0xCEBB400F137B8AA5ull)   /*  201 */,
		quint64(0x272E2B66580796BEull)   /*  202 */,    quint64(0x3612264125C2B0DEull)   /*  203 */,
		quint64(0x057702BDAD1EFBB2ull)   /*  204 */,    quint64(0xD4BABB8EACF84BE9ull)   /*  205 */,
		quint64(0x91583139641BC67Bull)   /*  206 */,    quint64(0x8BDC2DE08036E024ull)   /*  207 */,
		quint64(0x603C8156F49F68EDull)   /*  208 */,    quint64(0xF7D236F7DBEF5111ull)   /*  209 */,
		quint64(0x9727C4598AD21E80ull)   /*  210 */,    quint64(0xA08A0896670A5FD7ull)   /*  211 */,
		quint64(0xCB4A8F4309EBA9CBull)   /*  212 */,    quint64(0x81AF564B0F7036A1ull)   /*  213 */,
		quint64(0xC0B99AA778199ABDull)   /*  214 */,    quint64(0x959F1EC83FC8E952ull)   /*  215 */,
		quint64(0x8C505077794A81B9ull)   /*  216 */,    quint64(0x3ACAAF8F056338F0ull)   /*  217 */,
		quint64(0x07B43F50627A6778ull)   /*  218 */,    quint64(0x4A44AB49F5ECCC77ull)   /*  219 */,
		quint64(0x3BC3D6E4B679EE98ull)   /*  220 */,    quint64(0x9CC0D4D1CF14108Cull)   /*  221 */,
		quint64(0x4406C00B206BC8A0ull)   /*  222 */,    quint64(0x82A18854C8D72D89ull)   /*  223 */,
		quint64(0x67E366B35C3C432Cull)   /*  224 */,    quint64(0xB923DD61102B37F2ull)   /*  225 */,
		quint64(0x56AB2779D884271Dull)   /*  226 */,    quint64(0xBE83E1B0FF1525AFull)   /*  227 */,
		quint64(0xFB7C65D4217E49A9ull)   /*  228 */,    quint64(0x6BDBE0E76D48E7D4ull)   /*  229 */,
		quint64(0x08DF828745D9179Eull)   /*  230 */,    quint64(0x22EA6A9ADD53BD34ull)   /*  231 */,
		quint64(0xE36E141C5622200Aull)   /*  232 */,    quint64(0x7F805D1B8CB750EEull)   /*  233 */,
		quint64(0xAFE5C7A59F58E837ull)   /*  234 */,    quint64(0xE27F996A4FB1C23Cull)   /*  235 */,
		quint64(0xD3867DFB0775F0D0ull)   /*  236 */,    quint64(0xD0E673DE6E88891Aull)   /*  237 */,
		quint64(0x123AEB9EAFB86C25ull)   /*  238 */,    quint64(0x30F1D5D5C145B895ull)   /*  239 */,
		quint64(0xBB434A2DEE7269E7ull)   /*  240 */,    quint64(0x78CB67ECF931FA38ull)   /*  241 */,
		quint64(0xF33B0372323BBF9Cull)   /*  242 */,    quint64(0x52D66336FB279C74ull)   /*  243 */,
		quint64(0x505F33AC0AFB4EAAull)   /*  244 */,    quint64(0xE8A5CD99A2CCE187ull)   /*  245 */,
		quint64(0x534974801E2D30BBull)   /*  246 */,    quint64(0x8D2D5711D5876D90ull)   /*  247 */,
		quint64(0x1F1A412891BC038Eull)   /*  248 */,    quint64(0xD6E2E71D82E56648ull)   /*  249 */,
		quint64(0x74036C3A497732B7ull)   /*  250 */,    quint64(0x89B67ED96361F5ABull)   /*  251 */,
		quint64(0xFFED95D8F1EA02A2ull)   /*  252 */,    quint64(0xE72B3BD61464D43Dull)   /*  253 */,
		quint64(0xA6300F170BDC4820ull)   /*  254 */,    quint64(0xEBC18760ED78A77Aull)   /*  255 */,
		quint64(0xE6A6BE5A05A12138ull)   /*  256 */,    quint64(0xB5A122A5B4F87C98ull)   /*  257 */,
		quint64(0x563C6089140B6990ull)   /*  258 */,    quint64(0x4C46CB2E391F5DD5ull)   /*  259 */,
		quint64(0xD932ADDBC9B79434ull)   /*  260 */,    quint64(0x08EA70E42015AFF5ull)   /*  261 */,
		quint64(0xD765A6673E478CF1ull)   /*  262 */,    quint64(0xC4FB757EAB278D99ull)   /*  263 */,
		quint64(0xDF11C6862D6E0692ull)   /*  264 */,    quint64(0xDDEB84F10D7F3B16ull)   /*  265 */,
		quint64(0x6F2EF604A665EA04ull)   /*  266 */,    quint64(0x4A8E0F0FF0E0DFB3ull)   /*  267 */,
		quint64(0xA5EDEEF83DBCBA51ull)   /*  268 */,    quint64(0xFC4F0A2A0EA4371Eull)   /*  269 */,
		quint64(0xE83E1DA85CB38429ull)   /*  270 */,    quint64(0xDC8FF882BA1B1CE2ull)   /*  271 */,
		quint64(0xCD45505E8353E80Dull)   /*  272 */,    quint64(0x18D19A00D4DB0717ull)   /*  273 */,
		quint64(0x34A0CFEDA5F38101ull)   /*  274 */,    quint64(0x0BE77E518887CAF2ull)   /*  275 */,
		quint64(0x1E341438B3C45136ull)   /*  276 */,    quint64(0xE05797F49089CCF9ull)   /*  277 */,
		quint64(0xFFD23F9DF2591D14ull)   /*  278 */,    quint64(0x543DDA228595C5CDull)   /*  279 */,
		quint64(0x661F81FD99052A33ull)   /*  280 */,    quint64(0x8736E641DB0F7B76ull)   /*  281 */,
		quint64(0x15227725418E5307ull)   /*  282 */,    quint64(0xE25F7F46162EB2FAull)   /*  283 */,
		quint64(0x48A8B2126C13D9FEull)   /*  284 */,    quint64(0xAFDC541792E76EEAull)   /*  285 */,
		quint64(0x03D912BFC6D1898Full)   /*  286 */,    quint64(0x31B1AAFA1B83F51Bull)   /*  287 */,
		quint64(0xF1AC2796E42AB7D9ull)   /*  288 */,    quint64(0x40A3A7D7FCD2EBACull)   /*  289 */,
		quint64(0x1056136D0AFBBCC5ull)   /*  290 */,    quint64(0x7889E1DD9A6D0C85ull)   /*  291 */,
		quint64(0xD33525782A7974AAull)   /*  292 */,    quint64(0xA7E25D09078AC09Bull)   /*  293 */,
		quint64(0xBD4138B3EAC6EDD0ull)   /*  294 */,    quint64(0x920ABFBE71EB9E70ull)   /*  295 */,
		quint64(0xA2A5D0F54FC2625Cull)   /*  296 */,    quint64(0xC054E36B0B1290A3ull)   /*  297 */,
		quint64(0xF6DD59FF62FE932Bull)   /*  298 */,    quint64(0x3537354511A8AC7Dull)   /*  299 */,
		quint64(0xCA845E9172FADCD4ull)   /*  300 */,    quint64(0x84F82B60329D20DCull)   /*  301 */,
		quint64(0x79C62CE1CD672F18ull)   /*  302 */,    quint64(0x8B09A2ADD124642Cull)   /*  303 */,
		quint64(0xD0C1E96A19D9E726ull)   /*  304 */,    quint64(0x5A786A9B4BA9500Cull)   /*  305 */,
		quint64(0x0E020336634C43F3ull)   /*  306 */,    quint64(0xC17B474AEB66D822ull)   /*  307 */,
		quint64(0x6A731AE3EC9BAAC2ull)   /*  308 */,    quint64(0x8226667AE0840258ull)   /*  309 */,
		quint64(0x67D4567691CAECA5ull)   /*  310 */,    quint64(0x1D94155C4875ADB5ull)   /*  311 */,
		quint64(0x6D00FD985B813FDFull)   /*  312 */,    quint64(0x51286EFCB774CD06ull)   /*  313 */,
		quint64(0x5E8834471FA744AFull)   /*  314 */,    quint64(0xF72CA0AEE761AE2Eull)   /*  315 */,
		quint64(0xBE40E4CDAEE8E09Aull)   /*  316 */,    quint64(0xE9970BBB5118F665ull)   /*  317 */,
		quint64(0x726E4BEB33DF1964ull)   /*  318 */,    quint64(0x703B000729199762ull)   /*  319 */,
		quint64(0x4631D816F5EF30A7ull)   /*  320 */,    quint64(0xB880B5B51504A6BEull)   /*  321 */,
		quint64(0x641793C37ED84B6Cull)   /*  322 */,    quint64(0x7B21ED77F6E97D96ull)   /*  323 */,
		quint64(0x776306312EF96B73ull)   /*  324 */,    quint64(0xAE528948E86FF3F4ull)   /*  325 */,
		quint64(0x53DBD7F286A3F8F8ull)   /*  326 */,    quint64(0x16CADCE74CFC1063ull)   /*  327 */,
		quint64(0x005C19BDFA52C6DDull)   /*  328 */,    quint64(0x68868F5D64D46AD3ull)   /*  329 */,
		quint64(0x3A9D512CCF1E186Aull)   /*  330 */,    quint64(0x367E62C2385660AEull)   /*  331 */,
		quint64(0xE359E7EA77DCB1D7ull)   /*  332 */,    quint64(0x526C0773749ABE6Eull)   /*  333 */,
		quint64(0x735AE5F9D09F734Bull)   /*  334 */,    quint64(0x493FC7CC8A558BA8ull)   /*  335 */,
		quint64(0xB0B9C1533041AB45ull)   /*  336 */,    quint64(0x321958BA470A59BDull)   /*  337 */,
		quint64(0x852DB00B5F46C393ull)   /*  338 */,    quint64(0x91209B2BD336B0E5ull)   /*  339 */,
		quint64(0x6E604F7D659EF19Full)   /*  340 */,    quint64(0xB99A8AE2782CCB24ull)   /*  341 */,
		quint64(0xCCF52AB6C814C4C7ull)   /*  342 */,    quint64(0x4727D9AFBE11727Bull)   /*  343 */,
		quint64(0x7E950D0C0121B34Dull)   /*  344 */,    quint64(0x756F435670AD471Full)   /*  345 */,
		quint64(0xF5ADD442615A6849ull)   /*  346 */,    quint64(0x4E87E09980B9957Aull)   /*  347 */,
		quint64(0x2ACFA1DF50AEE355ull)   /*  348 */,    quint64(0xD898263AFD2FD556ull)   /*  349 */,
		quint64(0xC8F4924DD80C8FD6ull)   /*  350 */,    quint64(0xCF99CA3D754A173Aull)   /*  351 */,
		quint64(0xFE477BACAF91BF3Cull)   /*  352 */,    quint64(0xED5371F6D690C12Dull)   /*  353 */,
		quint64(0x831A5C285E687094ull)   /*  354 */,    quint64(0xC5D3C90A3708A0A4ull)   /*  355 */,
		quint64(0x0F7F903717D06580ull)   /*  356 */,    quint64(0x19F9BB13B8FDF27Full)   /*  357 */,
		quint64(0xB1BD6F1B4D502843ull)   /*  358 */,    quint64(0x1C761BA38FFF4012ull)   /*  359 */,
		quint64(0x0D1530C4E2E21F3Bull)   /*  360 */,    quint64(0x8943CE69A7372C8Aull)   /*  361 */,
		quint64(0xE5184E11FEB5CE66ull)   /*  362 */,    quint64(0x618BDB80BD736621ull)   /*  363 */,
		quint64(0x7D29BAD68B574D0Bull)   /*  364 */,    quint64(0x81BB613E25E6FE5Bull)   /*  365 */,
		quint64(0x071C9C10BC07913Full)   /*  366 */,    quint64(0xC7BEEB7909AC2D97ull)   /*  367 */,
		quint64(0xC3E58D353BC5D757ull)   /*  368 */,    quint64(0xEB017892F38F61E8ull)   /*  369 */,
		quint64(0xD4EFFB9C9B1CC21Aull)   /*  370 */,    quint64(0x99727D26F494F7ABull)   /*  371 */,
		quint64(0xA3E063A2956B3E03ull)   /*  372 */,    quint64(0x9D4A8B9A4AA09C30ull)   /*  373 */,
		quint64(0x3F6AB7D500090FB4ull)   /*  374 */,    quint64(0x9CC0F2A057268AC0ull)   /*  375 */,
		quint64(0x3DEE9D2DEDBF42D1ull)   /*  376 */,    quint64(0x330F49C87960A972ull)   /*  377 */,
		quint64(0xC6B2720287421B41ull)   /*  378 */,    quint64(0x0AC59EC07C00369Cull)   /*  379 */,
		quint64(0xEF4EAC49CB353425ull)   /*  380 */,    quint64(0xF450244EEF0129D8ull)   /*  381 */,
		quint64(0x8ACC46E5CAF4DEB6ull)   /*  382 */,    quint64(0x2FFEAB63989263F7ull)   /*  383 */,
		quint64(0x8F7CB9FE5D7A4578ull)   /*  384 */,    quint64(0x5BD8F7644E634635ull)   /*  385 */,
		quint64(0x427A7315BF2DC900ull)   /*  386 */,    quint64(0x17D0C4AA2125261Cull)   /*  387 */,
		quint64(0x3992486C93518E50ull)   /*  388 */,    quint64(0xB4CBFEE0A2D7D4C3ull)   /*  389 */,
		quint64(0x7C75D6202C5DDD8Dull)   /*  390 */,    quint64(0xDBC295D8E35B6C61ull)   /*  391 */,
		quint64(0x60B369D302032B19ull)   /*  392 */,    quint64(0xCE42685FDCE44132ull)   /*  393 */,
		quint64(0x06F3DDB9DDF65610ull)   /*  394 */,    quint64(0x8EA4D21DB5E148F0ull)   /*  395 */,
		quint64(0x20B0FCE62FCD496Full)   /*  396 */,    quint64(0x2C1B912358B0EE31ull)   /*  397 */,
		quint64(0xB28317B818F5A308ull)   /*  398 */,    quint64(0xA89C1E189CA6D2CFull)   /*  399 */,
		quint64(0x0C6B18576AAADBC8ull)   /*  400 */,    quint64(0xB65DEAA91299FAE3ull)   /*  401 */,
		quint64(0xFB2B794B7F1027E7ull)   /*  402 */,    quint64(0x04E4317F443B5BEBull)   /*  403 */,
		quint64(0x4B852D325939D0A6ull)   /*  404 */,    quint64(0xD5AE6BEEFB207FFCull)   /*  405 */,
		quint64(0x309682B281C7D374ull)   /*  406 */,    quint64(0xBAE309A194C3B475ull)   /*  407 */,
		quint64(0x8CC3F97B13B49F05ull)   /*  408 */,    quint64(0x98A9422FF8293967ull)   /*  409 */,
		quint64(0x244B16B01076FF7Cull)   /*  410 */,    quint64(0xF8BF571C663D67EEull)   /*  411 */,
		quint64(0x1F0D6758EEE30DA1ull)   /*  412 */,    quint64(0xC9B611D97ADEB9B7ull)   /*  413 */,
		quint64(0xB7AFD5887B6C57A2ull)   /*  414 */,    quint64(0x6290AE846B984FE1ull)   /*  415 */,
		quint64(0x94DF4CDEACC1A5FDull)   /*  416 */,    quint64(0x058A5BD1C5483AFFull)   /*  417 */,
		quint64(0x63166CC142BA3C37ull)   /*  418 */,    quint64(0x8DB8526EB2F76F40ull)   /*  419 */,
		quint64(0xE10880036F0D6D4Eull)   /*  420 */,    quint64(0x9E0523C9971D311Dull)   /*  421 */,
		quint64(0x45EC2824CC7CD691ull)   /*  422 */,    quint64(0x575B8359E62382C9ull)   /*  423 */,
		quint64(0xFA9E400DC4889995ull)   /*  424 */,    quint64(0xD1823ECB45721568ull)   /*  425 */,
		quint64(0xDAFD983B8206082Full)   /*  426 */,    quint64(0xAA7D29082386A8CBull)   /*  427 */,
		quint64(0x269FCD4403B87588ull)   /*  428 */,    quint64(0x1B91F5F728BDD1E0ull)   /*  429 */,
		quint64(0xE4669F39040201F6ull)   /*  430 */,    quint64(0x7A1D7C218CF04ADEull)   /*  431 */,
		quint64(0x65623C29D79CE5CEull)   /*  432 */,    quint64(0x2368449096C00BB1ull)   /*  433 */,
		quint64(0xAB9BF1879DA503BAull)   /*  434 */,    quint64(0xBC23ECB1A458058Eull)   /*  435 */,
		quint64(0x9A58DF01BB401ECCull)   /*  436 */,    quint64(0xA070E868A85F143Dull)   /*  437 */,
		quint64(0x4FF188307DF2239Eull)   /*  438 */,    quint64(0x14D565B41A641183ull)   /*  439 */,
		quint64(0xEE13337452701602ull)   /*  440 */,    quint64(0x950E3DCF3F285E09ull)   /*  441 */,
		quint64(0x59930254B9C80953ull)   /*  442 */,    quint64(0x3BF299408930DA6Dull)   /*  443 */,
		quint64(0xA955943F53691387ull)   /*  444 */,    quint64(0xA15EDECAA9CB8784ull)   /*  445 */,
		quint64(0x29142127352BE9A0ull)   /*  446 */,    quint64(0x76F0371FFF4E7AFBull)   /*  447 */,
		quint64(0x0239F450274F2228ull)   /*  448 */,    quint64(0xBB073AF01D5E868Bull)   /*  449 */,
		quint64(0xBFC80571C10E96C1ull)   /*  450 */,    quint64(0xD267088568222E23ull)   /*  451 */,
		quint64(0x9671A3D48E80B5B0ull)   /*  452 */,    quint64(0x55B5D38AE193BB81ull)   /*  453 */,
		quint64(0x693AE2D0A18B04B8ull)   /*  454 */,    quint64(0x5C48B4ECADD5335Full)   /*  455 */,
		quint64(0xFD743B194916A1CAull)   /*  456 */,    quint64(0x2577018134BE98C4ull)   /*  457 */,
		quint64(0xE77987E83C54A4ADull)   /*  458 */,    quint64(0x28E11014DA33E1B9ull)   /*  459 */,
		quint64(0x270CC59E226AA213ull)   /*  460 */,    quint64(0x71495F756D1A5F60ull)   /*  461 */,
		quint64(0x9BE853FB60AFEF77ull)   /*  462 */,    quint64(0xADC786A7F7443DBFull)   /*  463 */,
		quint64(0x0904456173B29A82ull)   /*  464 */,    quint64(0x58BC7A66C232BD5Eull)   /*  465 */,
		quint64(0xF306558C673AC8B2ull)   /*  466 */,    quint64(0x41F639C6B6C9772Aull)   /*  467 */,
		quint64(0x216DEFE99FDA35DAull)   /*  468 */,    quint64(0x11640CC71C7BE615ull)   /*  469 */,
		quint64(0x93C43694565C5527ull)   /*  470 */,    quint64(0xEA038E6246777839ull)   /*  471 */,
		quint64(0xF9ABF3CE5A3E2469ull)   /*  472 */,    quint64(0x741E768D0FD312D2ull)   /*  473 */,
		quint64(0x0144B883CED652C6ull)   /*  474 */,    quint64(0xC20B5A5BA33F8552ull)   /*  475 */,
		quint64(0x1AE69633C3435A9Dull)   /*  476 */,    quint64(0x97A28CA4088CFDECull)   /*  477 */,
		quint64(0x8824A43C1E96F420ull)   /*  478 */,    quint64(0x37612FA66EEEA746ull)   /*  479 */,
		quint64(0x6B4CB165F9CF0E5Aull)   /*  480 */,    quint64(0x43AA1C06A0ABFB4Aull)   /*  481 */,
		quint64(0x7F4DC26FF162796Bull)   /*  482 */,    quint64(0x6CBACC8E54ED9B0Full)   /*  483 */,
		quint64(0xA6B7FFEFD2BB253Eull)   /*  484 */,    quint64(0x2E25BC95B0A29D4Full)   /*  485 */,
		quint64(0x86D6A58BDEF1388Cull)   /*  486 */,    quint64(0xDED74AC576B6F054ull)   /*  487 */,
		quint64(0x8030BDBC2B45805Dull)   /*  488 */,    quint64(0x3C81AF70E94D9289ull)   /*  489 */,
		quint64(0x3EFF6DDA9E3100DBull)   /*  490 */,    quint64(0xB38DC39FDFCC8847ull)   /*  491 */,
		quint64(0x123885528D17B87Eull)   /*  492 */,    quint64(0xF2DA0ED240B1B642ull)   /*  493 */,
		quint64(0x44CEFADCD54BF9A9ull)   /*  494 */,    quint64(0x1312200E433C7EE6ull)   /*  495 */,
		quint64(0x9FFCC84F3A78C748ull)   /*  496 */,    quint64(0xF0CD1F72248576BBull)   /*  497 */,
		quint64(0xEC6974053638CFE4ull)   /*  498 */,    quint64(0x2BA7B67C0CEC4E4Cull)   /*  499 */,
		quint64(0xAC2F4DF3E5CE32EDull)   /*  500 */,    quint64(0xCB33D14326EA4C11ull)   /*  501 */,
		quint64(0xA4E9044CC77E58BCull)   /*  502 */,    quint64(0x5F513293D934FCEFull)   /*  503 */,
		quint64(0x5DC9645506E55444ull)   /*  504 */,    quint64(0x50DE418F317DE40Aull)   /*  505 */,
		quint64(0x388CB31A69DDE259ull)   /*  506 */,    quint64(0x2DB4A83455820A86ull)   /*  507 */,
		quint64(0x9010A91E84711AE9ull)   /*  508 */,    quint64(0x4DF7F0B7B1498371ull)   /*  509 */,
		quint64(0xD62A2EABC0977179ull)   /*  510 */,    quint64(0x22FAC097AA8D5C0Eull)   /*  511 */,
		quint64(0xF49FCC2FF1DAF39Bull)   /*  512 */,    quint64(0x487FD5C66FF29281ull)   /*  513 */,
		quint64(0xE8A30667FCDCA83Full)   /*  514 */,    quint64(0x2C9B4BE3D2FCCE63ull)   /*  515 */,
		quint64(0xDA3FF74B93FBBBC2ull)   /*  516 */,    quint64(0x2FA165D2FE70BA66ull)   /*  517 */,
		quint64(0xA103E279970E93D4ull)   /*  518 */,    quint64(0xBECDEC77B0E45E71ull)   /*  519 */,
		quint64(0xCFB41E723985E497ull)   /*  520 */,    quint64(0xB70AAA025EF75017ull)   /*  521 */,
		quint64(0xD42309F03840B8E0ull)   /*  522 */,    quint64(0x8EFC1AD035898579ull)   /*  523 */,
		quint64(0x96C6920BE2B2ABC5ull)   /*  524 */,    quint64(0x66AF4163375A9172ull)   /*  525 */,
		quint64(0x2174ABDCCA7127FBull)   /*  526 */,    quint64(0xB33CCEA64A72FF41ull)   /*  527 */,
		quint64(0xF04A4933083066A5ull)   /*  528 */,    quint64(0x8D970ACDD7289AF5ull)   /*  529 */,
		quint64(0x8F96E8E031C8C25Eull)   /*  530 */,    quint64(0xF3FEC02276875D47ull)   /*  531 */,
		quint64(0xEC7BF310056190DDull)   /*  532 */,    quint64(0xF5ADB0AEBB0F1491ull)   /*  533 */,
		quint64(0x9B50F8850FD58892ull)   /*  534 */,    quint64(0x4975488358B74DE8ull)   /*  535 */,
		quint64(0xA3354FF691531C61ull)   /*  536 */,    quint64(0x0702BBE481D2C6EEull)   /*  537 */,
		quint64(0x89FB24057DEDED98ull)   /*  538 */,    quint64(0xAC3075138596E902ull)   /*  539 */,
		quint64(0x1D2D3580172772EDull)   /*  540 */,    quint64(0xEB738FC28E6BC30Dull)   /*  541 */,
		quint64(0x5854EF8F63044326ull)   /*  542 */,    quint64(0x9E5C52325ADD3BBEull)   /*  543 */,
		quint64(0x90AA53CF325C4623ull)   /*  544 */,    quint64(0xC1D24D51349DD067ull)   /*  545 */,
		quint64(0x2051CFEEA69EA624ull)   /*  546 */,    quint64(0x13220F0A862E7E4Full)   /*  547 */,
		quint64(0xCE39399404E04864ull)   /*  548 */,    quint64(0xD9C42CA47086FCB7ull)   /*  549 */,
		quint64(0x685AD2238A03E7CCull)   /*  550 */,    quint64(0x066484B2AB2FF1DBull)   /*  551 */,
		quint64(0xFE9D5D70EFBF79ECull)   /*  552 */,    quint64(0x5B13B9DD9C481854ull)   /*  553 */,
		quint64(0x15F0D475ED1509ADull)   /*  554 */,    quint64(0x0BEBCD060EC79851ull)   /*  555 */,
		quint64(0xD58C6791183AB7F8ull)   /*  556 */,    quint64(0xD1187C5052F3EEE4ull)   /*  557 */,
		quint64(0xC95D1192E54E82FFull)   /*  558 */,    quint64(0x86EEA14CB9AC6CA2ull)   /*  559 */,
		quint64(0x3485BEB153677D5Dull)   /*  560 */,    quint64(0xDD191D781F8C492Aull)   /*  561 */,
		quint64(0xF60866BAA784EBF9ull)   /*  562 */,    quint64(0x518F643BA2D08C74ull)   /*  563 */,
		quint64(0x8852E956E1087C22ull)   /*  564 */,    quint64(0xA768CB8DC410AE8Dull)   /*  565 */,
		quint64(0x38047726BFEC8E1Aull)   /*  566 */,    quint64(0xA67738B4CD3B45AAull)   /*  567 */,
		quint64(0xAD16691CEC0DDE19ull)   /*  568 */,    quint64(0xC6D4319380462E07ull)   /*  569 */,
		quint64(0xC5A5876D0BA61938ull)   /*  570 */,    quint64(0x16B9FA1FA58FD840ull)   /*  571 */,
		quint64(0x188AB1173CA74F18ull)   /*  572 */,    quint64(0xABDA2F98C99C021Full)   /*  573 */,
		quint64(0x3E0580AB134AE816ull)   /*  574 */,    quint64(0x5F3B05B773645ABBull)   /*  575 */,
		quint64(0x2501A2BE5575F2F6ull)   /*  576 */,    quint64(0x1B2F74004E7E8BA9ull)   /*  577 */,
		quint64(0x1CD7580371E8D953ull)   /*  578 */,    quint64(0x7F6ED89562764E30ull)   /*  579 */,
		quint64(0xB15926FF596F003Dull)   /*  580 */,    quint64(0x9F65293DA8C5D6B9ull)   /*  581 */,
		quint64(0x6ECEF04DD690F84Cull)   /*  582 */,    quint64(0x4782275FFF33AF88ull)   /*  583 */,
		quint64(0xE41433083F820801ull)   /*  584 */,    quint64(0xFD0DFE409A1AF9B5ull)   /*  585 */,
		quint64(0x4325A3342CDB396Bull)   /*  586 */,    quint64(0x8AE77E62B301B252ull)   /*  587 */,
		quint64(0xC36F9E9F6655615Aull)   /*  588 */,    quint64(0x85455A2D92D32C09ull)   /*  589 */,
		quint64(0xF2C7DEA949477485ull)   /*  590 */,    quint64(0x63CFB4C133A39EBAull)   /*  591 */,
		quint64(0x83B040CC6EBC5462ull)   /*  592 */,    quint64(0x3B9454C8FDB326B0ull)   /*  593 */,
		quint64(0x56F56A9E87FFD78Cull)   /*  594 */,    quint64(0x2DC2940D99F42BC6ull)   /*  595 */,
		quint64(0x98F7DF096B096E2Dull)   /*  596 */,    quint64(0x19A6E01E3AD852BFull)   /*  597 */,
		quint64(0x42A99CCBDBD4B40Bull)   /*  598 */,    quint64(0xA59998AF45E9C559ull)   /*  599 */,
		quint64(0x366295E807D93186ull)   /*  600 */,    quint64(0x6B48181BFAA1F773ull)   /*  601 */,
		quint64(0x1FEC57E2157A0A1Dull)   /*  602 */,    quint64(0x4667446AF6201AD5ull)   /*  603 */,
		quint64(0xE615EBCACFB0F075ull)   /*  604 */,    quint64(0xB8F31F4F68290778ull)   /*  605 */,
		quint64(0x22713ED6CE22D11Eull)   /*  606 */,    quint64(0x3057C1A72EC3C93Bull)   /*  607 */,
		quint64(0xCB46ACC37C3F1F2Full)   /*  608 */,    quint64(0xDBB893FD02AAF50Eull)   /*  609 */,
		quint64(0x331FD92E600B9FCFull)   /*  610 */,    quint64(0xA498F96148EA3AD6ull)   /*  611 */,
		quint64(0xA8D8426E8B6A83EAull)   /*  612 */,    quint64(0xA089B274B7735CDCull)   /*  613 */,
		quint64(0x87F6B3731E524A11ull)   /*  614 */,    quint64(0x118808E5CBC96749ull)   /*  615 */,
		quint64(0x9906E4C7B19BD394ull)   /*  616 */,    quint64(0xAFED7F7E9B24A20Cull)   /*  617 */,
		quint64(0x6509EADEEB3644A7ull)   /*  618 */,    quint64(0x6C1EF1D3E8EF0EDEull)   /*  619 */,
		quint64(0xB9C97D43E9798FB4ull)   /*  620 */,    quint64(0xA2F2D784740C28A3ull)   /*  621 */,
		quint64(0x7B8496476197566Full)   /*  622 */,    quint64(0x7A5BE3E6B65F069Dull)   /*  623 */,
		quint64(0xF96330ED78BE6F10ull)   /*  624 */,    quint64(0xEEE60DE77A076A15ull)   /*  625 */,
		quint64(0x2B4BEE4AA08B9BD0ull)   /*  626 */,    quint64(0x6A56A63EC7B8894Eull)   /*  627 */,
		quint64(0x02121359BA34FEF4ull)   /*  628 */,    quint64(0x4CBF99F8283703FCull)   /*  629 */,
		quint64(0x398071350CAF30C8ull)   /*  630 */,    quint64(0xD0A77A89F017687Aull)   /*  631 */,
		quint64(0xF1C1A9EB9E423569ull)   /*  632 */,    quint64(0x8C7976282DEE8199ull)   /*  633 */,
		quint64(0x5D1737A5DD1F7ABDull)   /*  634 */,    quint64(0x4F53433C09A9FA80ull)   /*  635 */,
		quint64(0xFA8B0C53DF7CA1D9ull)   /*  636 */,    quint64(0x3FD9DCBC886CCB77ull)   /*  637 */,
		quint64(0xC040917CA91B4720ull)   /*  638 */,    quint64(0x7DD00142F9D1DCDFull)   /*  639 */,
		quint64(0x8476FC1D4F387B58ull)   /*  640 */,    quint64(0x23F8E7C5F3316503ull)   /*  641 */,
		quint64(0x032A2244E7E37339ull)   /*  642 */,    quint64(0x5C87A5D750F5A74Bull)   /*  643 */,
		quint64(0x082B4CC43698992Eull)   /*  644 */,    quint64(0xDF917BECB858F63Cull)   /*  645 */,
		quint64(0x3270B8FC5BF86DDAull)   /*  646 */,    quint64(0x10AE72BB29B5DD76ull)   /*  647 */,
		quint64(0x576AC94E7700362Bull)   /*  648 */,    quint64(0x1AD112DAC61EFB8Full)   /*  649 */,
		quint64(0x691BC30EC5FAA427ull)   /*  650 */,    quint64(0xFF246311CC327143ull)   /*  651 */,
		quint64(0x3142368E30E53206ull)   /*  652 */,    quint64(0x71380E31E02CA396ull)   /*  653 */,
		quint64(0x958D5C960AAD76F1ull)   /*  654 */,    quint64(0xF8D6F430C16DA536ull)   /*  655 */,
		quint64(0xC8FFD13F1BE7E1D2ull)   /*  656 */,    quint64(0x7578AE66004DDBE1ull)   /*  657 */,
		quint64(0x05833F01067BE646ull)   /*  658 */,    quint64(0xBB34B5AD3BFE586Dull)   /*  659 */,
		quint64(0x095F34C9A12B97F0ull)   /*  660 */,    quint64(0x247AB64525D60CA8ull)   /*  661 */,
		quint64(0xDCDBC6F3017477D1ull)   /*  662 */,    quint64(0x4A2E14D4DECAD24Dull)   /*  663 */,
		quint64(0xBDB5E6D9BE0A1EEBull)   /*  664 */,    quint64(0x2A7E70F7794301ABull)   /*  665 */,
		quint64(0xDEF42D8A270540FDull)   /*  666 */,    quint64(0x01078EC0A34C22C1ull)   /*  667 */,
		quint64(0xE5DE511AF4C16387ull)   /*  668 */,    quint64(0x7EBB3A52BD9A330Aull)   /*  669 */,
		quint64(0x77697857AA7D6435ull)   /*  670 */,    quint64(0x004E831603AE4C32ull)   /*  671 */,
		quint64(0xE7A21020AD78E312ull)   /*  672 */,    quint64(0x9D41A70C6AB420F2ull)   /*  673 */,
		quint64(0x28E06C18EA1141E6ull)   /*  674 */,    quint64(0xD2B28CBD984F6B28ull)   /*  675 */,
		quint64(0x26B75F6C446E9D83ull)   /*  676 */,    quint64(0xBA47568C4D418D7Full)   /*  677 */,
		quint64(0xD80BADBFE6183D8Eull)   /*  678 */,    quint64(0x0E206D7F5F166044ull)   /*  679 */,
		quint64(0xE258A43911CBCA3Eull)   /*  680 */,    quint64(0x723A1746B21DC0BCull)   /*  681 */,
		quint64(0xC7CAA854F5D7CDD3ull)   /*  682 */,    quint64(0x7CAC32883D261D9Cull)   /*  683 */,
		quint64(0x7690C26423BA942Cull)   /*  684 */,    quint64(0x17E55524478042B8ull)   /*  685 */,
		quint64(0xE0BE477656A2389Full)   /*  686 */,    quint64(0x4D289B5E67AB2DA0ull)   /*  687 */,
		quint64(0x44862B9C8FBBFD31ull)   /*  688 */,    quint64(0xB47CC8049D141365ull)   /*  689 */,
		quint64(0x822C1B362B91C793ull)   /*  690 */,    quint64(0x4EB14655FB13DFD8ull)   /*  691 */,
		quint64(0x1ECBBA0714E2A97Bull)   /*  692 */,    quint64(0x6143459D5CDE5F14ull)   /*  693 */,
		quint64(0x53A8FBF1D5F0AC89ull)   /*  694 */,    quint64(0x97EA04D81C5E5B00ull)   /*  695 */,
		quint64(0x622181A8D4FDB3F3ull)   /*  696 */,    quint64(0xE9BCD341572A1208ull)   /*  697 */,
		quint64(0x1411258643CCE58Aull)   /*  698 */,    quint64(0x9144C5FEA4C6E0A4ull)   /*  699 */,
		quint64(0x0D33D06565CF620Full)   /*  700 */,    quint64(0x54A48D489F219CA1ull)   /*  701 */,
		quint64(0xC43E5EAC6D63C821ull)   /*  702 */,    quint64(0xA9728B3A72770DAFull)   /*  703 */,
		quint64(0xD7934E7B20DF87EFull)   /*  704 */,    quint64(0xE35503B61A3E86E5ull)   /*  705 */,
		quint64(0xCAE321FBC819D504ull)   /*  706 */,    quint64(0x129A50B3AC60BFA6ull)   /*  707 */,
		quint64(0xCD5E68EA7E9FB6C3ull)   /*  708 */,    quint64(0xB01C90199483B1C7ull)   /*  709 */,
		quint64(0x3DE93CD5C295376Cull)   /*  710 */,    quint64(0xAED52EDF2AB9AD13ull)   /*  711 */,
		quint64(0x2E60F512C0A07884ull)   /*  712 */,    quint64(0xBC3D86A3E36210C9ull)   /*  713 */,
		quint64(0x35269D9B163951CEull)   /*  714 */,    quint64(0x0C7D6E2AD0CDB5FAull)   /*  715 */,
		quint64(0x59E86297D87F5733ull)   /*  716 */,    quint64(0x298EF221898DB0E7ull)   /*  717 */,
		quint64(0x55000029D1A5AA7Eull)   /*  718 */,    quint64(0x8BC08AE1B5061B45ull)   /*  719 */,
		quint64(0xC2C31C2B6C92703Aull)   /*  720 */,    quint64(0x94CC596BAF25EF42ull)   /*  721 */,
		quint64(0x0A1D73DB22540456ull)   /*  722 */,    quint64(0x04B6A0F9D9C4179Aull)   /*  723 */,
		quint64(0xEFFDAFA2AE3D3C60ull)   /*  724 */,    quint64(0xF7C8075BB49496C4ull)   /*  725 */,
		quint64(0x9CC5C7141D1CD4E3ull)   /*  726 */,    quint64(0x78BD1638218E5534ull)   /*  727 */,
		quint64(0xB2F11568F850246Aull)   /*  728 */,    quint64(0xEDFABCFA9502BC29ull)   /*  729 */,
		quint64(0x796CE5F2DA23051Bull)   /*  730 */,    quint64(0xAAE128B0DC93537Cull)   /*  731 */,
		quint64(0x3A493DA0EE4B29AEull)   /*  732 */,    quint64(0xB5DF6B2C416895D7ull)   /*  733 */,
		quint64(0xFCABBD25122D7F37ull)   /*  734 */,    quint64(0x70810B58105DC4B1ull)   /*  735 */,
		quint64(0xE10FDD37F7882A90ull)   /*  736 */,    quint64(0x524DCAB5518A3F5Cull)   /*  737 */,
		quint64(0x3C9E85878451255Bull)   /*  738 */,    quint64(0x4029828119BD34E2ull)   /*  739 */,
		quint64(0x74A05B6F5D3CECCBull)   /*  740 */,    quint64(0xB610021542E13ECAull)   /*  741 */,
		quint64(0x0FF979D12F59E2ACull)   /*  742 */,    quint64(0x6037DA27E4F9CC50ull)   /*  743 */,
		quint64(0x5E92975A0DF1847Dull)   /*  744 */,    quint64(0xD66DE190D3E623FEull)   /*  745 */,
		quint64(0x5032D6B87B568048ull)   /*  746 */,    quint64(0x9A36B7CE8235216Eull)   /*  747 */,
		quint64(0x80272A7A24F64B4Aull)   /*  748 */,    quint64(0x93EFED8B8C6916F7ull)   /*  749 */,
		quint64(0x37DDBFF44CCE1555ull)   /*  750 */,    quint64(0x4B95DB5D4B99BD25ull)   /*  751 */,
		quint64(0x92D3FDA169812FC0ull)   /*  752 */,    quint64(0xFB1A4A9A90660BB6ull)   /*  753 */,
		quint64(0x730C196946A4B9B2ull)   /*  754 */,    quint64(0x81E289AA7F49DA68ull)   /*  755 */,
		quint64(0x64669A0F83B1A05Full)   /*  756 */,    quint64(0x27B3FF7D9644F48Bull)   /*  757 */,
		quint64(0xCC6B615C8DB675B3ull)   /*  758 */,    quint64(0x674F20B9BCEBBE95ull)   /*  759 */,
		quint64(0x6F31238275655982ull)   /*  760 */,    quint64(0x5AE488713E45CF05ull)   /*  761 */,
		quint64(0xBF619F9954C21157ull)   /*  762 */,    quint64(0xEABAC46040A8EAE9ull)   /*  763 */,
		quint64(0x454C6FE9F2C0C1CDull)   /*  764 */,    quint64(0x419CF6496412691Cull)   /*  765 */,
		quint64(0xD3DC3BEF265B0F70ull)   /*  766 */,    quint64(0x6D0E60F5C3578A9Eull)   /*  767 */,
		quint64(0x5B0E608526323C55ull)   /*  768 */,    quint64(0x1A46C1A9FA1B59F5ull)   /*  769 */,
		quint64(0xA9E245A17C4C8FFAull)   /*  770 */,    quint64(0x65CA5159DB2955D7ull)   /*  771 */,
		quint64(0x05DB0A76CE35AFC2ull)   /*  772 */,    quint64(0x81EAC77EA9113D45ull)   /*  773 */,
		quint64(0x528EF88AB6AC0A0Dull)   /*  774 */,    quint64(0xA09EA253597BE3FFull)   /*  775 */,
		quint64(0x430DDFB3AC48CD56ull)   /*  776 */,    quint64(0xC4B3A67AF45CE46Full)   /*  777 */,
		quint64(0x4ECECFD8FBE2D05Eull)   /*  778 */,    quint64(0x3EF56F10B39935F0ull)   /*  779 */,
		quint64(0x0B22D6829CD619C6ull)   /*  780 */,    quint64(0x17FD460A74DF2069ull)   /*  781 */,
		quint64(0x6CF8CC8E8510ED40ull)   /*  782 */,    quint64(0xD6C824BF3A6ECAA7ull)   /*  783 */,
		quint64(0x61243D581A817049ull)   /*  784 */,    quint64(0x048BACB6BBC163A2ull)   /*  785 */,
		quint64(0xD9A38AC27D44CC32ull)   /*  786 */,    quint64(0x7FDDFF5BAAF410ABull)   /*  787 */,
		quint64(0xAD6D495AA804824Bull)   /*  788 */,    quint64(0xE1A6A74F2D8C9F94ull)   /*  789 */,
		quint64(0xD4F7851235DEE8E3ull)   /*  790 */,    quint64(0xFD4B7F886540D893ull)   /*  791 */,
		quint64(0x247C20042AA4BFDAull)   /*  792 */,    quint64(0x096EA1C517D1327Cull)   /*  793 */,
		quint64(0xD56966B4361A6685ull)   /*  794 */,    quint64(0x277DA5C31221057Dull)   /*  795 */,
		quint64(0x94D59893A43ACFF7ull)   /*  796 */,    quint64(0x64F0C51CCDC02281ull)   /*  797 */,
		quint64(0x3D33BCC4FF6189DBull)   /*  798 */,    quint64(0xE005CB184CE66AF1ull)   /*  799 */,
		quint64(0xFF5CCD1D1DB99BEAull)   /*  800 */,    quint64(0xB0B854A7FE42980Full)   /*  801 */,
		quint64(0x7BD46A6A718D4B9Full)   /*  802 */,    quint64(0xD10FA8CC22A5FD8Cull)   /*  803 */,
		quint64(0xD31484952BE4BD31ull)   /*  804 */,    quint64(0xC7FA975FCB243847ull)   /*  805 */,
		quint64(0x4886ED1E5846C407ull)   /*  806 */,    quint64(0x28CDDB791EB70B04ull)   /*  807 */,
		quint64(0xC2B00BE2F573417Full)   /*  808 */,    quint64(0x5C9590452180F877ull)   /*  809 */,
		quint64(0x7A6BDDFFF370EB00ull)   /*  810 */,    quint64(0xCE509E38D6D9D6A4ull)   /*  811 */,
		quint64(0xEBEB0F00647FA702ull)   /*  812 */,    quint64(0x1DCC06CF76606F06ull)   /*  813 */,
		quint64(0xE4D9F28BA286FF0Aull)   /*  814 */,    quint64(0xD85A305DC918C262ull)   /*  815 */,
		quint64(0x475B1D8732225F54ull)   /*  816 */,    quint64(0x2D4FB51668CCB5FEull)   /*  817 */,
		quint64(0xA679B9D9D72BBA20ull)   /*  818 */,    quint64(0x53841C0D912D43A5ull)   /*  819 */,
		quint64(0x3B7EAA48BF12A4E8ull)   /*  820 */,    quint64(0x781E0E47F22F1DDFull)   /*  821 */,
		quint64(0xEFF20CE60AB50973ull)   /*  822 */,    quint64(0x20D261D19DFFB742ull)   /*  823 */,
		quint64(0x16A12B03062A2E39ull)   /*  824 */,    quint64(0x1960EB2239650495ull)   /*  825 */,
		quint64(0x251C16FED50EB8B8ull)   /*  826 */,    quint64(0x9AC0C330F826016Eull)   /*  827 */,
		quint64(0xED152665953E7671ull)   /*  828 */,    quint64(0x02D63194A6369570ull)   /*  829 */,
		quint64(0x5074F08394B1C987ull)   /*  830 */,    quint64(0x70BA598C90B25CE1ull)   /*  831 */,
		quint64(0x794A15810B9742F6ull)   /*  832 */,    quint64(0x0D5925E9FCAF8C6Cull)   /*  833 */,
		quint64(0x3067716CD868744Eull)   /*  834 */,    quint64(0x910AB077E8D7731Bull)   /*  835 */,
		quint64(0x6A61BBDB5AC42F61ull)   /*  836 */,    quint64(0x93513EFBF0851567ull)   /*  837 */,
		quint64(0xF494724B9E83E9D5ull)   /*  838 */,    quint64(0xE887E1985C09648Dull)   /*  839 */,
		quint64(0x34B1D3C675370CFDull)   /*  840 */,    quint64(0xDC35E433BC0D255Dull)   /*  841 */,
		quint64(0xD0AAB84234131BE0ull)   /*  842 */,    quint64(0x08042A50B48B7EAFull)   /*  843 */,
		quint64(0x9997C4EE44A3AB35ull)   /*  844 */,    quint64(0x829A7B49201799D0ull)   /*  845 */,
		quint64(0x263B8307B7C54441ull)   /*  846 */,    quint64(0x752F95F4FD6A6CA6ull)   /*  847 */,
		quint64(0x927217402C08C6E5ull)   /*  848 */,    quint64(0x2A8AB754A795D9EEull)   /*  849 */,
		quint64(0xA442F7552F72943Dull)   /*  850 */,    quint64(0x2C31334E19781208ull)   /*  851 */,
		quint64(0x4FA98D7CEAEE6291ull)   /*  852 */,    quint64(0x55C3862F665DB309ull)   /*  853 */,
		quint64(0xBD0610175D53B1F3ull)   /*  854 */,    quint64(0x46FE6CB840413F27ull)   /*  855 */,
		quint64(0x3FE03792DF0CFA59ull)   /*  856 */,    quint64(0xCFE700372EB85E8Full)   /*  857 */,
		quint64(0xA7BE29E7ADBCE118ull)   /*  858 */,    quint64(0xE544EE5CDE8431DDull)   /*  859 */,
		quint64(0x8A781B1B41F1873Eull)   /*  860 */,    quint64(0xA5C94C78A0D2F0E7ull)   /*  861 */,
		quint64(0x39412E2877B60728ull)   /*  862 */,    quint64(0xA1265EF3AFC9A62Cull)   /*  863 */,
		quint64(0xBCC2770C6A2506C5ull)   /*  864 */,    quint64(0x3AB66DD5DCE1CE12ull)   /*  865 */,
		quint64(0xE65499D04A675B37ull)   /*  866 */,    quint64(0x7D8F523481BFD216ull)   /*  867 */,
		quint64(0x0F6F64FCEC15F389ull)   /*  868 */,    quint64(0x74EFBE618B5B13C8ull)   /*  869 */,
		quint64(0xACDC82B714273E1Dull)   /*  870 */,    quint64(0xDD40BFE003199D17ull)   /*  871 */,
		quint64(0x37E99257E7E061F8ull)   /*  872 */,    quint64(0xFA52626904775AAAull)   /*  873 */,
		quint64(0x8BBBF63A463D56F9ull)   /*  874 */,    quint64(0xF0013F1543A26E64ull)   /*  875 */,
		quint64(0xA8307E9F879EC898ull)   /*  876 */,    quint64(0xCC4C27A4150177CCull)   /*  877 */,
		quint64(0x1B432F2CCA1D3348ull)   /*  878 */,    quint64(0xDE1D1F8F9F6FA013ull)   /*  879 */,
		quint64(0x606602A047A7DDD6ull)   /*  880 */,    quint64(0xD237AB64CC1CB2C7ull)   /*  881 */,
		quint64(0x9B938E7225FCD1D3ull)   /*  882 */,    quint64(0xEC4E03708E0FF476ull)   /*  883 */,
		quint64(0xFEB2FBDA3D03C12Dull)   /*  884 */,    quint64(0xAE0BCED2EE43889Aull)   /*  885 */,
		quint64(0x22CB8923EBFB4F43ull)   /*  886 */,    quint64(0x69360D013CF7396Dull)   /*  887 */,
		quint64(0x855E3602D2D4E022ull)   /*  888 */,    quint64(0x073805BAD01F784Cull)   /*  889 */,
		quint64(0x33E17A133852F546ull)   /*  890 */,    quint64(0xDF4874058AC7B638ull)   /*  891 */,
		quint64(0xBA92B29C678AA14Aull)   /*  892 */,    quint64(0x0CE89FC76CFAADCDull)   /*  893 */,
		quint64(0x5F9D4E0908339E34ull)   /*  894 */,    quint64(0xF1AFE9291F5923B9ull)   /*  895 */,
		quint64(0x6E3480F60F4A265Full)   /*  896 */,    quint64(0xEEBF3A2AB29B841Cull)   /*  897 */,
		quint64(0xE21938A88F91B4ADull)   /*  898 */,    quint64(0x57DFEFF845C6D3C3ull)   /*  899 */,
		quint64(0x2F006B0BF62CAAF2ull)   /*  900 */,    quint64(0x62F479EF6F75EE78ull)   /*  901 */,
		quint64(0x11A55AD41C8916A9ull)   /*  902 */,    quint64(0xF229D29084FED453ull)   /*  903 */,
		quint64(0x42F1C27B16B000E6ull)   /*  904 */,    quint64(0x2B1F76749823C074ull)   /*  905 */,
		quint64(0x4B76ECA3C2745360ull)   /*  906 */,    quint64(0x8C98F463B91691BDull)   /*  907 */,
		quint64(0x14BCC93CF1ADE66Aull)   /*  908 */,    quint64(0x8885213E6D458397ull)   /*  909 */,
		quint64(0x8E177DF0274D4711ull)   /*  910 */,    quint64(0xB49B73B5503F2951ull)   /*  911 */,
		quint64(0x10168168C3F96B6Bull)   /*  912 */,    quint64(0x0E3D963B63CAB0AEull)   /*  913 */,
		quint64(0x8DFC4B5655A1DB14ull)   /*  914 */,    quint64(0xF789F1356E14DE5Cull)   /*  915 */,
		quint64(0x683E68AF4E51DAC1ull)   /*  916 */,    quint64(0xC9A84F9D8D4B0FD9ull)   /*  917 */,
		quint64(0x3691E03F52A0F9D1ull)   /*  918 */,    quint64(0x5ED86E46E1878E80ull)   /*  919 */,
		quint64(0x3C711A0E99D07150ull)   /*  920 */,    quint64(0x5A0865B20C4E9310ull)   /*  921 */,
		quint64(0x56FBFC1FE4F0682Eull)   /*  922 */,    quint64(0xEA8D5DE3105EDF9Bull)   /*  923 */,
		quint64(0x71ABFDB12379187Aull)   /*  924 */,    quint64(0x2EB99DE1BEE77B9Cull)   /*  925 */,
		quint64(0x21ECC0EA33CF4523ull)   /*  926 */,    quint64(0x59A4D7521805C7A1ull)   /*  927 */,
		quint64(0x3896F5EB56AE7C72ull)   /*  928 */,    quint64(0xAA638F3DB18F75DCull)   /*  929 */,
		quint64(0x9F39358DABE9808Eull)   /*  930 */,    quint64(0xB7DEFA91C00B72ACull)   /*  931 */,
		quint64(0x6B5541FD62492D92ull)   /*  932 */,    quint64(0x6DC6DEE8F92E4D5Bull)   /*  933 */,
		quint64(0x353F57ABC4BEEA7Eull)   /*  934 */,    quint64(0x735769D6DA5690CEull)   /*  935 */,
		quint64(0x0A234AA642391484ull)   /*  936 */,    quint64(0xF6F9508028F80D9Dull)   /*  937 */,
		quint64(0xB8E319A27AB3F215ull)   /*  938 */,    quint64(0x31AD9C1151341A4Dull)   /*  939 */,
		quint64(0x773C22A57BEF5805ull)   /*  940 */,    quint64(0x45C7561A07968633ull)   /*  941 */,
		quint64(0xF913DA9E249DBE36ull)   /*  942 */,    quint64(0xDA652D9B78A64C68ull)   /*  943 */,
		quint64(0x4C27A97F3BC334EFull)   /*  944 */,    quint64(0x76621220E66B17F4ull)   /*  945 */,
		quint64(0x967743899ACD7D0Bull)   /*  946 */,    quint64(0xF3EE5BCAE0ED6782ull)   /*  947 */,
		quint64(0x409F753600C879FCull)   /*  948 */,    quint64(0x06D09A39B5926DB6ull)   /*  949 */,
		quint64(0x6F83AEB0317AC588ull)   /*  950 */,    quint64(0x01E6CA4A86381F21ull)   /*  951 */,
		quint64(0x66FF3462D19F3025ull)   /*  952 */,    quint64(0x72207C24DDFD3BFBull)   /*  953 */,
		quint64(0x4AF6B6D3E2ECE2EBull)   /*  954 */,    quint64(0x9C994DBEC7EA08DEull)   /*  955 */,
		quint64(0x49ACE597B09A8BC4ull)   /*  956 */,    quint64(0xB38C4766CF0797BAull)   /*  957 */,
		quint64(0x131B9373C57C2A75ull)   /*  958 */,    quint64(0xB1822CCE61931E58ull)   /*  959 */,
		quint64(0x9D7555B909BA1C0Cull)   /*  960 */,    quint64(0x127FAFDD937D11D2ull)   /*  961 */,
		quint64(0x29DA3BADC66D92E4ull)   /*  962 */,    quint64(0xA2C1D57154C2ECBCull)   /*  963 */,
		quint64(0x58C5134D82F6FE24ull)   /*  964 */,    quint64(0x1C3AE3515B62274Full)   /*  965 */,
		quint64(0xE907C82E01CB8126ull)   /*  966 */,    quint64(0xF8ED091913E37FCBull)   /*  967 */,
		quint64(0x3249D8F9C80046C9ull)   /*  968 */,    quint64(0x80CF9BEDE388FB63ull)   /*  969 */,
		quint64(0x1881539A116CF19Eull)   /*  970 */,    quint64(0x5103F3F76BD52457ull)   /*  971 */,
		quint64(0x15B7E6F5AE47F7A8ull)   /*  972 */,    quint64(0xDBD7C6DED47E9CCFull)   /*  973 */,
		quint64(0x44E55C410228BB1Aull)   /*  974 */,    quint64(0xB647D4255EDB4E99ull)   /*  975 */,
		quint64(0x5D11882BB8AAFC30ull)   /*  976 */,    quint64(0xF5098BBB29D3212Aull)   /*  977 */,
		quint64(0x8FB5EA14E90296B3ull)   /*  978 */,    quint64(0x677B942157DD025Aull)   /*  979 */,
		quint64(0xFB58E7C0A390ACB5ull)   /*  980 */,    quint64(0x89D3674C83BD4A01ull)   /*  981 */,
		quint64(0x9E2DA4DF4BF3B93Bull)   /*  982 */,    quint64(0xFCC41E328CAB4829ull)   /*  983 */,
		quint64(0x03F38C96BA582C52ull)   /*  984 */,    quint64(0xCAD1BDBD7FD85DB2ull)   /*  985 */,
		quint64(0xBBB442C16082AE83ull)   /*  986 */,    quint64(0xB95FE86BA5DA9AB0ull)   /*  987 */,
		quint64(0xB22E04673771A93Full)   /*  988 */,    quint64(0x845358C9493152D8ull)   /*  989 */,
		quint64(0xBE2A488697B4541Eull)   /*  990 */,    quint64(0x95A2DC2DD38E6966ull)   /*  991 */,
		quint64(0xC02C11AC923C852Bull)   /*  992 */,    quint64(0x2388B1990DF2A87Bull)   /*  993 */,
		quint64(0x7C8008FA1B4F37BEull)   /*  994 */,    quint64(0x1F70D0C84D54E503ull)   /*  995 */,
		quint64(0x5490ADEC7ECE57D4ull)   /*  996 */,    quint64(0x002B3C27D9063A3Aull)   /*  997 */,
		quint64(0x7EAEA3848030A2BFull)   /*  998 */,    quint64(0xC602326DED2003C0ull)   /*  999 */,
		quint64(0x83A7287D69A94086ull)   /* 1000 */,    quint64(0xC57A5FCB30F57A8Aull)   /* 1001 */,
		quint64(0xB56844E479EBE779ull)   /* 1002 */,    quint64(0xA373B40F05DCBCE9ull)   /* 1003 */,
		quint64(0xD71A786E88570EE2ull)   /* 1004 */,    quint64(0x879CBACDBDE8F6A0ull)   /* 1005 */,
		quint64(0x976AD1BCC164A32Full)   /* 1006 */,    quint64(0xAB21E25E9666D78Bull)   /* 1007 */,
		quint64(0x901063AAE5E5C33Cull)   /* 1008 */,    quint64(0x9818B34448698D90ull)   /* 1009 */,
		quint64(0xE36487AE3E1E8ABBull)   /* 1010 */,    quint64(0xAFBDF931893BDCB4ull)   /* 1011 */,
		quint64(0x6345A0DC5FBBD519ull)   /* 1012 */,    quint64(0x8628FE269B9465CAull)   /* 1013 */,
		quint64(0x1E5D01603F9C51ECull)   /* 1014 */,    quint64(0x4DE44006A15049B7ull)   /* 1015 */,
		quint64(0xBF6C70E5F776CBB1ull)   /* 1016 */,    quint64(0x411218F2EF552BEDull)   /* 1017 */,
		quint64(0xCB0C0708705A36A3ull)   /* 1018 */,    quint64(0xE74D14754F986044ull)   /* 1019 */,
		quint64(0xCD56D9430EA8280Eull)   /* 1020 */,    quint64(0xC12591D7535F5065ull)   /* 1021 */,
		quint64(0xC83223F1720AEF96ull)   /* 1022 */,    quint64(0xC3A0396F7363A51Full)   /* 1023 */
};

/**
 * @file
 * $Id: TigerHash.cpp,v 1.7 2005/04/24 08:13:11 arnetheduck Exp $
 */

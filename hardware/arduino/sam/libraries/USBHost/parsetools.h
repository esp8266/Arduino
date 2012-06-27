/* Copyright (C) 2011 Circuits At Home, LTD. All rights reserved.

This software may be distributed and modified under the terms of the GNU
General Public License version 2 (GPL2) as published by the Free Software
Foundation and appearing in the file GPL2.TXT included in the packaging of
this file. Please note that GPL2 Section 2[b] requires that all works based
on this software must also be made publicly available under the terms of
the GPL2 ("Copyleft").

Contact information
-------------------

Circuits At Home, LTD
Web      :  http://www.circuitsathome.com
e-mail   :  support@circuitsathome.com
*/

#ifndef PARSETOOLS_H_INCLUDED
#define PARSETOOLS_H_INCLUDED

#include <stdint.h>
#include "Arduino.h"

struct MultiValueBuffer
{
	uint8_t		valueSize;
	void		*pValue;
};

class MultiByteValueParser
{
	uint8_t				*pBuf;
	uint32_t			countDown;
	uint32_t			valueSize;

public:
	MultiByteValueParser() : pBuf(NULL), countDown(0), valueSize(0) {};

	const uint8_t* GetBuffer() { return pBuf; };

	void Initialize(MultiValueBuffer * const pbuf)
	{
		pBuf = (uint8_t*)pbuf->pValue;
		countDown = valueSize = pbuf->valueSize;
	};

	bool Parse(uint8_t **pp, uint32_t *pcntdn);
};

class ByteSkipper
{
	uint8_t				*pBuf;
	uint32_t			nStage;
	uint32_t			countDown;

public:
	ByteSkipper() : pBuf(NULL), nStage(0), countDown(0) {};

	void Initialize(MultiValueBuffer *pbuf)
	{
		pBuf = (uint8_t*)pbuf->pValue;
		countDown = 0;
	};

	bool Skip(uint8_t **pp, uint32_t *pcntdn, uint32_t bytes_to_skip)
	{
		switch (nStage)
		{
		case 0:
			countDown = bytes_to_skip;
			nStage ++;
		case 1:
			for (; countDown && (*pcntdn); countDown--, (*pp)++, (*pcntdn)--);

			if (!countDown)
				nStage = 0;
		};
		return (!countDown);
	};
};

// Pointer to a callback function triggered for each element of PTP array when used with PTPArrayParser
typedef void (*PTP_ARRAY_EL_FUNC)(const MultiValueBuffer * const p, uint32_t count, const void *me);

class PTPListParser
{
public:
	enum ParseMode { modeArray, modeRange/*, modeEnum*/ };

private:
	uint32_t				nStage;
	uint32_t				enStage;

	uint32_t				arLen;
	uint32_t				arLenCntdn;

	uint32_t				lenSize;		// size of the array length field in bytes
	uint32_t				valSize;		// size of the array element in bytes

	MultiValueBuffer		*pBuf;

	// The only parser for both size and array element parsing
	MultiByteValueParser	theParser;

	uint32_t /*ParseMode*/	prsMode;

public:
	PTPListParser() :
		nStage(0),
		enStage(0),
		arLen(0),
		arLenCntdn(0),
		lenSize(0),
		valSize(0),
		pBuf(NULL),
		prsMode(modeArray)
		{};

	void Initialize(const uint32_t len_size, const uint32_t val_size, MultiValueBuffer * const p, const uint32_t mode = modeArray)
	{
		pBuf	= p;
		lenSize	= len_size;
		valSize = val_size;
		prsMode = mode;

		if (prsMode == modeRange)
		{
			arLenCntdn = arLen = 3;
			nStage = 2;
		}
		else
		{
			arLenCntdn = arLen = 0;
			nStage = 0;
		}
		enStage = 0;
		theParser.Initialize(p);
	};

	bool Parse(uint8_t **pp, uint32_t *pcntdn, PTP_ARRAY_EL_FUNC pf, const void *me = NULL);
};

#endif /* PARSETOOLS_H_INCLUDED */

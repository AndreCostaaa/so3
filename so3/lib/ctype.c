/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+

 *  linux/lib/ctype.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <ctype.h>

const char hex_asc[] = "0123456789abcdef";
const char hex_asc_upper[] = "0123456789ABCDEF";

const unsigned char _ctype[] = {
	_C,	  _C,	   _C,	    _C,	     _C,      _C,      _C,	_C, /* 0-7 */
	_C,	  _C | _S, _C | _S, _C | _S, _C | _S, _C | _S, _C,	_C, /* 8-15 */
	_C,	  _C,	   _C,	    _C,	     _C,      _C,      _C,	_C, /* 16-23 */
	_C,	  _C,	   _C,	    _C,	     _C,      _C,      _C,	_C, /* 24-31 */
	_S | _SP, _P,	   _P,	    _P,	     _P,      _P,      _P,	_P, /* 32-39 */
	_P,	  _P,	   _P,	    _P,	     _P,      _P,      _P,	_P, /* 40-47 */
	_D,	  _D,	   _D,	    _D,	     _D,      _D,      _D,	_D, /* 48-55 */
	_D,	  _D,	   _P,	    _P,	     _P,      _P,      _P,	_P, /* 56-63 */
	_P,	  _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U, /* 64-71 */
	_U,	  _U,	   _U,	    _U,	     _U,      _U,      _U,	_U, /* 72-79 */
	_U,	  _U,	   _U,	    _U,	     _U,      _U,      _U,	_U, /* 80-87 */
	_U,	  _U,	   _U,	    _P,	     _P,      _P,      _P,	_P, /* 88-95 */
	_P,	  _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L, /* 96-103 */
	_L,	  _L,	   _L,	    _L,	     _L,      _L,      _L,	_L, /* 104-111 */
	_L,	  _L,	   _L,	    _L,	     _L,      _L,      _L,	_L, /* 112-119 */
	_L,	  _L,	   _L,	    _P,	     _P,      _P,      _P,	_C, /* 120-127 */
	0,	  0,	   0,	    0,	     0,	      0,       0,	0,  0,	0,  0,	0,  0,	0,  0,	0, /* 128-143 */
	0,	  0,	   0,	    0,	     0,	      0,       0,	0,  0,	0,  0,	0,  0,	0,  0,	0, /* 144-159 */
	_S | _SP, _P,	   _P,	    _P,	     _P,      _P,      _P,	_P, _P, _P, _P, _P, _P, _P, _P, _P, /* 160-175 */
	_P,	  _P,	   _P,	    _P,	     _P,      _P,      _P,	_P, _P, _P, _P, _P, _P, _P, _P, _P, /* 176-191 */
	_U,	  _U,	   _U,	    _U,	     _U,      _U,      _U,	_U, _U, _U, _U, _U, _U, _U, _U, _U, /* 192-207 */
	_U,	  _U,	   _U,	    _U,	     _U,      _U,      _U,	_P, _U, _U, _U, _U, _U, _U, _U, _L, /* 208-223 */
	_L,	  _L,	   _L,	    _L,	     _L,      _L,      _L,	_L, _L, _L, _L, _L, _L, _L, _L, _L, /* 224-239 */
	_L,	  _L,	   _L,	    _L,	     _L,      _L,      _L,	_P, _L, _L, _L, _L, _L, _L, _L, _L
}; /* 240-255 */

int hex_to_bin(char ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - '0';
	ch = tolower(ch);
	if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	return -1;
}

/*
* 
* ************************************************************************
* *                                                                      *
* *               This software is part of the ast package               *
* *          Copyright (c) 1985-2008 AT&T Intellectual Property          *
* *                      and is licensed under the                       *
* *                  Common Public License, Version 1.0                  *
* *                    by AT&T Intellectual Property                     *
* *                                                                      *
* *                A copy of the License is available at                 *
* *            http://www.opensource.org/licenses/cpl1.0.txt             *
* *         (with md5 checksum 059e8cd6165cb4c31e351f2b69388fd9)         *
* *                                                                      *
* *              Information and Software Systems Research               *
* *                            AT&T Research                             *
* *                           Florham Park NJ                            *
* *                                                                      *
* *                 Glenn Fowler <gsf@research.att.com>                  *
* *                  David Korn <dgk@research.att.com>                   *
* *                   Phong Vo <kpv@research.att.com>                    *
* *                                                                      *
* ************************************************************************
*/
#include	"sfstdio.h"

/*	Get a byte.
**	Written by Kiem-Phong Vo
*/

#if __STD_C
int fgetc(reg FILE* f)
#else
int fgetc(f)
reg FILE*	f;
#endif
{
	reg Sfio_t*	sf;
	reg int		rv;

	if(!(sf = _sfstream(f)))
		return -1;
	if((rv = sfgetc(sf)) < 0)
		_stdseterr(f,sf);
	return rv;
}

#if _lib__IO_getc && !_done_IO_getc && !defined(fgetc)
#define _done_IO_getc	1
#define fgetc	_IO_getc
#include	"fgetc.c"
#undef fgetc
#endif

#if _lib_fgetc_unlocked && !_done_fgetc_unlocked && !defined(fgetc)
#define _done_fgetc_unlocked	1
#define fgetc	fgetc_unlocked
#include	"fgetc.c"
#undef fgetc
#endif

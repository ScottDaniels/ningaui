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
#include	"sftest.h"

static int	Type;

#if __STD_C
static void notify(Sfio_t* f, int type, int fd)
#else
static void notify(f, type, fd)
Sfio_t*	f;
int		type;
int		fd;
#endif
{
	switch(Type = type)
	{
	case SF_NEW:
	case SF_CLOSING:
	case SF_SETFD:
	case SF_READ:
	case SF_WRITE:
		return;
	default:
		terror("Unexpected nofity-type: %d\n",type);
	}
}

MAIN()
{
	Sfio_t*	f;
	int	fd;

	sfnotify(notify);

	if(!(f = sfopen(NIL(Sfio_t*), tstfile(0), "w")) && Type != SF_NEW)
		terror("Notify did not announce SF_NEW event\n");
	fd = sffileno(f);
	close(fd+5);
	if(sfsetfd(f,fd+5) != fd+5 || Type != SF_SETFD)
		terror("Notify did not announce SF_SETFD event\n");
	if(sfclose(f) < 0 || Type != SF_CLOSING)
		terror("Notify did not announce SF_CLOSING event\n");
	
	if(sfputc(sfstdin,'a') >= 0 || Type != SF_WRITE)
		terror("Notify did not announce SF_WRITE event\n");

	if(sfgetc(sfstdout) >= 0 || Type != SF_READ)
		terror("Notify did not announce SF_READ event\n");

	TSTEXIT(0);
}

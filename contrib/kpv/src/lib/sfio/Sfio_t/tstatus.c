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

MAIN()
{
	Sfio_t*	ip;
	Sfio_t*	op;
	int	n;

	if(!(ip = sfopen((Sfio_t*)0, "/dev/null", "r")))
		terror("/dev/null read open\n");
	if(!(op = sfopen((Sfio_t*)0, tstfile(0), "w")))
		terror("Write open\n");

	n = (int)sfmove(ip, op, SF_UNBOUND, -1);

	if(n)
		terror("move count %d != 0\n", n);
	if(!sfeof(ip))
		terror("sfeof(ip) expected\n");
	if(sfeof(op))
		terror("sfeof(op) not expected\n");

	if(sferror(ip))
		terror("sferror(ip) not expected\n");
	if(sferror(op))
		terror("sferror(op) not expected\n");

	if(sfclose(ip))
		terror("sfclose(ip)\n");
	if(sfclose(op))
		terror("sfclose(op)\n");

	TSTEXIT(0);
}

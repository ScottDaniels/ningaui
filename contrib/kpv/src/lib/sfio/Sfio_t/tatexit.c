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

/* test to see if files created in atexit functions work ok */

void ae()
{
	Sfio_t*	f = sfopen(NIL(Sfio_t*), tstfile(0), "w");

	if(!f)
		terror("Can't create file");

	if(sfwrite(f,"1234\n",5) != 5)
		terror("Can't write to file");
}

MAIN()
{
	Sfio_t* f;

	if(argc <= 1) /* atexit function registered after some sfio access */
	{	if(!(f = sfopen(NIL(Sfio_t*), tstfile(1), "w")) )
			terror("Can't create file");
		if(sfwrite(f,"1234\n",5) != 5)
			terror("Can't write to file");

		atexit(ae);

		system(sfprints("%s 1",argv[0]));
	}
	else /* atexit function registered before some sfio access */
	{	atexit(ae);

		if(!(f = sfopen(NIL(Sfio_t*), tstfile(1), "w")) )
			terror("Can't create file");
		if(sfwrite(f,"1234\n",5) != 5)
			terror("Can't write to file");
	}

	TSTEXIT(0);
}

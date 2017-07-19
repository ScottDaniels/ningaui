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
	char	*s = "1234567890\n";
	Sfoff_t	n, i;
	Sfio_t	*f;
	char	buf[1024];
	char*	addr;

	if(sfopen(sfstdout,tstfile(0),"w+") != sfstdout)
		terror("Opening output file\n");
	for(i = 0; i < 10000; ++i)
		if(sfputr(sfstdout,s,-1) < 0)
			terror("Writing data\n");

	if(!(f = sfopen((Sfio_t*)0,tstfile(1),"w")))
		terror("Opening output file \n");

	sfseek(sfstdout,(Sfoff_t)0,0);
	if((n = sfmove(sfstdout,f,(Sfoff_t)SF_UNBOUND,'\n')) != i)
		terror("Move %d lines, Expect %d\n",n,i);

	sfseek(sfstdout,(Sfoff_t)0,0);
	sfseek(f,(Sfoff_t)0,0);
	sfsetbuf(sfstdout,buf,sizeof(buf));
	if((n = sfmove(sfstdout,f,(Sfoff_t)SF_UNBOUND,'\n')) != i)
		terror("Move %d lines, Expect %d\n",n,i);

	sfopen(sfstdin,tstfile(0),"r");
	sfopen(sfstdout,tstfile(1),"w");
	sfmove(sfstdin,sfstdout,(Sfoff_t)SF_UNBOUND,-1);
	if(!sfeof(sfstdin))
		terror("Sfstdin is not eof\n");
	if(sferror(sfstdin))
		terror("Sfstdin is in error\n");
	if(sferror(sfstdout))
		terror("Sfstdout is in error\n");

	sfseek(sfstdin,(Sfoff_t)0,0);
	sfseek(sfstdout,(Sfoff_t)0,0);
	sfsetbuf(sfstdin,buf,sizeof(buf));

	addr = (char*)sbrk(0);
	sfmove(sfstdin,sfstdout,(Sfoff_t)((unsigned long)(~0L)>>1),-1);
	if((ssize_t)((char*)sbrk(0)-addr) > 256*1024)
		terror("Too much space allocated in sfmove\n");

	if(!sfeof(sfstdin))
		terror("Sfstdin is not eof2\n");
	if(sferror(sfstdin))
		terror("Sfstdin is in error2\n");
	if(sferror(sfstdout))
		terror("Sfstdout is in error2\n");

	TSTEXIT(0);
}

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

/*	Test to see if multiple writers to the same file create
**	a consistent set of records.
*/

#define N_PROC	3
#define N_REC	1000
#define B_SIZE	256

#if __STD_C
static ssize_t inspect(Sfio_t* f, const Void_t* buf, size_t n, Sfdisc_t* disc)
#else
static ssize_t inspect(f, buf, n, disc)
Sfio_t*		f;
Void_t* 	buf;
size_t		n;
Sfdisc_t*	disc;
#endif
{
	int	w, k;
	char*	s;
	Sfio_t*	sf;

	if(*((char*)buf + n-1) != '\n')
		terror("Not writing a whole record\n");

	sf = sfnew(NIL(Sfio_t*),(Void_t*)buf,n,-1,SF_STRING|SF_READ);
	while((s = sfgetr(sf,'\n',0)) )
	{	w = sfvalue(sf)-1;
		if(s[0] != s[w-1]-1)
			terror("Bad record\n");
		k = (s[2]-'0')*10 + (s[3]-'0');
		if(k != w)
			terror("Expect %d, get %d\n",k,w);
	}
	sfclose(sf);

	if((w = write(sffileno(f),buf,n)) != (int)n)
		terror("Write %d returns %d\n",n,w);

	return w;
}

static Sfdisc_t	Disc[N_PROC];

MAIN()
{
	ssize_t		size[N_PROC][N_REC];
	int		count[N_PROC];
	char		record[N_PROC][128], *s, *file;
	int		i, r, n, pid;
	Sfio_t*		f;
	Sfio_t*		fa[N_PROC];
	char		buf[N_PROC][B_SIZE], b[N_PROC][128];
	unsigned long	u;

	/* create pseudo-random record sizes */
	u = 1;
	for(i = 0; i < N_PROC; ++i)
	for(r = 0; r < N_REC; ++r)
	{	u = u * 0x63c63cd9L + 0x9c39c33dL;
		size[i][r] = (ssize_t)(u%63) + 16;
	}

	/* records for different processes */
	for(i = 0; i < N_PROC; ++i)
	for(r = 0; r < 128; ++r)
		record[i][r] = '0' + 2*i;

	/* create file */
	file = tstfile(0);
	if(!(f = sfopen(NIL(Sfio_t*),file,"w+")) )
		terror("Opening temporary file %s\n", file);

	/* open file for appending */
	for(i = 0; i < N_PROC; ++i)
		if(!(fa[i] = sfopen(NIL(Sfio_t*), file, "a")) )
			terror("Open %s to append", file);

	/* fork processes */
	for(i = 0; i < N_PROC; ++i)
	{
#ifdef DEBUG
#define FORK()		0
#define RETURN(v)
#else
#define FORK()		fork()
#define RETURN(v)	exit(v)
#endif
		if((pid = (int)FORK()) < 0 )
			terror("Creating process %d\n", i);
		else if(pid == 0)
		{	/* write to file */
			sfsetbuf(fa[i], (Void_t*)buf[i], sizeof(buf[i]));
			sfset(fa[i], SF_WHOLE, 1);
			Disc[i].writef = inspect;
			sfdisc(fa[i], &Disc[i]);

			for(r = 0; r < N_REC; ++r)
			{	n = size[i][r]; s = b[i];
				memcpy(s,record[i],n-1);
				s[1] = '(';
				s[2] = ((n-1)/10) + '0';
				s[3] = ((n-1)%10) + '0';
				s[4] = ')';
				s[n-2] = s[0] + 1;
				s[n-1] = '\n';
				if(sfwrite(fa[i],s,n) != n)
					terror("sfwrite failed");
			}
			sfsync(fa[i]);

			RETURN(0);
		}
	}

	for(i = 0; i < N_PROC; ++i)
	{	wait(&r);
		count[i] = 0;
	}

	n = 0;
	while((s = sfgetr(f,'\n',0)) )
	{	n += 1;
		if((i = s[0] - '0') < 0 || (i%2) != 0 || (i /= 2) >= N_PROC )
			terror("%d: Wrong record type\n", n);

		r = sfvalue(f);
		if(s[r-2] != s[0]+1)
			terror("%d: process%d, number=%d\n", n, i, count[i]);
		for(r -= 3; r > 4; --r)
			if(s[r] != s[0])
				terror("%d: process%d, count=%d\n", n, i, count[i]);

		if(sfvalue(f) != size[i][count[i]])
			terror("%d: process%d number=%d expected size=%d read size=%d\n",
				n, i, count[i], size[i][count[i]], sfvalue(f));

		count[i] += 1;
	}

	for(i = 0; i < N_PROC; ++i)
		if(count[i] != N_REC)
			terror("Bad count%d %d\n", i, count[i]);

	TSTEXIT(0);
}

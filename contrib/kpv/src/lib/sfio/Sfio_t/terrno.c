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

/* errnos tested for xopen-compliance */

MAIN()
{
	Sfio_t*	fw;
	Sfio_t*	fr;
	int	fds[2];
	int	lseek_errno;
	int	rv;

	if(!(fw = sfopen(NIL(Sfio_t*), tstfile(0), "w")) )
		terror("Can't create temp file %s to write", tstfile(0));
	if(!(fr = sfopen(NIL(Sfio_t*), tstfile(0), "r")) )
		terror("Can't open temp file %s to read", tstfile(0));

	sfseek(fr, (Sfoff_t)0, SEEK_END);
	if(sfgetc(fr) >= 0 || !sfeof(fr))
		terror("Should have seen eof");

	errno = 0;
	if((rv = sfwrite(fr, "a", 1)) == 1)
		terror("sfwrite returns %d, expecting 1", rv);
	if(errno != EBADF)
		twarn("Wrong errno %d after sfwrite(%d), expecting %d",errno,rv,EBADF);

	/* on some system (eg, apple), lseek does not set errno for this case */
	errno = 0;
	lseek(sffileno(fw), (off_t)(-2), SEEK_SET);
	lseek_errno = errno;
	lseek(sffileno(fw), (off_t)0, SEEK_SET);
	errno = 0;

	if(sfseek(fw, (Sfoff_t)(-2), SEEK_SET) != (Sfoff_t)(-1) )
		terror("sfseek should have failed");
	if(errno != lseek_errno)
		twarn("Wrong errno %d after sfseek, expecting %d", errno, lseek_errno);

	errno = 0;
	if(sfseek(fw, (Sfoff_t)0, SEEK_SET|SEEK_CUR|SEEK_END) >= 0)
		terror("sfseek should not have succeeded");
	if(errno != EINVAL)
		twarn("Wrong errno %d after sfseek, expecting %d", errno, EINVAL);

	if(pipe(fds) < 0)
		terror("Can't create pipes");

	if(!(fw = sfnew(fw, NIL(Void_t*), (size_t)SF_UNBOUND, fds[1], SF_WRITE)) )
		terror("Can't create stream for pipe");

	errno = 0;
	if(sfseek(fw, (Sfoff_t)0, SEEK_SET) >= 0)
		terror("sfseek should have failed on a pipe");
	if(errno != ESPIPE)
		twarn("Wrong errno %d after sfseek, expecting %d", ESPIPE);

	close(sffileno(fw));
	errno = 0;
	if(sfseek(fw, (Sfoff_t)0, SEEK_END) >= 0)
		terror("sfseek should have failed on a closed file descriptor");
	if(errno != EBADF)
		twarn("Wrong errno %d after sfseek, expecting %d", EBADF);

	TSTEXIT(0);
}

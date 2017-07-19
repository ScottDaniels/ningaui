#ifdef	MAIN
#define	EXTERN
#else
#define	EXTERN extern
#endif

#include	"io.h"
#include	<stdlib.h>
#include	<stdio.h>
#include	<setjmp.h>
#include	<string.h>

EXTERN int ifd;

EXTERN long lnum;
EXTERN long nbytes;
EXTERN long noverflow;
EXTERN int bflag;
EXTERN int cflag;
EXTERN int hflag;
EXTERN int iflag;
EXTERN int lflag;
EXTERN int Lflag;
EXTERN int nflag;
EXTERN int oneflag;
EXTERN int sflag;
EXTERN int vflag;
EXTERN int xflag;
EXTERN long nmatch;
EXTERN char *progname;
EXTERN char *curfile;
EXTERN int prname;
EXTERN int offsetunit;
EXTERN jmp_buf env;
EXTERN int longlinewarned;

extern char *optarg;
extern int optind;
extern int getopt(int, char* const *, const char*);

#ifdef BAD_FORM
	/* must include stdlib for opteron -- scott */
	extern void *calloc(int, int);
	extern void free(void*);
	extern void *malloc(int);
	extern void *realloc(void*, int);

	extern int read(int, char*, unsigned);
	extern int close(int);
#endif
extern int xopen(char *, int, ...);

/* this does not compile under linux or cygwin, as it exists as a macro, so we must check for it -- scott */
#ifndef tolower
extern int tolower(int);
#endif

extern void abort(void);
#ifdef	ROBHDR
extern void perror(char*);
#else
extern void perror(const char*);
#endif
extern void exit(int);

typedef void (*SUCCFN)(char*,char*);
extern void count(char *, char *);	/* updates lnum,nbytes */
extern void count_m(char *, char *);	/* updates lnum,nbytes */
extern int cwxrd(char**,char**);
extern int cwxmatch(char**,char**);
extern int bmxmatch(char**,char**);	/* variants for -x for cw/bm */
extern void dogre(Parsetype, char*, char*, unsigned char*, PROCFN*, void**, RDFN*, MATCHFN*);
extern void dofgrep(char*, char*, unsigned char*, PROCFN*, void**, RDFN*, MATCHFN*);
extern re_re *egprep(enum Parsetype, unsigned char*, unsigned char*, unsigned char*, int);
extern int greprd(char**, char**);
extern int grepmatch(char**, char**);	/* normal arguments to *find */
extern void inc(char*, char*);
extern void inc_m(char*, char*);	/* increments nmatch */
extern void null(char*, char*);		/* does nothing */
extern void oneshot(char*, char*);	/* increments nmatch, does the longjmp */
extern void pr(char*, char*);
extern void pr_m(char*, char*);
extern int re_lit(re_re*, unsigned char**, unsigned char**);

EXTERN SUCCFN succfn, failfn, succ2fn;
EXTERN re_re *globre;		/* the current re */

#define		MAXLINE		65536

#ifdef c_plusplus
#define UNUSED
#define UNUSED2
#else
#ifdef __cplusplus
#define UNUSED
#define UNUSED2
#else
#define UNUSED unused
#define UNUSED2 unused2
#endif
#endif

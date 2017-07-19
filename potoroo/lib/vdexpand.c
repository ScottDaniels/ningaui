/*
* ======================================================================== v1.0/0a157 
*                                                         
*       This software is part of the AT&T Ningaui distribution 
*	
*       Copyright (c) 2001-2009 by AT&T Intellectual Property. All rights reserved
*	AT&T and the AT&T logo are trademarks of AT&T Intellectual Property.
*	
*       Ningaui software is licensed under the Common Public
*       License, Version 1.0  by AT&T Intellectual Property.
*                                                                      
*       A copy of the License is available at    
*       http://www.opensource.org/licenses/cpl1.0.txt             
*                                                                      
*       Information and Software Systems Research               
*       AT&T Labs 
*       Florham Park, NJ                            
*	
*	Send questions, comments via email to ningaui_support@research.att.com.
*	
*                                                                      
* ======================================================================== 0a229
*/

#include	<stdio.h>
#include	<string.h>

#include	<sfio.h>
#include	"ningaui.h"
#include	"vdelhdr.h"

/*	Expand a squeezed string
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 2/15/95
*/
typedef struct _table_s
{	uchar*		delta;		/* delta string			*/
	uchar*		tar;		/* target data			*/
	int		n_tar;
	K_UDECL(quick,recent,rhere);	/* address caches		*/
} Table_t;

static int
vdunfold(Table_t* tab)
{
	reg int		size, copy;
	reg int		inst, k_type, n;
	reg uchar	*tar, *to, *fr;
	reg int		t, c_addr, n_tar;

	n_tar = tab->n_tar;
	tar = tab->tar;

	for(t = 0, c_addr = 0; t < n_tar; )
	{	inst = STRGETC(tab);
		k_type = K_GET(inst);

		if(!VD_ISCOPY(k_type))
		{	if(K_ISMERGE(k_type))	/* merge/add instruction	*/
				size = A_TGET(inst);
			else if(A_ISHERE(inst))	/* locally coded ADD size	*/
				size = A_LGET(inst);
			else			/* non-local ADD size		*/
			{	STRGETU(tab,size);
				size = A_GET(size);
			}
			if((t+size) > n_tar)	/* out of sync	*/
				return -1;
			c_addr += size;

			/* copy data from the delta stream to target */
			STRREAD(tab,tar+t,size);
			t += size;

			if(K_ISMERGE(k_type))
			{	size = C_TGET(inst);
				k_type -= K_MERGE;
				goto do_copy;
			}
		}
		else
		{	if(C_ISHERE(inst))	/* locally coded COPY size */
				size = C_LGET(inst);
			else
			{	STRGETU(tab,size);
				size = C_GET(size);
			}
		do_copy:
			if((t+size) > n_tar)	/* out of sync */
				return -1;

			if(k_type >= K_QUICK && k_type < (K_QUICK+K_QTYPE) )
			{	copy = STRGETC(tab);
				copy = tab->quick[copy + ((k_type-K_QUICK)<<VD_BITS)];
			}
			else
			{	STRGETU(tab,copy);
				if(k_type >= K_RECENT && k_type < (K_RECENT+K_RTYPE) )
					copy += tab->recent[k_type - K_RECENT];
				else if(k_type == K_HERE)
					copy = c_addr - copy;
				/* else k_type == K_SELF */
			}
			K_UPDATE(tab->quick,tab->recent,tab->rhere,copy);
			c_addr += size;

			if(copy >= t || (copy+size) > n_tar) /* out-of-sync */
				return -1;

			for(;;)	/* allow for copying overlapped data */
			{	if((n = t-copy) > size)
					n = size;
				to = tar+t; fr = tar+copy;
				MEMCPY(to,fr,n);
				t += n;
				if((size -= n) == 0)
					break;
			}
		}
	}

	return 0;
}

int
vdexpand(void* target, int size, void* delta)
{
	reg int	t;
	Table_t	tab;

	/* get true target size */
	tab.tar = (uchar*)target;
	tab.delta = (uchar*)delta;
	STRGETU(&tab,t);
	if(t > size)
		return -1;
	tab.n_tar = t;

	K_INIT(tab.quick,tab.recent,tab.rhere);
	if(vdunfold(&tab) < 0)
		return -1;

	return t;
}

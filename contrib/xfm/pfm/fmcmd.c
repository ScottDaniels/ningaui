/*
* ---------------------------------------------------------------------------
* This source code is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* If this code is modified and/or redistributed, please retain this
* header, as well as any other 'flower-box' headers, that are
* contained in the code in order to give credit where credit is due.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* Please use this URL to review the GNU General Public License:
* http://www.gnu.org/licenses/gpl.txt
* ---------------------------------------------------------------------------
*/
#include <stdio.h>     
#include <stdlib.h>
#include <fcntl.h>    
#include <ctype.h>   
#include <string.h> 
#include <memory.h>
#include <time.h>

#include "../lib/symtab.h"		/* our utilities/tools */
#include "../afileio/afidefs.h"   


#include "fmconst.h"               /* constant definitons */
#include "rfm_const.h"


#include "fmcmds.h"
#include "fmstruct.h"              /* structure definitions */
#include "fmproto.h"

/*
***************************************************************************
*
*   Mnemonic: FMcmd
*   Abstract: This routine is responsible for dispatching the proper
*             routine to handle the command that was input by the user.
*             Commands are those tokins that begin with a period and are 2
*             characters long.
*   Parms:    buf - Pointer to the command. (.aa)
*   Returns:  Nothing.
*   Date:     17 November 1988
*   Author:   E. Scott Daniels
*
*   Modified: 	1-1-89 - To support normal and definition lists
*             	4-30-98- To support user defined list item characters
*             	5-05-89- To support boxes
*             	6-10-89- To add .** as a comment line in the input file
*             	3 May 1991 - To support .ts (two sided) command
*             	4 May 1991 - To support page shifting
*             	5 May 1992 - To support postscript
*             	1 Oct 1992 - To support punctuation space command
*             	2 Nov 1992 - To add center command
*            	13 Nov 1992 - To free/malloc current font buffer
*             	3 Dec 1992 - To flush before setting font size or font
*             	6 Jun 1993 - To set y position for list item bullets higher
*            	21 Feb 1994 - To handle shift value in dlstack rather than old
*                          left margin; correct multi column problem
*                          and to call setstr for running strings
*	 	21 Oct 2007 - Added index interface
**************************************************************************
*/
int FMcmd( char *buf )
{
	int cmd;                  /* command converted to integer for switch */
	int i;                    /* temp integer */
	struct li_blk *liptr;     /* pointer to head to delete on a .el cmd */
	char *ptr;                /* dummy pointer to skip parameters */
	int len;                  /* length of parameter returned */
	int rc = 1;		/* return code indicating command or not */

	cmd = toupper( buf[1] );
	i = toupper( buf[2] );       /* pick off and translate indiv characters */
	cmd = (cmd << 8) + i;        /* combine the characters */

	switch( cmd )
	{
	
		case C_ABORT:	exit( 1 ); break;	/* get out w/o end housekeeping */

		case C_ASIS:	flags2 |= F2_ASIS; break;

		case C_BEGDEFLST:        FMbd( ); break; 	/* definition list */

		case C_BEGLIST:          FMbeglst( ); break;

		case C_BLOCKCTR:		/* .bc {start|end} */
				if( FMgetparm( &buf ) != 0 )
				{
					FMflush( );		/* need to put out last one */
					cenx1 = cur_col->lmar;
					cenx2 = cur_col->lmar + cur_col->width;  /* defalult center */
					if( *buf == 's' || *buf == 'S' )
						flags2 |= F2_CENTER;
					else
						flags2 &= ~F2_CENTER;
				}
				break;

		case C_BOTY:	/* .by [-]x[ip] if - (neg) then thats how far UP from bottom */
				len = FMgetparm( &ptr );
				boty = FMgetpts( ptr, len );
				if( boty <= 0 )
					boty += (11 * 72)-10;
				break;
	
		case C_BREAK:           FMflush( ); break;

		case C_BOX:             FMbox( ); break;

		case C_CAPTURE:		FMcapture( ); break;

		case C_CCOL:            FMccol( 0 ); break;

		case C_TABLECELL:	FMcell( 1 ); break;

		case C_CENTER:          FMcenter( ); break;

		case C_COLOUR:  	/*FMignore( ); break;	 */
				if( FMgetparm( &ptr ) > 0 )
				{
					if( textcolour )
						free( textcolour );
					textcolour = strdup( ptr );
					FMsetcolour( NULL );			/* force change on next flush */
				}
				break;


		case C_COMMA:		FMcomma( ); break;

		case C_COMMENT:         FMignore( ); break;

		case C_DEFHEADER:       FMdefheader( ); break;

		case C_CDEFINE:		FMcd( ); break;

		case C_EP:              FMep( ); break;

		case C_CEJECT:           
				FMflush( );             /* flush what is there... must be done here */
				FMceject( );            /* eject column, flush page if in last column */
				break;

		case C_CPAGE:            FMcpage( ); break;

		case C_DEFDELIM:         /* define the variable definition delimiter */
				if( FMgetparm( &ptr ) != 0 )
				vardelim = ptr[0];    /* set the new pointer */
				break;

		case C_DEFITEM:         FMditem( ); break;

		case C_DEFVAR:          FMdv( ); break;

		case C_DOUBLESPACE:	flags = flags | DOUBLESPACE; break;

		case C_ELSE:		FMelse( ); break;

		case C_ENDDEFLST:        /* end definition list */
				if( dlstackp >= 0 )     /* valid stack pointer? */
				{
					flags2 &= ~F2_DIRIGHT;  /* turn off the right justify flag for di's */
					FMflush( );             
					i = dlstack[dlstackp].indent / PTWIDTH;     /* calc line len shift */
					lmar -= dlstack[dlstackp].indent;           /* shift margin back to left */
					linelen += dlstack[dlstackp].indent;        /* reset line len */
					dlstackp--;                          /* "pop" from the stack */
				}
				break;

		case C_ENDIF:            break; /* .fi encountered - ignore it */

		case C_ENDLIST:          /* end a list */
				if( lilist != NULL )    /* if inside of a list */
				{
					FMflush( );           /* clear anything that is there */
					FMendlist( TRUE );      /* terminate the list and delete the block */
				}
				break;


		case C_ENDTABLE: 	FMendtable( ); break;
	
   		case C_EVAL:                  /* evaluate expression and push result */
    				if( FMgetparm( &buf ) > 0 )  /* get parameter entered */
     					AFIpushtoken( fptr->file, buf );
    				break;

		case C_FIGURE:           FMfigure( ); break;

		case C_FORMAT:           FMformat( ); break;

		case C_GETVALUE:         FMgetval( ); break;

		case C_GREY:             /* set grey scale for fills */
				if( FMgetparm( &buf ) > 0 )  /* get parameter entered */
				fillgrey = atoi( buf );   /* convert it to integer */
				break;

		case C_HDMARG:           FMindent( &hlmar );    break;

		case C_HN:               FMhn( );   break;

		case C_H1:              FMheader( headers[0] ); break;

		case C_H2:               FMheader( headers[1] ); break;

		case C_H3:               FMheader( headers[2] ); break;

		case C_H4:               FMheader( headers[3] ); break;

		case C_HYPHEN:		
				if( FMgetparm( &buf ) > 0 )  /* get parameter entered */
				{
					if( *(buf+1) == 'n' )	/* assume on */
						flags3 |= F3_HYPHEN;
					else
						flags3 &= ~F3_HYPHEN;
				}
				else
					flags3 |= F3_HYPHEN;
				break;

		case C_IF:               FMif( ); break;

		case C_IMBED:            FMimbed( ); break;

		case C_INDENT:           /* user indention of next line */
				FMflush( );             /* force a break then */
				FMindent( &lmar );      /* indent the left margin value */
				break;

		case C_INDEX:
				fmindex( );
				break;

		case C_JUMP: 	FMjump( ); break;

		case C_JUSTIFY:         FMsetjust( ); break;

		
		case C_LINESIZE:        /* set line size for line command */
				if( FMgetparm( &buf ) > 0 )   /* get the parameter */
				{
					linesize = atoi( buf );   /* convert to integer */
					if( linesize > 10 )
						linesize = 2;        /* dont allow them to be crazy */
				}
				break;

		case C_LISTITEM:        /* list item entered */
				if( lilist != NULL && lilist->yindex < 60 )
				{
					FMflush( );          /* output what we have so far */
					if( cury + textspace + textsize  > boty )  /* flush before marking */
					FMceject( );
					lilist->ypos[lilist->yindex] =  (cury + textsize);
					lilist->yindex++;     /* point at next index */
				}
				break;

		case C_LL:                      /* reset line length */
				FMflush( );                   /* terminate previous list item */
				FMll( );
				break;

		case C_LINE:            FMline( );   break;

		case C_ONPAGEEJECT:	FMoneject( ); break;	/* on all eject commands */

		case C_OUTLINE:          /* use true charpath and fill instead of stroke */
				if( FMgetparm( &buf ) > 0 )  /* get the parameter on | off */
				{
					if( toupper( buf[1] ) == 'N' )
						flags2 |= F2_TRUECHAR;        /* turn on the flag */
					else
						flags2 &= ~F2_TRUECHAR;       /* turn off the flag */
				}
				break;

		case C_NOFORMAT:                /* turn formatting off */
				FMflush( );                    /* send last formatted line on its way */
				flags = flags | NOFORMAT;      /* turn no format flag on */
				break;

		case C_PAGE:            /* eject the page now */
				FMflush( );            /* terminate the line in progress */
				FMpflush( );            /* and do the flush */
				break;

		case C_PAGENUM:		FMpgnum( ); break;

		case C_PAGEMAR:         FMindent( &pageshift );  break;

		case C_POP:		FMpop_state( ); break;

		case C_PUNSPACE:        flags2 ^= F2_PUNSPACE; break;
	
		case C_PUSH:		FMpush_state( ); break;

		case C_QUIT:		 AFIclose( fptr->file ); break;

		case C_RFOOT:           FMsetstr( &rfoot, HEADFOOT_SIZE ); break;

		case C_RHEAD:         	FMsetstr( &rhead, HEADFOOT_SIZE ); break;

		case C_SETX:		 FMsetx( ); break;

		case C_SETY:             FMsety( ); break;

   		case C_SHOWV:
				if( (len = FMgetparm( &buf )) > 0 )
				{
					if( strcmp( buf, "all" ) == 0 )
						FMshowvars( );
					else
					{
						if( (ptr = sym_get( symtab, buf, 0 )) )
							fprintf( stderr, "(%s @ %ld) %s = (%s)\n", fptr->name, AFIstat( fptr->file, AFI_OPS, NULL), buf, ptr );
						else
							fprintf( stderr, "(%s @ %ld) %s = UNDEFINED\n", fptr->name, AFIstat( fptr->file, AFI_OPS, NULL), buf );
					}
				}
				break;

		case C_SINGLESPACE:        /* turn off double space */
				if( flags & DOUBLESPACE )
					flags = flags & (255-DOUBLESPACE);
				break;

		case C_SKIP:            
				if( cury == topy )
					break;                 /* if not at top fall into space */

		case C_SPACE:            /* user wants blank lines */
				FMspace( );
				break;

		case C_SETFONT:           /* set font for text (font name only parm) */
				if( (len = FMgetparm( &ptr )) != 0 );     /* if a parameter was entered */
				{
					free( curfont );          
					curfont = strdup( ptr );
					FMfmt_add( );		/* add a format block to the list */
				}
				break;

		case C_SETTXTSIZE:        /* set text font size */
				if( FMgetparm( &ptr ) != 0 )     /* if number entered */
				{
					if( (i = atoi( ptr )) > 5 )    /* if number is ok */
					{
						textsize = i; 
						FMfmt_add( );
					}
				}
				break;

		case C_SMASH:	 flags2 |= F2_SMASH; break;

		case C_TABLE:	 FMtable( ); break;

		case C_TABLEHEADER:	 FMth( ); break;

		case C_TABLEROW:	 FMtr( 0 ); break;

		case C_TMPFONT:	 FMtmpfont( ); break;

		case C_TMPTOP:            FMtmpy( cmd ); break;

		case C_TOC:	 FMtc( ); break;

		case C_TOPMAR:            FMtopmar( ); break;

		case C_TWOSIDE:          flags2 = flags2 ^ F2_2SIDE; break;
		
		case C_TRACE:	if( FMgetparm( &ptr ) != 0 )
				{
					trace = atoi( ptr );
					fprintf( stderr, " trace on=%d: ", trace );
				}
				else
				{
					trace = 0;
					fprintf( stderr, " trace off: ", trace );
				}

				fprintf( stderr, " cury=%d textsize=%d textspace=%d font=%s boty=%d topy=%d col_lmar=%d col_wid=%d\n", cury, textsize, textspace, curfont, boty, topy, cur_col->lmar, cur_col->width );
				break;

		case C_STOPRUN:	rc = 2; break;			/* cause fmrun to pop */

			default:
				rc = 0;			/* indicate to caller we did not process */
				break;
		
	}     /* end switch */

	return rc;

}                  /* FMcmd */


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
****************************************************************************
* **** warning **** this routine is no longer used. see fmsetstr.c
*   Mnemonic:  FMrhead
*   Abstract:  This routine builds a string that is to be used for the
*              running header.
*              that the user enteres.
*   Parms:     None.
*   Returns:   Nothing.
*   Date:      3 December 1988
*   Author:    E. Scott Daniels
*
*   Modified:  22 Apr 1991 - To make header adjustable on the page
*              29 Oct 1992 - To null pointer when removed and to write EOS
*                            inside of the array.
*              27 Nov 1992 - To remove reference to rheadlen
***************************************************************************
*/
FMrhead( )
{
 char *buf;          /* pointer at the token */
 int i;

 if( rhead == NULL )        /* no buffer yet */
  rhead = (char *) malloc( (unsigned) HEADFOOT_SIZE );

 i = FMgetparm( &buf );       /* get parameter */

 if( i == 0 || (strncmp( "off", buf, 3 ) == 0) )
  {
   if( rhead )
    free( rhead );
   rhead = NULL;
   return;
  }

 buf = &inbuf[4];           /* start just past the .rh command */

 for( i = 0; i < HEADFOOT_SIZE-1 && buf[i] != EOS; i++ )
  rhead[i] = buf[i];               /* copy the header */
 rhead[i] = EOS;                   /* terminate the string */

 while( FMgetparm( &buf ) != 0 );       /* clear the input buffer */
}               /* FMrhead */

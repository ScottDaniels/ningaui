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
*****************************************************************************
*
*  Mnemonic: FMline
*  Abstract: This routine is responsible for putting a line into the output
*            file.
*  Parms:    None.
*  Returns:  Nothing.
*  Date:     1 November 1992
*  Author:   E. Scott Daniels
*
*  Modified:  6 Jul 1994 - To convert for rfm.
*            11 Apr 1994 - To check for box to see what margins to use.
*****************************************************************************
*/
void FMline( )
{
 char out[100];  /* output buffer */
 int len;        /* lenght of output string */

 FMflush( );     /* put out what ever is currently cached */

 AFIwrite( ofile, "\\pard" );
 sprintf( out, "\\brdrb\\brdrs\\brdrw%d\\brsp20", linesize * 20 );
 AFIwrite( ofile, out );   /* write the command to draw the line out */

 FMpara( 0, TRUE );    /* terminate the line and define next paragraph */

 /*cury += textsize + linesize; */  /* bump the current spot up some */
 rflags |= RF_PAR;
}           /* FMline */

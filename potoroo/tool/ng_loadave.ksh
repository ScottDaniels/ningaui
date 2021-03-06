#!/ningaui/bin/ksh
# ======================================================================== v1.0/0a157 
#                                                         
#       This software is part of the AT&T Ningaui distribution 
#	
#       Copyright (c) 2001-2009 by AT&T Intellectual Property. All rights reserved
#	AT&T and the AT&T logo are trademarks of AT&T Intellectual Property.
#	
#       Ningaui software is licensed under the Common Public
#       License, Version 1.0  by AT&T Intellectual Property.
#                                                                      
#       A copy of the License is available at    
#       http://www.opensource.org/licenses/cpl1.0.txt             
#                                                                      
#       Information and Software Systems Research               
#       AT&T Labs 
#       Florham Park, NJ                            
#	
#	Send questions, comments via email to ningaui_support@research.att.com.
#	
#                                                                      
# ======================================================================== 0a229


#!/ningaui/bin/ksh
# Mnemonic:	ng_loadave
# Abstract:	Provides a portable means for getting the load average (not all 
#		hosts support an uptime command).
#		
# Date:		30 April 2003 (HBD KT!)
# Author:	E. Scott Daniels
# ---------------------------------------------------------------------------------
CARTULARY=${NG_CARTULARY:-$NG_ROOT/cartulary}     # get standard configuration file
. $CARTULARY

STDFUN=${NG_STDFUN:-$NG_ROOT/lib/stdfun}  # get standard functions
. $STDFUN



# -------------------------------------------------------------------

short=0

while [[ $1 = -* ]]
do
	case $1 in 
		-_)	__=$2; shift;;
		-_*)	__=${1#-?};;

		-s)	short=1;;			# display just the last three numbers

		-v)	verbose=1;;
		--man)	show_man; cleanup 1;;
		-\?)	usage; cleanup 1;;
		-*)	error_msg "unrecognised option: $1"
			usage;
			cleanup 1
			;;
	esac

	shift
done


# get stuff from uptime, otherwise get the load average junk from w (-u not supported everywhere)
if ! uptime 
then
	w|grep load
fi | read stuff
	
# assume the load average string is:  <junk tokens> load average[s]: x.x x.x x.x
if [[ $short -gt 0 ]]
then
	echo ${stuff##*:}			# user wants only the numbers
else
	echo $stuff
fi

cleanup 0

/* ---------- SCD: Self Contained Documentation ------------------- 
&scd_start
&doc_title(ng_loadave:Portable load average)

&space
&synop	ng_loadave

&space
&desc	&This provides the applications with a portable method for determining the 
	current system load average. &This insulates the user programme from understanding
	which local command (usually uptime) is necessary to generate the load averages 
	string.  The current load average information (the information usually written 
	as the first record of information generated by the uptime or w command) is 
	written to the standard output device

&space
&opts	The following options are recognised by &this when placed on the command line:
&begterms
&term	-s : Short form.  Only the three load average numbers are writtten.  
	If not supplied, the entire load average string is generated.
&endterms

&space
&exit	This script always exits with a good return code (0).

&space
&see

&space
&mods
&owner(Scott Daniels)
&lgterms
&term	30 Apr 2003 (sd) : Thin end of the wedge.
&endterms

&scd_end
------------------------------------------------------------------ */

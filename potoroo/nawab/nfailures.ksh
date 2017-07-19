#!/usr/common/ast/bin/ksh
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


#!/usr/common/ast/bin/ksh
# Mnemonic:	nfailures - dig failures from nawab 
# Abstract:	Digs through a nawab expain and lists the failures. Optionally
#		generates the resubmit command(s) to resubmit them
#		
# Date:		Converted to real tool 13 Aug 2003
# Author:	E. Scott Daniels
# ---------------------------------------------------------------------------------
CARTULARY=${NG_CARTULARY:-$NG_ROOT/cartulary}     # get standard configuration file
. $CARTULARY

STDFUN=${NG_STDFUN:-$NG_ROOT/lib/stdfun}  # get standard functions
. $STDFUN


# -------------------------------------------------------------------
ustring="[-a] [-r] [-s jobber-host] [-t sec] [pattern]"
resub=0
all=0
timeout=180
jobber=$srv_Jobber		# where nawab runs
pname=""

while [[ $1 = -* ]]
do
	case $1 in 
		-a)	all=1;;
		-p)	pname=$2; shift;;
		-r)	resub=1;;
		-s)	jobber="$2"; shift;;
		-t)	timeout=$2; shift;;

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

if [[ -z $jobber ]]
then
	log_msg "cannot determine host where nawab runs (no srv_Jobber)."
	cleanup 3
fi

set -o pipefail
ng_nreq -s $jobber -t $timeout explain fail $pname | awk -v all=$all -v resub=$resub '
	function getname( n, 	a )
	{ 
		split( n, a, "(" );
		return a[1];
	}

	/programme not found/ { printf( "the programme is unknown to nawab\n" ); next; }

	/troupe:/ { troupe = getname( $3 ); next; }
	/programme:/ { prog = getname( $3 ); next; }

	/failure/ { 
		if( all || !resub )
			printf("%s %s %s %s\n",  prog, troupe, $3, $4 ); 
		if( all || resub )
			printf( "ng_nreq resubmit %s %s %d\n", prog, troupe, $3+0 );
	}
	END {
		if( ! NR )	
			printf( "\n" );		# keeps grep from returning a bad rc
	}
	' |grep "${1:-.*}" |sort


cleanup $?



/* ---------- SCD: Self Contained Documentation ------------------- 
&scd_start
&doc_title(ng_nfailures:Display Nawab Job Faiures)

&space
&synop	ng_nfailures [-a] [-r] [-s jobber-node] [-t sec] [pattern]


&space
&desc	&This will send a request to &ital(nawab) and interepret the results
	to produce a list of jobs for which failed statuses have been reported
	to &ital(nawab) by the job scheduler. If a pattern is given on the command 
	line, then the failures are limited to job names that match the pattern.

&space
	If the &lit(-r) (resubmit) option is supplied on the command line, then 
	&this will generate the commands necessary to resubmit each of the failed 
	jobs. If the all option (-a) is supplied, then both the failure status and 
	resubmit commands are written to the standard output device. 

&space
&opts	The following options are recognised by &this when placed on the command line:
&begterms
&term	-a : All mode.  Both the resubmit commands and the status messages are written
	to the standard output device. 
&space
&term	-r : Resubmit mode.  Causes the commands necessary to resubmit each failed job
	to be written to the standard output. If not supplied, then the failure status 
	messages generated by &ital(nawab) are written.
&space
&term	-s host : Defines the host where the "jobber" daemons are running. &ital(Nawab) 
	is expeccted to be on this host.  If this option is not defined, then the 
	cartulary variable &lit(srv_Jobber) is used. 
&space
&term	-t sec : Defines a timeout (seconds) that &this will wait for communcaiton back 
	from &ital(nawab). If not defined 180 seconds is used. 
&endterms


&space
&parms	&This allows an optional parameter to be supplied on the command line. This is 
	a pattern used to reduce the amount of output that is displayed.  The pattern 
	is used against the failure messages and &this will produce output only for 
	the failure information that contains the pattern.

&space
&exit	A zero return code indicates that &this was successful in its attempt to 
	query &ital(nawab) and generate output.  When the host on which &ital(nawab)
	executes is busy, response from &ital(nawab) can exceed the timeout value
	that either was supplied or was used as a default.  In either case this time
	out will cause the script to exit with a non-zero (bad) status indicating 
	that some or all of the data that was expected from &ital(nawab) was not 
	receeived.  

&space
&see	&ital(nawab,) &ital(seneschal).

&space
&mods
&owner(Scott Daniels)
&lgterms
&term	13 Aug 2003 (sd) : Conversion from a Q&D tool.
&endterms

&scd_end
------------------------------------------------------------------ */
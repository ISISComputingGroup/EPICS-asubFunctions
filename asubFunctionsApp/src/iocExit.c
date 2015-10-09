/** @file iocExit.c
 *  @author Freddie Akeroyd, STFC (freddie.akeroyd@stfc.ac.uk)
 *  @ingroup asub_functions
 *
 */
#include <string.h>
#include <stdlib.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <menuFtype.h>
#include <errlog.h>
#include <epicsExit.h>

#include <epicsExport.h>

/**
 *  @ingroup asub_functions
 *  @param[in] prec Pointer to aSub record
 */

static long iocExit(aSubRecord *prec) 
{
    long stat = *(long*)prec->a;
    if (stat == 0)
	{
	    epicsExit(EXIT_SUCCESS);
	}
	else
	{
	    epicsExit(EXIT_FAILURE);
	}
    return 0; /* process output links */
}

epicsRegisterFunction(iocExit); /* must also be mentioned in asubFunctions.dbd */

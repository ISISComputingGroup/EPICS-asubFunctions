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
#include <epicsThread.h>

#include <epicsExport.h>

static void exitFunc(void* arg)
{
    long stat = (long)arg;
	epicsThreadSleep(0.1); /* give asub record time to finish */
    if (stat == 0)
	{
	    epicsExit(EXIT_SUCCESS);
	}
	else
	{
	    epicsExit(EXIT_FAILURE);
	}
}
 
/**
 *  @ingroup asub_functions
 *  @param[in] prec Pointer to aSub record
 */
static long iocExit(aSubRecord *prec) 
{
    long stat = *(long*)prec->a;
	epicsThreadCreate("iocExit", epicsThreadPriorityMedium, 
	                  epicsThreadGetStackSize(epicsThreadStackMedium), exitFunc, (void*)stat);
    return 0; /* process output links */
}

epicsRegisterFunction(iocExit); /* must also be mentioned in asubFunctions.dbd */

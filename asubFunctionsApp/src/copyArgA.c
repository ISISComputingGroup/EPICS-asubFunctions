/** @file copyArgA.c
 *  @ingroup asub_functions
 *
 *  It expect the A input to be the waveform data and B to be "NORD" (number of elements)
 *  it set the output length to be one more than the input to ensure NULL termination on copy
 */
#include <string.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <menuFtype.h>
#include <errlog.h>

#include <epicsExport.h>
/**
 *  Copy a character waveform
 *  @ingroup asub_functions
 *  @param[in] prec Pointer to aSub record
 */
static long copyArgA(aSubRecord *prec) 
{
    unsigned n;
	const char* str_in = (const char*)(prec->a); /* waveform CHAR data */
	char* str_out = (char*)(prec->vala); /* waveform CHAR data */
    if (prec->fta != menuFtypeCHAR || prec->fta != prec->ftva)
	{
         errlogPrintf("%s incorrect input type - must be CHAR. ", prec->name);
		 return -1;
	}
	memset(str_out, '\0', prec->nova); /* initialise output with NULL */
    for(n=0; n<prec->nea && n<prec->noa && n<prec->nova; ++n)
    {
        str_out[n] = str_in[n];
    }
    prec->neva = (n+1 < prec->nova ? n+1 : prec->nova); /* +1 ensures a NULL terminator is copied to the next record */
    return 0; /* process output links */
}

epicsRegisterFunction(copyArgA); /* must also be mentioned in asubFunctions.dbd */

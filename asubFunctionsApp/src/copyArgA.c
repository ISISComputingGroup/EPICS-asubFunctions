/** @file copyArgA.c
 *  @author Freddie Akeroyd, STFC (freddie.akeroyd@stfc.ac.uk)
 *  @ingroup asub_functions
 *
 *  Copy a CHAR waveform record into a STRING waveform record. If this is done by
 *  a normal CAPUT the character byte codes are not preserved
 *
 *  It expect the A input to be the waveform data and B to be "NORD" (number of elements)
 *  it write its output to VALA
 */
#include <string.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <menuFtype.h>
#include <errlog.h>

#include <epicsExport.h>
/**
 *  Convert a character waveform into a string waveform
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
         errlogPrintf("%s incorrect input type. ", prec->name);
		 return -1;
	}
    
	memset(str_out, '\0', prec->nova); /* pad with NULL */
    for(n=0; n<prec->noa && n<prec->nea; ++n)
    {
        str_out[n] = str_in[n];
    }
    return 0; /* process output links */
}

epicsRegisterFunction(copyArgA); /* must also be mentioned in asubFunctions.dbd */

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
    unsigned i;
	const char* str_in = (const char*)(prec->a); /* waveform CHAR data */
	char* str_out = (char*)(prec->vala); /* waveform CHAR data */
    if (prec->fta != menuFtypeCHAR || prec->fta != prec->ftva)
	{
         errlogPrintf("%s incorrect input type. ", prec->name);
		 return -1;
	}
    for(i=0; i<prec->noa && i<prec->nova; ++i)
    {
        str_out[i] = str_in[i];
    }
    if (i < prec->nova)
    {
        str_out[i] = '\0';
    }
    prec->neva = i;
    return 0; /* process output links */
}

epicsRegisterFunction(copyArgA); /* must also be mentioned in asubFunctions.dbd */

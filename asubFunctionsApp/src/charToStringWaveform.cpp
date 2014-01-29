#include <string.h>
#include <aSubRecord.h>
#include <registryFunction.h>
#include <epicsExport.h>

static long charToStringWaveform(aSubRecord *prec) 
{
	const char* str_in = (const char*)(prec->a);
    epicsUInt32 len_in = *(epicsUInt32*)(prec->b);
    epicsOldString *str_out = (epicsOldString *)prec->vala;
    if (prec->noa < len_in) // check input space
	{
	    len_in = prec->noa;
	}
//	Maybe could use prec->nea ? Think it is set by db link but not by CA
	if (prec->nova * sizeof(epicsOldString) < len_in) // check output space
	{
	    len_in = prec->nova * sizeof(epicsOldString);
	}
	memcpy(str_out, str_in, len_in);
	epicsUInt32 len_out = 0;
	if (len_in > 0)
	{
		len_out = 1 + (len_in - 1) / sizeof(epicsOldString);
	}
	prec->neva = len_out;
    return 0; /* process output links */
}

extern "C" 
{
epicsRegisterFunction(charToStringWaveform); // must also be mentioned in asubFunctions.dbd
}
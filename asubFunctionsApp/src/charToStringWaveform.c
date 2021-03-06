/** @file charToStringWaveform.c
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
static long charToStringWaveform(aSubRecord *prec) 
{
    int i;
	const char* str_in = (const char*)(prec->a); /* waveform CHAR data */
    epicsUInt32 len_in = *(epicsUInt32*)(prec->b); /* usually NORD from waveform */
    epicsOldString* str_out = (epicsOldString*)prec->vala; /* epicsOldString is typedef for epics fixed length string */ 
	epicsUInt32 len_out = 0;
	epicsUInt32 max_out_bytes = prec->nova * sizeof(epicsOldString);
    if (prec->fta != menuFtypeCHAR || prec->ftb != menuFtypeULONG || prec->ftva != menuFtypeSTRING)
	{
         errlogPrintf("%s incorrect input type. A (CHAR), B (ULONG), VALA (STRING)", prec->name);
		 return -1;
	}
    if (prec->noa < len_in) /* check input space */
	{
	    len_in = prec->noa;
	}
    /*	Maybe could use prec->nea ? Think it is set by db link but not by CA */
	if (max_out_bytes < len_in) /* check output space */
	{
	    len_in = max_out_bytes;
	}
	memcpy((char*)str_out, str_in, len_in);
	if (len_in > 0)
	{
		len_out = 1 + (len_in - 1) / sizeof(epicsOldString);
	}
	/* memset((char*)str_out + len_in, '\0', len_out * sizeof(epicsOldString) - len_in); /* NULL pad rest of last string written to */
	memset((char*)str_out + len_in, '\0', max_out_bytes - len_in); /* NULL pad all unused parts */
#if 0
	for(i=0; i<len_out; ++i)
	{
	    str_out[i][sizeof(epicsOldString)-1] = '\0'; /* ensure each individual string is NULL terminated */
	}
#endif
	prec->neva = len_out;
    return 0; /* process output links */
}

epicsRegisterFunction(charToStringWaveform); /* must also be mentioned in asubFunctions.dbd */

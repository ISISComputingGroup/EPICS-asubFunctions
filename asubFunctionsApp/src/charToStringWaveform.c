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

#include <epicsExport.h>
/**
 *  Convert a character waveform into a string waveform
 *  @ingroup asub_functions
 *  @param[in] prec Pointer to aSub record
 */
static long charToStringWaveform(aSubRecord *prec) 
{
	const char* str_in = (const char*)(prec->a); /* waveform CHAR data */
    epicsUInt32 len_in = *(epicsUInt32*)(prec->b); /* usually NORD from waveform */
    epicsOldString* str_out = (epicsOldString*)prec->vala; /* epicsOldString is typedef for epics fixed length string */ 
	epicsUInt32 len_out = 0;
	epicsUInt32 max_out_bytes = prec->nova * sizeof(epicsOldString);
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
	memset((char*)str_out + len_in, ' ', len_out * sizeof(epicsOldString) - len_in); /* space pad rest of last string written to */
	/* memset((char*)str_out + len_in, ' ', max_out_bytes - len_in); /* space pad all unused parts */
	prec->neva = len_out;
    return 0; /* process output links */
}

epicsRegisterFunction(charToStringWaveform); /* must also be mentioned in asubFunctions.dbd */

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
#include <stdlib.h>
#include <registryFunction.h>
#include <aSubRecord.h>

#include <epicsExport.h>
/**
 *  Convert a character waveform into a string waveform based on a separating character
 *  @ingroup asub_functions
 *  @param[in] prec Pointer to aSub record
 */
static long splitToStringWaveform(aSubRecord *prec) 
{
	const char* str_in = (const char*)(prec->a); /* waveform CHAR data */
	char *str_tmp, *str_ptr;
	int i;
    epicsUInt32 len_in = *(epicsUInt32*)(prec->b); /* usually NORD from waveform */
	epicsOldString* delim_in = (epicsOldString*)(prec->c);
    epicsOldString* str_out = (epicsOldString*)prec->vala; /* epicsOldString is typedef for epics fixed length string */ 
	epicsUInt32 len_out = 0;
    if (prec->noa < len_in) /* check input space */
	{
	    len_in = prec->noa;
	}
    /*	Maybe could use prec->nea ? Think it is set by db link but not by CA */
	str_tmp = strdup(str_in);
	str_ptr = strtok(str_tmp, delim_in[0]);
	for(i = 0; str_ptr != NULL && i < prec->nova; ++i)
	{
	    memset(str_out[i], '\0', sizeof(epicsOldString));
	    strncpy(str_out[i], str_ptr, sizeof(epicsOldString)-1);
	    str_ptr = strtok(NULL, delim_in[0]);
	}
	len_out = i;
	free(str_tmp);
	for(i=len_out+1; i<prec->nova; ++i)
	{
	    memset(str_out[i], '\0', sizeof(epicsOldString));
	}
	prec->neva = len_out;
    return 0; /* process output links */
}

epicsRegisterFunction(splitToStringWaveform); /* must also be mentioned in asubFunctions.dbd */

/** @file charToStringWaveform.c
 *  @author Freddie Akeroyd, STFC (freddie.akeroyd@stfc.ac.uk)
 *  @ingroup asub_functions
 *
 *  Split a CHAR waveform record into a STRING waveform record. 
 *
 *  It expect the A input to be the CHAR aveform data and B to be "NORD" (number of elements) and C to be the string of delimiters
 *  it write its output to VALA
 */
#include <string.h>
#include <stdlib.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <menuFtype.h>
#include <errlog.h>
#include <epicsString.h>

#include <epicsExport.h>
/**
 *  Convert a character waveform into a string waveform based on a separating character
 *  @ingroup asub_functions
 *  @param[in] prec Pointer to aSub record
 */
static long splitToStringWaveform(aSubRecord *prec) 
{
	const char* str_in = (const char*)(prec->a); /* waveform CHAR data */
	char *str_tmp, *str_ptr, *saveptr;
	int i;
    epicsUInt32 len_in = *(epicsUInt32*)(prec->b); /* usually NORD from waveform */
	epicsOldString* delim_in = (epicsOldString*)(prec->c);
    epicsOldString* str_out = (epicsOldString*)(prec->vala); /* epicsOldString is typedef for epics fixed length string */ 
	epicsUInt32 len_out = 0;
    if (prec->fta != menuFtypeCHAR || prec->ftb != menuFtypeULONG || prec->ftc != menuFtypeSTRING || prec->ftva != menuFtypeSTRING)
	{
         errlogPrintf("%s incorrect input type. A (CHAR), B (ULONG), C (STRING), VALA (STRING)", prec->name);
		 return -1;
	}
    if (prec->noa < len_in) /* check input space */
	{
	    len_in = prec->noa;
	}
    /*	Maybe could use prec->nea ? Think it is set by db link but not by CA */
	str_tmp = malloc(1 + len_in);
	strncpy(str_tmp, str_in, len_in);
	str_tmp[len_in] = '\0';
	str_ptr = epicsStrtok_r(str_tmp, delim_in[0], &saveptr);
	for(i = 0; str_ptr != NULL && i < prec->nova; ++i)
	{
	    memset(str_out[i], '\0', sizeof(epicsOldString));
	    strncpy(str_out[i], str_ptr, sizeof(epicsOldString)-1);
	    str_ptr = epicsStrtok_r(NULL, delim_in[0], &saveptr);
	}
	len_out = i;
	free(str_tmp);
	for(i=len_out; i<prec->nova; ++i)
	{
	    memset(str_out[i], '\0', sizeof(epicsOldString));
	}
	prec->neva = len_out;
    return 0; /* process output links */
}

epicsRegisterFunction(splitToStringWaveform); /* must also be mentioned in asubFunctions.dbd */

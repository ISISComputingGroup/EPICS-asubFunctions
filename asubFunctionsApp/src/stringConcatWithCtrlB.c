/** @file stringConcatWithCtrlB.c
 *  @author Freddie Akeroyd, STFC (freddie.akeroyd@stfc.ac.uk)
 *  @ingroup asub_functions
 *
 * expects A, B, C to be strings and D to be either a CHAR or DOUBLE waveform
 * writes a char waveform to VALA
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <menuFtype.h>
#include <errlog.h>

#include <epicsExport.h>
/**
 *  Concat several strings together with a \2 (^B) between each string 
 *  @ingroup asub_functions
 *  @param[in] prec Pointer to aSub record
 */
static long stringConcatWithCtrlB(aSubRecord *prec) 
{
    epicsOldString* name_in = (epicsOldString*)(prec->a);
    epicsOldString* type_in = (epicsOldString*)(prec->b);
    epicsOldString* units_in = (epicsOldString*)(prec->c);
	char* value_in = (char*)(prec->d); 
    char* str_out = (char*)(prec->vala); /* waveform CHAR data */
	epicsUInt32 max_bytes_out = prec->nova;
    epicsUInt32 len_value_in = prec->nod;
    unsigned n = 0, j;
    int free_value_in  = 0;
    if (prec->fta != menuFtypeSTRING || prec->ftb != menuFtypeSTRING || prec->ftc != menuFtypeSTRING || prec->ftva != menuFtypeCHAR)
	{
         errlogPrintf("%s incorrect input type. A (STRING), B (STRING), C (STRING), VALA (CHAR)", prec->name);
		 return -1;
	}
    if (prec->ftd != menuFtypeCHAR && prec->ftd != menuFtypeDOUBLE)
	{
         errlogPrintf("%s incorrect input type. D (CHAR or DOUBLE)", prec->name);
		 return -1;
	}
    if (prec->ftd == menuFtypeDOUBLE)
    {
        value_in = malloc(32);
        sprintf(value_in, "%g", *(double*)prec->d);
        len_value_in = strlen(value_in);
        free_value_in = 1;
    }
    str_out[0] = '\0';
    for(j=0; n<max_bytes_out && j<sizeof(epicsOldString); ++n, ++j)
    {
        if (name_in[0][j] == '\0')
            break;
        str_out[n] = name_in[0][j];
    }
    if (n < max_bytes_out)
    {
        str_out[n++] = '\2';
    }
    for(j=0; n<max_bytes_out && j<sizeof(epicsOldString); ++n, ++j)
    {
        if (type_in[0][j] == '\0')
            break;
        str_out[n] = type_in[0][j];
    }
    if (n < max_bytes_out)
    {
        str_out[n++] = '\2';
    }
    for(j=0; n<max_bytes_out && j<sizeof(epicsOldString); ++n, ++j)
    {
        if (units_in[0][j] == '\0')
            break;
        str_out[n] = units_in[0][j];
    }
    if (n < max_bytes_out)
    {
        str_out[n++] = '\2';
    }
    for(j=0; n<max_bytes_out && j<len_value_in; ++n, ++j)
    {
        if (value_in[j] == '\0')
            break;
        str_out[n] = value_in[j];
    }
    /* NULL pad rest of output waveform if there is unused space */
    for(j = n; j < max_bytes_out; ++j)
    {
        str_out[j] = '\0';
    }
	/* we need to include NULL byte in NEVA - this is so it gets copied across to target char waveform, otherwise the
	   target is not guaranteed to be NULL terminated */
	if (n < max_bytes_out)
	{
        prec->neva = n + 1; /* +1 to make sure a NULL byte is included */
	}
	else
	{
        prec->neva = n;
	}
    if (free_value_in)
    {
        free(value_in);
    }
    return 0; /* process output links */
}

epicsRegisterFunction(stringConcatWithCtrlB); /* must also be mentioned in asubFunctions.dbd */

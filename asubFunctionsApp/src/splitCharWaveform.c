/** @file splitCharWaveform.c
 *  @author Freddie Akeroyd, STFC (freddie.akeroyd@stfc.ac.uk)
 *  @ingroup asub_functions
 *
 *  Splits a CHAR waveform record into separate STRINGs with one per asub output. Number of strings written to VALU. 
 *  It expects the A input to be the CHAR waveform data and B to be "NORD" (number of elements) 
 *  C can be either 1 (split on separator in D) or 2 (split by length in E) and D to be the delimiter
 *  E to be the split length
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
 * get reference to VALA etc. output fields via index. Assumes VALA, VALB etc. are packed sequentially in structure and 
 * of same data type size so can move pointer by fixed amount. Index 0 -> 'VALA' etc.
 */
static epicsOldString* getVal(aSubRecord *prec, int i)
{
    ptrdiff_t step = &(prec->valb) - &(prec->vala);  /* allow for structure padding */
	void** rec_val_ptr = &(prec->vala) + i * step; /* pointer to field in strtucture */
	return (epicsOldString*)(*rec_val_ptr); /* epicsOldString is typedef for epics fixed length string */ 
}

/** get NOVA etc. fields via index. Assumes NOVA, NOVB are consecutive types */
static epicsUInt32 getNov(aSubRecord *prec, int i)
{
    ptrdiff_t step = &(prec->novb) - &(prec->nova);  /* allow for structure padding */
	epicsUInt32* rec_val_ptr = &(prec->nova) + i * step;
	return *rec_val_ptr;
}

/** get NEVA etc. fields via index */
static epicsUInt32* getNev(aSubRecord *prec, int i)
{
    ptrdiff_t step = &(prec->nevb) - &(prec->neva);  /* allow for structure padding */
	epicsUInt32* rec_val_ptr = &(prec->neva) + i * step;
	return rec_val_ptr;
}

/** get FTVA etc. fields via index */
static epicsEnum16 getFtv(aSubRecord *prec, int i)
{
    ptrdiff_t step = &(prec->ftvb) - &(prec->ftva);  /* allow for structure padding */
	epicsEnum16* rec_val_ptr = &(prec->ftva) + i * step;
	return *rec_val_ptr; 
}

static char* process_string(int mode, char* str, const char* delim, int split_len, char** saveptr, char** memptr)
{
    char* tmpstr;
	size_t n;
    if (mode == 1)
	{
	    return epicsStrtok_r(str, delim, saveptr);
	}
	else if (mode == 2)
	{
	    if (split_len == 0)
		{
		    return NULL;
		}
	    if (str != NULL) /* first call */
		{
		    tmpstr = (char*)malloc(split_len + 1);
			*memptr = tmpstr;
			*saveptr = str;
		}
		else
		{
		    tmpstr = *memptr;
		}
		n = strlen(*saveptr);
		if (n == 0)
		{
		    free(tmpstr);
			*memptr = NULL;
		    return NULL;
		}
		
		strncpy(tmpstr, *saveptr, split_len);
		tmpstr[split_len] = '\0';

		if (n > split_len)
		{
		    *saveptr += split_len;
		}
		else
		{
		    *saveptr += n;
		}
		return tmpstr;
	}
	else
	{
	    return NULL;
	}
} 
 /**
 *  Convert a character waveform into a separate strings based on a separating character
 *  @ingroup asub_functions
 *  @param[in] prec Pointer to aSub record
 */
static long splitCharWaveform(aSubRecord *prec)
{
	const char* str_in = (const char*)(prec->a); /* waveform CHAR data */
	char *str_tmp, *str_ptr, *saveptr, *memptr;
	int i;

    epicsUInt32 len_in = *(epicsUInt32*)(prec->b); /* usually NORD from input char waveform */
	epicsUInt32 mode = *(epicsUInt32*)(prec->c);
	epicsOldString* delim_in = (epicsOldString*)(prec->d);
	epicsUInt32 split_len = *(epicsUInt32*)(prec->e);
	epicsOldString* str_out;
	epicsUInt32 len_out = 0;

    if (prec->fta != menuFtypeCHAR || prec->ftb != menuFtypeULONG || prec->ftc != menuFtypeULONG || 
	       prec->ftd != menuFtypeSTRING || (mode == 2 && prec->fte != menuFtypeULONG) || prec->ftvu != menuFtypeULONG)
	{
         errlogPrintf("%s incorrect input type. Should be FTA (CHAR), FTB (ULONG), FTC (ULONG), FTD (STRING), FTE (ULONG), FTVU (ULONG)", prec->name);
		 return -1;
	}

    if (prec->noa < len_in) /* check input space */
	{
	    len_in = prec->noa;
	}

    /*	Maybe could use prec->nea ? Think it is set by db link but not by CA */
	str_tmp = (char*)malloc(1 + len_in);
	strncpy(str_tmp, str_in, len_in);
	str_tmp[len_in] = '\0';
	
	str_ptr = process_string(mode, str_tmp, delim_in[0], split_len, &saveptr, &memptr);
	
    /* output split strings to A -> T (index 0->19) with number of strings in VALU */ 
	for(i = 0; str_ptr != NULL && i < 20; ++i)
	{
	    if (getFtv(prec, i) != menuFtypeSTRING)
		{
             errlogPrintf("%s incorrect output type FTV%c != STRING", prec->name, 'A' + i);
		     return -1;
		}
		if (getNov(prec, i) != 1)
		{
             errlogPrintf("%s incorrect output size NOV%c != 1", prec->name, 'A' + i);
		     return -1;
		}
		
		str_out = getVal(prec, i);
	    memset(str_out[0], '\0', sizeof(epicsOldString));
	    strncpy(str_out[0], str_ptr, sizeof(epicsOldString)-1);
		*(getNev(prec, i)) = 1;
	    str_ptr = process_string(mode, NULL, delim_in[0], split_len, &saveptr, &memptr);
	}

	len_out = i;
	free(str_tmp);

	/* NULL out any remaining string outputs */
	for(i=len_out; i<20; ++i)
	{
	    if (getFtv(prec, i) == menuFtypeSTRING)
		{
		    str_out = getVal(prec, i);
	        memset(str_out[0], '\0', sizeof(epicsOldString));
		}
		*(getNev(prec, i)) = 0;
	}

	*(epicsUInt32*)(prec->valu) = len_out;
	prec->nevu = 1;

    return 0; /* process output links */
}

epicsRegisterFunction(splitCharWaveform); /* must also be mentioned in asubFunctions.dbd */

/** @file copyArgs.c
 *  @author Freddie Akeroyd, STFC (freddie.akeroyd@stfc.ac.uk)
 *  @ingroup asub_functions
 *
 * copy all arguments from out top in of asubrecord
 */
#include <string.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <menuFtype.h>
#include <errlog.h>

#include <epicsExport.h>

/**
 * calculate size in bytes of a menuFType type
 */
static size_t menuFTypeSize(menuFtype type)
{
    switch(type)
	{
		case menuFtypeCHAR:
		case menuFtypeUCHAR:
		    return 1;
			
		case menuFtypeSHORT:
		case menuFtypeUSHORT:
		case menuFtypeENUM:
		    return 2;
			
		case menuFtypeLONG:
		case menuFtypeULONG:
		case menuFtypeFLOAT:
			return 4;
			
	    case menuFtypeDOUBLE:
			return 8;

		case menuFtypeSTRING:
			return sizeof(epicsOldString);

		default:
			return -1;
 
	}
}

/**
 * copy an individual aSub argument by directly indexing into structure
 * (i == 0) -> 'A', (arg == 1) -> 'B' etc.
 */ 
static long copyArg(aSubRecord *prec, int arg)
{
    size_t tsize;
    char let = 'A' + arg;
    void* val_in = (char*)prec->a + arg * sizeof(prec->a);
    void* val_out = (char*)prec->vala + arg * sizeof(prec->vala);
    epicsEnum16 ft_in = *(&(prec->fta) + arg * sizeof(prec->fta));
    epicsEnum16 ft_out = *(&(prec->ftva) + arg * sizeof(prec->ftva));
 	epicsUInt32 no_in = *(&(prec->noa) + arg * sizeof(prec->noa));
	epicsUInt32 no_out = *(&(prec->nova) + arg * sizeof(prec->nova));
	epicsUInt32* p_ne_in = (&(prec->nea) + arg * sizeof(prec->nea));
	epicsUInt32* p_ne_out = (&(prec->neva) + arg * sizeof(prec->neva));
    if (ft_in != ft_out)
	{
         errlogPrintf("%s FT%c and FTV%c do not match. ", prec->name, let, let);
		 return -1;
	}
    if (no_in != no_out)
	{
         errlogPrintf("%s NO%c and NOV%c do not match. ", prec->name, let, let);
		 return -1;
	}
	tsize = menuFTypeSize(ft_in);
	if (tsize <= 0)
	{
         errlogPrintf("%s FT%c is invalid type code %d. ", prec->name, let, (int)ft_in);
		 return -1;
	}
	memcpy(val_out, val_in, no_in * tsize);
    *p_ne_out = *p_ne_in;	
	return 0;
} 
 
 
/**
 *  Convert a character waveform into a string waveform
 *  @ingroup asub_functions
 *  @param[in] prec Pointer to aSub record
 */
static long copyArgs(aSubRecord *prec) 
{
    int i;
	/* aSubRecord has 21 arguments, 'A' to 'U' */
    for(i = 0; i<21; ++i)
	{
		if (copyArg(prec, i) != 0)
		{
			return -1;
		}
	}
    return 0; /* process output links */
}

epicsRegisterFunction(copyArgs); /* must also be mentioned in asubFunctions.dbd */

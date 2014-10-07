/** @file genData.c
 *  @author Freddie Akeroyd, STFC (freddie.akeroyd@stfc.ac.uk)
 *  @ingroup asub_functions
 *
 *  generate an array of data based on cirteria in an input string
 */
#include <string.h>
#include <stdio.h>
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
static long genData(aSubRecord *prec) 
{
	epicsOldString* str_in = (epicsOldString*)(prec->a); 
    double* vals_out = (double*)(prec->vala); 
	epicsUInt32* len_vals_out = (epicsUInt32*)(prec->valb);
	double start, step, val;
	int i, nvals;
    if (prec->fta != menuFtypeSTRING || prec->ftva != menuFtypeDOUBLE || prec->ftvb != menuFtypeULONG) 
	{
         errlogPrintf("%s incorrect input type. A (STRING), VALA (DOUBLE), VALB (ULONG)", prec->name);
		 return -1;
	}
	if ( 3 != sscanf(*str_in, "%lf;%lf;%d", &start, &step, &nvals) )
	{
         errlogPrintf("%s incorrect string (not in format start;step;nvals). %s", prec->name, *str_in);
		 return -1;
	}
    if (prec->nova < nvals) 
	{
         errlogPrintf("%s not enough space. %d > %d", prec->name, nvals, prec->nova);
		 return -1;
	}
	val = start;
	for(i=0; i<nvals; ++i)
	{
	    vals_out[i] = val;
		val += step;
	}
	*len_vals_out = nvals;
	prec->neva = nvals;
	prec->nevb = 1;
    return 0; /* process output links */
}

epicsRegisterFunction(genData); /* must also be mentioned in asubFunctions.dbd */

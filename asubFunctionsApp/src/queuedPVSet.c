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

#include "queuedPVSetImpl.h"

#include <epicsExport.h>
/**
 *  Concat several strings together with a \2 (^B) between each string 
 *  @ingroup asub_functions
 *  @param[in] prec Pointer to aSub record
 */
static long queuedPVSet(aSubRecord *prec) 
{
    char* pv_in = (char*)(prec->a); /* waveform CHAR data */
    char* value_in = (char*)(prec->b); /* waveform CHAR data */
	epicsFloat64 timeout = *(epicsFloat64*)(prec->c);
    epicsUInt32 max_len_pv_in = prec->noa;
    epicsUInt32 max_len_value_in = prec->nob;
    epicsUInt32 len_pv_in = prec->nea;
    epicsUInt32 len_value_in = prec->neb;
    if (prec->fta != menuFtypeCHAR || prec->ftb != menuFtypeCHAR || prec->ftc != menuFtypeDOUBLE)
	{
         errlogPrintf("%s incorrect input type for queuedPVSet. A (CHAR), B (CHAR), C (DOUBLE)\n", prec->name);
		 return -1;
	}
	return queuedPVSetImpl(prec->name, pv_in, len_pv_in, max_len_pv_in, value_in, max_len_value_in, len_value_in, timeout);
}

epicsRegisterFunction(queuedPVSet); /* must also be mentioned in asubFunctions.dbd */

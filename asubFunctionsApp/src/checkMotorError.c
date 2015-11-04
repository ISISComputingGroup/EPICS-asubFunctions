/** @file setFunctions.c
 *  @author Freddie Akeroyd, STFC (freddie.akeroyd@stfc.ac.uk)
 *  @ingroup asub_functions
 *
 */
#include <string.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <menuFtype.h>
#include <errlog.h>
#include <alarm.h>
#include <epicsStdio.h>

#include <epicsExport.h>

/**
 *  @ingroup asub_functions
 *  @param[in] prec Pointer to aSub record
 */
 
static long checkMotorError(aSubRecord *prec) 
{
    char buffer[512];
    epicsUInt32 msta = *(epicsUInt32*)(prec->a); 
    epicsInt16 lvio = *(epicsInt16*)(prec->b); 
    epicsInt16 hls = *(epicsInt16*)(prec->c); 
    epicsInt16 lls = *(epicsInt16*)(prec->d); 
    epicsEnum16 stat = *(epicsEnum16*)(prec->e); 
    epicsEnum16 sevr = *(epicsEnum16*)(prec->f); 
	epicsOldString* str_out = (epicsOldString*)prec->vala;
    char* output = (char*)str_out;
    if (prec->fta != menuFtypeULONG || prec->ftb != menuFtypeSHORT || 
	     prec->ftc != menuFtypeSHORT || prec->ftd != menuFtypeSHORT ||
		 prec->fte != menuFtypeUSHORT || prec->ftf != menuFtypeUSHORT || prec->ftva != menuFtypeSTRING)
	{
         errlogPrintf("%s incorrect input type. A (ULONG), VALA (CHAR)", prec->name);
		 return -1;
	}
    buffer[0] = '\0';
	if ( (sevr != epicsSevNone) && (stat != epicsAlarmNone) )
	{
/*	    epicsSnprintf(buffer, sizeof(buffer), "ALARM %s %s", epicsAlarmSeverityStrings[sevr], epicsAlarmConditionStrings[stat]); */
	    epicsSnprintf(buffer, sizeof(buffer), "ALARM ");
	}
	if ( (msta & (1 << 6)) || (msta & (1 << 9)) || (msta & (1 << 12)) )
	{
	    strcat(buffer, "MOTOR_ERROR ");
	}
	if ( (lvio == 1) || (hls != 0) || (lls != 0) || (msta & (1 << 2)) || (msta & (1 << 13)) )
	{
	    strcat(buffer, "AT_LIMIT ");	    
	}
	strncpy(output, buffer, sizeof(epicsOldString));
	output[sizeof(epicsOldString) - 1] = '\0';
	prec->neva = 1;
    return 0; /* process output links */
}

epicsRegisterFunction(checkMotorError); /* must also be mentioned in asubFunctions.dbd */

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

#include <epicsExport.h>

/**
 *  @ingroup asub_functions
 *  @param[in] prec Pointer to aSub record
 */
static long displayMotorMSTA(aSubRecord *prec) 
{
    char buffer[512];
    epicsUInt32 msta = *(epicsUInt32*)(prec->a); 
    char* msta_display = (char*)prec->vala; 
	int n, max_len = prec->nova;
    if (prec->fta != menuFtypeULONG || prec->ftva != menuFtypeCHAR)
	{
         errlogPrintf("%s incorrect input type. A (ULONG), VALA (CHAR)", prec->name);
		 return -1;
	}
	buffer[0] = '\0';
	strcat(buffer, ((msta & 0x1) ? "LAST_RAW_DIR: POS, " : "LAST_RAW_DIR: NEG, ") );
	strcat(buffer, ((msta & 0x2) ? "MOT_DONE: YES, " : "MOT_DONE: NO, ") );
	strcat(buffer, ((msta & 0x4) ? "PLUS_LS, " : "") );
	strcat(buffer, ((msta & 0x8) ? "HOMELS: YES " : "") );
/*	strcat(buffer, ((msta & 0x10) ? "DONE: YES" : "DONE: NO") ); not used*/
	strcat(buffer, ((msta & 0x20) ? "POS: closed_loop, " : "") );
	strcat(buffer, ((msta & 0x40) ? "SLIP_STALL, " : "") );
	strcat(buffer, ((msta & 0x80) ? "AT_HOME, " : "") );
	strcat(buffer, ((msta & 0x100) ? "ENCODER: YES, " : "ENCODER: NO, ") );
	strcat(buffer, ((msta & 0x200) ? "PROBLEM, " : "") );
	strcat(buffer, ((msta & 0x400) ? "MOVING: YES, " : "MOVING: NO, ") );
	strcat(buffer, ((msta & 0x800) ? "GAIN_SUPPORT, " : "") );
	strcat(buffer, ((msta & 0x1000) ? "COMM_ERR, " : "") );
	strcat(buffer, ((msta & 0x2000) ? "MINUS_LS, " : "") );
	strcat(buffer, ((msta & 0x4000) ? "HOMED: YES" : "HOMED: NO") );
	n = strlen(buffer);
	strncpy(msta_display, buffer, max_len);
	if (n >= max_len)
	{
		msta_display[max_len-1] = '\0';
		prec->neva = max_len - 1;
	}
	else
	{
	    memset(msta_display + n, '\0', max_len - n); 
		prec->neva = n;
	}
    return 0; /* process output links */
}

epicsRegisterFunction(displayMotorMSTA); /* must also be mentioned in asubFunctions.dbd */

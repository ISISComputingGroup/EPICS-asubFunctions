/** @file setFunctions.c
 *  @author Freddie Akeroyd, STFC (freddie.akeroyd@stfc.ac.uk)
 *  @ingroup asub_functions
 *
 */
#include <string.h>
#include <stdio.h>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <menuFtype.h>
#include <errlog.h>

#include <epicsExport.h>

typedef enum {GALIL,GALIL_SIMPLE,MCLEN,MCLEN_SIMPLE} statusType;

static size_t generateMclenMstaDisplayString(epicsUInt32 msta, char* msta_display, int max_len) {
    char buffer[512];
	buffer[0] = '\0';
	strcat(buffer, ((msta & 128) ? "BUSY, " : "IDLE, ") );
	strcat(buffer, ((msta & 64) ? "ERROR, " : "") );
	strcat(buffer, ((msta & 32) ? "UPPER LIM, " : "") );
	strcat(buffer, ((msta & 16) ? "LOWER LIM, " : "") );
	strcat(buffer, ((msta & 8) ? "JOGGING, " : "") );
	strncpy(msta_display, buffer, max_len);
	return strlen(buffer);
}

static size_t generateSimpleMclenMstaDisplayString(epicsUInt32 msta, char* msta_display, int max_len) {
    char buffer[512];
	buffer[0] = '\0';
	strcat(buffer, (msta & 128) ? "BUSY" : "IDLE");
	strncpy(msta_display, buffer, max_len);
	return strlen(buffer);
}

static size_t generateGalilMstaDisplayString(epicsUInt32 msta, char* msta_display, int max_len) {
    char buffer[512];
	buffer[0] = '\0';
	strcat(buffer, ((msta & 0x400) ? "MOVING, " : "") );
	strcat(buffer, ((msta & 0x2) ? "DONE, " : "") );
	strcat(buffer, ((msta & 0x40) ? "SLIP_STALL, " : "") );
	strcat(buffer, ((msta & 0x200) ? "PROBLEM, " : "") );
	strcat(buffer, ((msta & 0x1000) ? "COMM_ERR, " : "") );
	strcat(buffer, ((msta & 0x2000) ? "MINUS_LS, " : "") );
	strcat(buffer, ((msta & 0x8) ? "HOMELS, " : "") );
	strcat(buffer, ((msta & 0x4) ? "PLUS_LS, " : "") );
	strcat(buffer, ((msta & 0x80) ? "AT_HOME, " : "") );
	strcat(buffer, ((msta & 0x800) ? "GAIN_SUPPORT, " : "") );
	strcat(buffer, ((msta & 0x1) ? "DIR: POS, " : "DIR: NEG, ") );
/*	strcat(buffer, ((msta & 0x10) ? "DONE: YES, " : "DONE: NO, ") ); not used*/
	strcat(buffer, ((msta & 0x100) ? "ENCODER: YES, " : "ENCODER: NO, ") );
	strcat(buffer, ((msta & 0x4000) ? "HOMED: YES, " : "HOMED: NO, ") );
	strcat(buffer, ((msta & 0x20) ? "POS: closed_loop, " : "") );
	strncpy(msta_display, buffer, max_len);
	return strlen(buffer);
}

static size_t generateSimpleGalilMstaDisplayString(epicsUInt32 msta, char* msta_display, int max_len) {
    char buffer[512];
	buffer[0] = '\0';
	if (msta & 0x400) // moving
	{
		strcat(buffer, (msta & 0x1) ? "MOVING +" : "MOVING -");
	}
	else if (msta & 0x2) // done
	{
		strcat(buffer, "STATIONARY");
	}
	strcat(buffer, ((msta & 0x40) ? " (SLIP_STALL)" : "") );
//	strcat(buffer, ((msta & 0x200) ? " (PROBLEM)" : "") );
//	strcat(buffer, ((msta & 0x1000) ? " (COMM_ERR)" : "") );
	strncpy(msta_display, buffer, max_len);
	return strlen(buffer);
}

static size_t generateDefaultMstaDisplayString(epicsUInt32 msta, char* msta_display, int max_len) {
    char buffer[512];
	char temp[512];
	buffer[0] = '\0';
	sprintf(temp, "%d", msta);
	strcat(buffer,temp);
	return strlen(buffer);
}

/**
 *  @ingroup asub_functions
 *  @param[in] prec Pointer to aSub record
 */
static long displayMotorMSTA(aSubRecord *prec, statusType type) 
{
    epicsUInt32 msta = *(epicsUInt32*)(prec->a); 
    char* msta_display = (char*)prec->vala; 
	int max_len = prec->nova;
	size_t n;
    if (prec->fta != menuFtypeULONG || prec->ftva != menuFtypeCHAR)
	{
         errlogPrintf("%s incorrect input type. A (ULONG), VALA (CHAR)", prec->name);
		 return -1;
	}
	switch(type) {
		case GALIL :
	        n = generateGalilMstaDisplayString(msta,msta_display,max_len);	
			break;
		case GALIL_SIMPLE :
	        n = generateSimpleGalilMstaDisplayString(msta,msta_display,max_len);	
			break;
		case MCLEN :
	        n = generateMclenMstaDisplayString(msta,msta_display,max_len);	
			break;
		case MCLEN_SIMPLE :
	        n = generateSimpleMclenMstaDisplayString(msta,msta_display,max_len);	
			break;
		default :
			n = generateDefaultMstaDisplayString(msta,msta_display,max_len);
	}
	if (n >= max_len)
	{
		msta_display[max_len-1] = '\0';
		prec->neva = max_len;
	}
	else
	{
	    memset(msta_display + n, '\0', max_len - n); 
		prec->neva = (int)n + 1; // n+1 to make sure '\0' is included
	}
    return 0; /* process output links */
}

static long displayGalilMotorMSTA(aSubRecord *prec) 
{
	return displayMotorMSTA(prec,GALIL);
}

static long displayGalilMotorMSTASimple(aSubRecord *prec) 
{
	return displayMotorMSTA(prec,GALIL_SIMPLE);
}

static long displayMclenMotorMSTA(aSubRecord *prec) 
{
	return displayMotorMSTA(prec,MCLEN);
}

static long displayMclenMotorMSTASimple(aSubRecord *prec) 
{
	return displayMotorMSTA(prec,MCLEN_SIMPLE);
}

epicsRegisterFunction(displayGalilMotorMSTA); /* must also be mentioned in asubFunctions.dbd */
epicsRegisterFunction(displayGalilMotorMSTASimple); /* must also be mentioned in asubFunctions.dbd */
epicsRegisterFunction(displayMclenMotorMSTA); /* must also be mentioned in asubFunctions.dbd */
epicsRegisterFunction(displayMclenMotorMSTASimple); /* must also be mentioned in asubFunctions.dbd */

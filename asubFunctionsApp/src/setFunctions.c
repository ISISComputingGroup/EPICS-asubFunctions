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

#include "setFunctions.h"

/**
 *  @ingroup asub_functions
 *  @param[in] prec Pointer to aSub record
 */
static long addToSet(aSubRecord *prec) 
{
    epicsOldString* set_name = (epicsOldString*)prec->a; /* epicsOldString is typedef for epics fixed length string */ 
	const char* item_value = (const char*)(prec->b); /* waveform CHAR data */
    epicsUInt32 max_len_item_value = *(epicsUInt32*)(prec->c); /* usually NORD from waveform */
    if (prec->fta != menuFtypeSTRING || prec->ftb != menuFtypeCHAR || prec->ftc != menuFtypeULONG)
	{
         errlogPrintf("%s incorrect input type. A (STRING), B (CHAR), C (ULONG)", prec->name);
		 return -1;
	}
    if (prec->nob < max_len_item_value) /* check input space - not sure ->neb always gets set */
	{
	    max_len_item_value = prec->nob;
	}
	addToSetImpl(*set_name, item_value, max_len_item_value);
    return 0; /* process output links */
}

static long removeFromSet(aSubRecord *prec) 
{
    epicsOldString* set_name = (epicsOldString*)prec->a; /* epicsOldString is typedef for epics fixed length string */ 
	const char* item_value = (const char*)(prec->b); /* waveform CHAR data */
    epicsUInt32 max_len_item_value = *(epicsUInt32*)(prec->c); /* usually NORD from waveform */
    if (prec->fta != menuFtypeSTRING || prec->ftb != menuFtypeCHAR || prec->ftc != menuFtypeULONG)
	{
         errlogPrintf("%s incorrect input type. A (STRING), B (CHAR), C (ULONG)", prec->name);
		 return -1;
	}
    if (prec->nob < max_len_item_value) /* check input space - not sure ->neb always gets set */
	{
	    max_len_item_value = prec->nob;
	}
	removeFromSetImpl(*set_name, item_value, max_len_item_value);
    return 0; /* process output links */
}

static long getSetItemCount(aSubRecord *prec)
{
    epicsOldString* set_name = (epicsOldString*)prec->a; /* epicsOldString is typedef for epics fixed length string */ 
    int item_count = 0;
    if (prec->fta != menuFtypeSTRING || prec->ftva != menuFtypeULONG)
	{
         errlogPrintf("%s incorrect input type. A (STRING), VALA (ULONG)", prec->name);
		 return -1;
	}
    getSetItemCountImpl(*set_name, &item_count);
    *(epicsUInt32*)prec->vala = item_count;
    prec->neva = 1;
    return 0; /* process output links */
}

static long getSetItems(aSubRecord *prec) 
{
    int item_count = 0;
    epicsOldString* set_name = (epicsOldString*)prec->a; /* epicsOldString is typedef for epics fixed length string */ 
	char* item_value = (char*)(prec->vala); /* waveform CHAR data */
    if (prec->fta != menuFtypeSTRING || prec->ftva != menuFtypeCHAR || prec->ftvb != menuFtypeULONG)
	{
         errlogPrintf("%s incorrect input type. A (STRING), VALA (CHAR), VALB (ULONG)", prec->name);
		 return -1;
	}
	getSetItemsImpl(*set_name, item_value, prec->nova, &(prec->neva), &item_count);
    *(epicsUInt32*)prec->valb = item_count;
    prec->nevb = 1;
    return 0; /* process output links */
}

epicsRegisterFunction(addToSet); /* must also be mentioned in asubFunctions.dbd */
epicsRegisterFunction(removeFromSet); /* must also be mentioned in asubFunctions.dbd */
epicsRegisterFunction(getSetItems); /* must also be mentioned in asubFunctions.dbd */
epicsRegisterFunction(getSetItemCount); /* must also be mentioned in asubFunctions.dbd */

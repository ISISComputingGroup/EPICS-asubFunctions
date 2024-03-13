//
// @file parseJSON.cpp
// @ingroup asub_functions
//

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <string>
#include <algorithm>
#include <registryFunction.h>
#include <aSubRecord.h>
#include <menuFtype.h>
#include <errlog.h>
#include <epicsString.h>

#include <boost/algorithm/string.hpp>

#include <epicsExport.h>

#include "convertUNIXTime.h"

long convertUNIXTimeImpl(double secs_double, const char* format, bool utc, char* outstr, size_t len_outstr)
{
    char buffer[128], buffer2[128];
    time_t secs_int = (time_t)secs_double;
    int ms = 1000.0 * (secs_double - secs_int);
    struct tm *ptm = (utc ? gmtime(&secs_int) : localtime(&secs_int));
    strftime(buffer, sizeof(buffer), format, ptm);
    snprintf(buffer2, sizeof(buffer2), buffer, ms);
    strncpy(outstr, buffer2, len_outstr);
    return 0;    
}

// INPA - time
// INPB - format
// INPC - 0 = localtime, 1 = utc
// OUTA - time string
// if format includes a %%d (i.e. an escaped %d) it will get millseconds
// e.g. ISO 8601 with milliseconds "%Y%m%dT%H%M%S.%%d"
static long convertUNIXTime(aSubRecord *prec)
{
    if (prec->fta != menuFtypeDOUBLE) {
         errlogPrintf("%s incorrect input type. Should be FTA (DOUBLE)", prec->name);
         return -1;
    }
    if (prec->ftb != menuFtypeSTRING) {
         errlogPrintf("%s incorrect input type. Should be FTB (STRING)", prec->name);
         return -1;
    }
    if (prec->ftc != menuFtypeDOUBLE) {
         errlogPrintf("%s incorrect input type. Should be FTC (DOUBLE)", prec->name);
         return -1;
    }
    if (prec->ftva != menuFtypeSTRING) {
         errlogPrintf("%s incorrect output type. Should be FTVA (STRING)", prec->name);
         return -1;
    }
    double secs_double = *(double*)(prec->a);
    const char* format = *(epicsOldString*)prec->b;
    bool utc = (*(double*)(prec->c) != 0.0 ? true : false);
    char* outstr = *(epicsOldString*)prec->vala;
    return convertUNIXTimeImpl(secs_double, format, utc, outstr, sizeof(epicsOldString));
}

extern "C" {
    epicsRegisterFunction(convertUNIXTime); /* must also be mentioned in asubFunctions.dbd */
}

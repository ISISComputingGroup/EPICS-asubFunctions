// @file parseJSON.cpp
//  @author Freddie Akeroyd, STFC (freddie.akeroyd@stfc.ac.uk)
//  @ingroup asub_functions
//
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
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <epicsExport.h>

#include <parseJSON.h>

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

static json getJSONValueRecurse(const json& j, const std::vector<std::string>& path_bits, int level)
{
    if (level < path_bits.size()) {
        if (j.is_array()) {
            return getJSONValueRecurse(j[atoi(path_bits[level].c_str())], path_bits, level + 1);
        } else {
            return getJSONValueRecurse(j[path_bits[level]], path_bits, level + 1);
        }
    } else {
        return j;
    }
}

json getJSONValue(const json& j, const std::string& path)
{
	std::vector<std::string> path_bits;
	boost::algorithm::split(path_bits, path, boost::is_any_of("/"), boost::token_compress_on);
    auto r = std::remove(path_bits.begin(), path_bits.end(), "");
    path_bits.erase(r, path_bits.end());     
	return getJSONValueRecurse(j, path_bits, 0);
}

json getJSONValue(const std::string& json_str, const std::string& path)
{
    json j(json_str);
	return getJSONValue(j, path);
}

 /**
 *  Convert a character waveform into a separate strings based on a separating character
 *  @ingroup asub_functions
 *  @param[in] prec Pointer to aSub record
 */
static long parseJSON(aSubRecord *prec)
{
    std::string json_in((const char*)(prec->a)); /* waveform CHAR data */
    std::string path_in((const char*)(prec->b)); /* waveform CHAR data */
	char *str_tmp, *str_ptr, *saveptr, *memptr;
	int i;

    epicsUInt32 len_in = *(epicsUInt32*)(prec->b); /* usually NORD from input char waveform */
	epicsUInt32 mode = *(epicsUInt32*)(prec->c);
	epicsOldString* delim_in = (epicsOldString*)(prec->d);
	epicsUInt32 split_len = *(epicsUInt32*)(prec->e);
	epicsOldString* str_out;
	epicsUInt32 len_out = 0;

    json value = getJSONValue(json_in, path_in);
    if (value.is_array()) {
        1;
    }
        
    if (prec->fta != menuFtypeCHAR || prec->ftb != menuFtypeCHAR || prec->ftc != menuFtypeULONG || 
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

extern "C" {
epicsRegisterFunction(parseJSON); /* must also be mentioned in asubFunctions.dbd */
}
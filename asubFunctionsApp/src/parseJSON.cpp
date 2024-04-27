/*************************************************************************\ 
* Copyright (c) 2024 Science and Technology Facilities Council (STFC), GB. 
* All rights reverved. 
* This file is distributed subject to a Software License Agreement found 
* in the LICENSE file that is included with this distribution. 
\*************************************************************************/ 

//
// define parseJSON function to be called from an EPICS aSub record
// to parse JSON and extract elements as per supplied paths
// is able to return scalars and arrays from JSON into DB records
// see documentation of function below
//
// Function was originally developed for parsing JSON returned
// by devices supporting the Sample Environment Communication Protocol (SECoP)
// designed by the International Society for Sample Environment (ISSE)
// See https://sampleenvironment.github.io/secop-site/
//
// For examples of usage see parseJSON_tests.cc in the tests directory
//
// For an example of usage in an EPICS aSub record
// see https://github.com/ISISComputingGroup/he3nmr
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

// calculate reference to field index i of a set of adjacent fields of the same data type 
// passed fields A and B must be consecutive in the structure
// so we can then calculate the byte size of each element and hence the correct offset
template <typename T>
static T* getASubFieldPtr(T* field_a, T* field_b, int i)
{
    ptrdiff_t step = field_b - field_a;
    return field_a + i * step;
}

// get reference to VAL* output fields amnd others via index.
// Assumes VALA, VALB etc. are packed sequentially in structure and 
// of same data type size so can move pointer by fixed amount.
// Index 0 -> 'VALA' etc.
static void* getASubVAL(aSubRecord *prec, int i)
{
    return *(getASubFieldPtr<void*>(&(prec->vala), &(prec->valb), i));
}

static void* getASubInput(aSubRecord *prec, int i)
{
    return *(getASubFieldPtr<void*>(&(prec->a), &(prec->b), i));
}

static epicsUInt32 getASubNO(aSubRecord *prec, int i)
{
    return *(getASubFieldPtr<epicsUInt32>(&(prec->noa), &(prec->nob), i));
}

static epicsUInt32 getASubNE(aSubRecord *prec, int i)
{
    return *(getASubFieldPtr<epicsUInt32>(&(prec->nea), &(prec->neb), i));
}

static epicsUInt32 getASubNOV(aSubRecord *prec, int i)
{
    return *(getASubFieldPtr<epicsUInt32>(&(prec->nova), &(prec->novb), i));
}

static epicsUInt32& getASubNEV(aSubRecord *prec, int i)
{
    return *(getASubFieldPtr<epicsUInt32>(&(prec->neva), &(prec->nevb), i));
}

static epicsEnum16 getASubFTV(aSubRecord *prec, int i)
{
    return *(getASubFieldPtr<epicsEnum16>(&(prec->ftva), &(prec->ftvb), i));
}

static epicsEnum16 getASubFT(aSubRecord *prec, int i)
{
    return *(getASubFieldPtr<epicsEnum16>(&(prec->fta), &(prec->ftb), i));
}

// If field is of single string or char array type return its value
// return blank string if invalid type (string array or numeric) so we skip it
std::string getASubStringInput(aSubRecord *prec, int i) {
    epicsEnum16 ft = getASubFT(prec, i);
    if (ft == menuFtypeCHAR || ft == menuFtypeUCHAR) {
        return (const char*)getASubInput(prec, i);
    } else if (ft == menuFtypeSTRING && getASubNO(prec, i) == 1) {
        return (const char*)getASubInput(prec, i);
    } else {
        return "";
    }        
}

// recursively walk path_bits in JSON to get value of interest
// level is current point in tree and increemented each recursion
static json getJSONValueRecurse(const json& j, const std::vector<std::string>& path_bits, int level)
{
    if (level < path_bits.size()) {
        const std::string& key = path_bits[level]; // key to locate for next iteration
        if (j.is_array()) {
            char* endptr = NULL;
            long index = strtol(key.c_str(), &endptr, 10);  // if JSON array at this level we expect key to be an numeric index
            if ( endptr != NULL && (endptr - key.c_str()) == key.size() &&
                 index >= 0 && index < j.size() ) {             
                return getJSONValueRecurse(j.at(index), path_bits, level + 1);
            } else {
                throw std::runtime_error("array index " + std::to_string(index) + " is either out of range or not a valid integer");
            }
        } else if (j.contains(key)) { // pull substructure and recurse
            return getJSONValueRecurse(j.at(key), path_bits, level + 1);
        }
        else {
            throw std::runtime_error("cannot find key " + key + " at level " + std::to_string(level));
        }
    } else {
        return j; // reached end of path, return result
    }
}

// extract value at location specified by path from json
// path is like /a/b/c with a,b,c being field names or array indexes
json getJSONValue(const json& j, const std::string& path)
{
    std::vector<std::string> path_bits;
    boost::algorithm::split(path_bits, path, boost::is_any_of("/"), boost::token_compress_on);
    // remove any blank path elements, usually from initial or final / in path 
    auto r = std::remove(path_bits.begin(), path_bits.end(), "");
    path_bits.erase(r, path_bits.end());
    return getJSONValueRecurse(j, path_bits, 0);
}

// extract value at location specified by path from json_str
json getJSONValue(const std::string& json_str, const std::string& path)
{
    json json_parsed = json::parse(json_str);
    return getJSONValue(json_parsed, path);
}

template <typename T>
static T getValueConvert(const json& value)
{
    return value.get<T>();
}

// copy json values to an aSub record output field
template <typename T>
static void copyOutput(const json& values, aSubRecord *prec, int i)
{
    epicsUInt32& nev = getASubNEV(prec, i); // reference for number of items written to an array output field
    T* val_out = (T*)getASubVAL(prec, i);
    if (values.is_array()) {
        epicsUInt32 ncopy = std::min((epicsUInt32)values.size(), getASubNOV(prec, i));
        for(size_t j=0; j<ncopy; ++j) {
            val_out[j] = getValueConvert<T>(values.at(j));
        }
        nev = ncopy;
    } else {
        val_out[0] = getValueConvert<T>(values);
        nev = 1;        
    }
}

// input A is JSON for parseJSON
// output A is number of input links processed (for debugging)
// input B onwards are paths to items in JSON to extract
// output B onwards are value extracted from path specified in corresponding input
static long parseJSON(aSubRecord *prec)
{
    if (prec->fta != menuFtypeCHAR) {
         errlogPrintf("%s incorrect input type. Should be FTA (CHAR)", prec->name);
         return -1;
    }
    if (prec->ftva != menuFtypeULONG) {
         errlogPrintf("%s incorrect output type. Should be FTVA (ULONG)", prec->name);
         return -1;
    }
    epicsUInt32& nprocessed = *(epicsUInt32*)getASubVAL(prec, 0); // VALA
    nprocessed = 0; // number of processed input links
    try {
        std::string json_str((const char*)(prec->a)); /* waveform CHAR data */    
        json json_parsed = json::parse(json_str);
        // loop over fields from B to U
        int max_index = 'U' - 'A';
        for(int i=1; i<max_index; ++i) {
            std::string path = getASubStringInput(prec, i);
            if (path.size() == 0) {
                continue;
            }
            json value = getJSONValue(json_parsed, path);
            if (value.is_object()) {
                errlogPrintf("%s incorrect return json type (object) for field %c.", prec->name, 'A' + i);
                return -1;
            }
            epicsUInt32& nev = getASubNEV(prec, i);
            epicsUInt32 nov = getASubNOV(prec, i);
            switch(getASubFTV(prec, i)) {
                case menuFtypeSTRING:
                    if (value.is_array()) {
                        epicsUInt32 ncopy = std::min((epicsUInt32)value.size(), nov);
                        epicsOldString* val_out = (epicsOldString*)getASubVAL(prec, i);
                        for(size_t j=0; j<ncopy; ++j) {
                            if (value.at(j).is_string()) {
                                strncpy(val_out[j], value.at(j).get<std::string>().c_str(), sizeof(epicsOldString));
                            } else {
                                strncpy(val_out[j], nlohmann::to_string(value.at(j)).c_str(), sizeof(epicsOldString));
                            }
                        }
                        nev = ncopy;
                    } else {
                        memset(getASubVAL(prec, i), 0, sizeof(epicsOldString));
                        if (value.is_string()) {
                            strncpy((char*)getASubVAL(prec, i), value.get<std::string>().c_str(), sizeof(epicsOldString));
                        } else {
                            strncpy((char*)getASubVAL(prec, i), nlohmann::to_string(value).c_str(), sizeof(epicsOldString));
                        }
                        nev = 1;
                    }
                    break;
                    
                case menuFtypeCHAR:
                case menuFtypeUCHAR:
                    memset(getASubVAL(prec, i), 0, nov);
                    if (value.is_string()) {
                        strncpy((char*)getASubVAL(prec, i), value.get<std::string>().c_str(), nov);
                        nev = std::min(nov, (epicsUInt32)value.get<std::string>().size());
                    } else {
                        strncpy((char*)getASubVAL(prec, i), nlohmann::to_string(value).c_str(), nov);
                        nev = std::min(nov, (epicsUInt32)nlohmann::to_string(value).size());
                    }
                    break;

                case menuFtypeFLOAT:
                    copyOutput<float>(value, prec, i);
                    break;

                case menuFtypeDOUBLE:
                    copyOutput<double>(value, prec, i);
                    break;

                case menuFtypeLONG:
                    copyOutput<epicsInt32>(value, prec, i);
                    break;

                case menuFtypeULONG:
                    copyOutput<epicsUInt32>(value, prec, i);
                    break;

                case menuFtypeSHORT:
                    copyOutput<epicsInt16>(value, prec, i);
                    break;

                case menuFtypeUSHORT:
                    copyOutput<epicsUInt16>(value, prec, i);
                    break;

                case menuFtypeENUM:
                    copyOutput<epicsUInt16>(value, prec, i);
                    break;
                
                default:
                    errlogPrintf("%s unknown output FTV type for field %c.", prec->name, 'A' + i);
                    return -1;
            }
            ++nprocessed;
        }
    }
    catch(const std::exception& ex) {
        errlogPrintf("%s error %s.", prec->name, ex.what());
        return -1;
    }
    return 0; /* process record output links */
}

extern "C" {
    epicsRegisterFunction(parseJSON); /* must also be mentioned in asubFunctions.dbd */
}

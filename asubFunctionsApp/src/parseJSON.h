/*************************************************************************\ 
* Copyright (c) 2024 Science and Technology Facilities Council (STFC), GB. 
* All rights reverved. 
* This file is distributed subject to a Software License Agreement found 
* in the LICENSE file that is included with this distribution. 
\*************************************************************************/ 

#ifndef PARSEJSON_H
#define PARSEJSON_H

epicsShareExtern json getJSONValue(const std::string& json_str, const std::string& path);
epicsShareExtern json getJSONValue(const json& json, const std::string& path);

#endif /* PARSEJSON_H */

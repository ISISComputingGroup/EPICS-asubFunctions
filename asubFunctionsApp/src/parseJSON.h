#ifndef PARSEJSON_H
#define PARSEJSON_H

epicsShareExtern json getJSONValue(const std::string& json_str, const std::string& path);
epicsShareExtern json getJSONValue(const json& json, const std::string& path);

#endif /* PARSEJSON_H */

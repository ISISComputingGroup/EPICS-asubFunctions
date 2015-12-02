/// @file setFunctions.c
/// @author Freddie Akeroyd, STFC (freddie.akeroyd@stfc.ac.uk)
/// @ingroup asub_functions
///
///
#include <map>
#include <set>
#include <string>
#include <cstring>

#include <epicsGuard.h>
#include <epicsMutex.h>

#include "setFunctions.h"

static std::map< std::string, std::set<std::string> > the_sets;

static epicsMutex set_lock;

// look for a possible NULL terminator in bytes
static int get_length(const char* str, int maxlen)
{
    int len;
	for(len = 0; len < maxlen && str[len] != '\0'; ++len)
	    ;
	return len;
}
 
int addToSetImpl(const char* set_name, const char* item_value, int max_len_item_value)
{
    epicsGuard<epicsMutex> _lock(set_lock);
    std::set<std::string>& the_set = the_sets[set_name];
    std::string item(item_value, get_length(item_value, max_len_item_value));
	the_set.insert(item);
	return 0;
}

int removeFromSetImpl(const char* set_name, const char* item_value, int max_len_item_value)
{
    epicsGuard<epicsMutex> _lock(set_lock);
    std::set<std::string>& the_set = the_sets[set_name];
    std::string item(item_value, get_length(item_value, max_len_item_value));
	the_set.erase(item);
	return 0;
}

int	getSetItemCountImpl(const char* set_name, int* item_count)
{
    epicsGuard<epicsMutex> _lock(set_lock);
    const std::set<std::string>& the_set = the_sets[set_name];
	*item_count = the_set.size();
	return 0;
}

int	getSetItemsImpl(const char* set_name, char* set_values, int max_len, int* len, int* item_count)
{
    epicsGuard<epicsMutex> _lock(set_lock);
    const std::set<std::string>& the_set = the_sets[set_name];
	int i = 0;
    for(std::set<std::string>::const_iterator it = the_set.begin(); i < max_len && it != the_set.end(); ++it)
	{
	    strncpy(set_values + i, it->c_str(), max_len - i);
		i += it->size();
		if (i < max_len)
		{
		    set_values[i] = ' ';
			++i;
		}
	}
	if (i < max_len)
	{
	    if (i > 0)
		{
		    --i;   // remove trailing final ' ' added above
		}
	    memset(set_values + i, '\0', max_len - i); // or space pad?
	    *len = i + 1; // +1 to make sure '\0' is included
	}
	else
	{
	    set_values[max_len-1] = '\0';
		*len = max_len;
	}
	*item_count = the_set.size();
	return 0;
}



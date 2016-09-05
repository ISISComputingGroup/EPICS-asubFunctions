/** @file queuedPVSetImpl.cpp
 *  @author Freddie Akeroyd, STFC (freddie.akeroyd@stfc.ac.uk)
 *  @ingroup asub_functions
 *
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <map>
#include <string>
#include <registryFunction.h>
#include <epicsTypes.h>
#include <menuFtype.h>
#include <errlog.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <epicsMessageQueue.h>

#include "queuedPVSetImpl.h"

#include "pv.h"

struct PVItem
{
    char recname[256];
    char name[256];
    char value[1024];
	double timeout;
};

static epicsMutex the_lock;

static bool first_call = true;

#define MESSAGE_QUEUE_SIZE 300

/// queue of pending PV set opoerations to do
static epicsMessageQueue the_queue(MESSAGE_QUEUE_SIZE, sizeof(PVItem));

/// event used by queuedCallback to singal PV completion callback success 
static epicsEvent callback_event;

/// PV completion callback function used by putCallback()
static void queuedCallback(void *var, pvType type, unsigned count, pvValue *value, void *arg, pvStat status)
{
    PVItem* pitem = (PVItem*)arg;
//	printf("queuedCallback %s \"%s\" = \"%s\"\n", pitem->recname, pitem->name, pitem->value);
    callback_event.signal();
}

/// loop round queued PV set operations to do
// PV references cached in pv_map
static void queuedPVThread(void* arg)
{
    PVItem item;
	std::map<std::string,pvVariable*> pv_map;
	std::map<std::string,pvVariable*>::iterator it;
	pvSystem *sys = newPvSystem( "ca" );
    while( true ) 
	{
		if ( the_queue.receive(&item, sizeof(item)) == sizeof(item) )
		{
			try
			{
				pvVariable* var;
			    if ( (it = pv_map.find(item.name)) == pv_map.end() )
				{
					var = sys->newVariable( item.name );
		            sys->pend(0.5, TRUE);
					pv_map[item.name] = var;
				}
				else
				{
					var = it->second;
				}
//		        printf("queuedPVThread %s set \"%s\" = \"%s\" STARTING\n", item.recname, item.name, item.value);
                if (var->putCallback(pvTypeCHAR, strlen(item.value), (pvValue*)item.value, queuedCallback, &item) != pvStatOK)
				{
		            errlogSevPrintf(errlogInfo, "queuedPVThread: %s set \"%s\" = \"%s\" PUT ERROR\n", item.recname, item.name, item.value);
					pv_map.erase(item.name);
					delete var;
					continue;
				}
				int ntry = 10;
				bool done = false;
				for(int i=0; i<ntry && !done; ++i)
				{
		            sys->pend(0.1, TRUE);
				    if ( callback_event.wait(item.timeout / static_cast<double>(ntry)) )
					{
						done = true;
					}
				}
//				while( !callback_event.tryWait() )
//				{
//		            sys->pend(0.1, TRUE);
//				}
				if ( !done )
				{
		            errlogSevPrintf(errlogInfo, "queuedPVThread: %s set \"%s\" = \"%s\" WAIT TIMEOUT %.2f queued %d\n", item.recname, item.name, item.value, item.timeout, the_queue.pending());
				}
//				else
//				{
//					printf("queuedPVThread %s set \"%s\" = \"%s\" DONE\n", item.recname, item.name, item.value);
//				}
			}
			catch(const std::exception& ex)
			{
                errlogSevPrintf(errlogInfo, "queuedPVThread: %s exception %s\n", item.recname, ex.what());
			}
//		    epicsThreadSleep(0.1);
		}
	}
    delete sys;
}

/// return 0 for message queued, -1 for error. Note that we do not know at this point if a queued message was
/// successfully delivered
int queuedPVSetImpl(const char* recname, const char* pv_in, epicsUInt32 len_pv_in, epicsUInt32 max_len_pv_in, const char* value_in, epicsUInt32 max_len_value_in, epicsUInt32 len_value_in, double timeout)
{
    PVItem item;
	if (max_len_pv_in >= sizeof(item.name))
	{
        errlogSevPrintf(errlogMajor, "queuedPVSet: %s pv to set too long\n", recname);
	    return -1;
	}
	if (max_len_value_in >= sizeof(item.value))
	{
        errlogSevPrintf(errlogMajor, "queuedPVSet: %s pv value too long\n", recname);
	    return -1;
	}
	if (pv_in == NULL || pv_in[0] == '\0' || len_pv_in == 0)
	{
// we get this on startup if IOC is not running	
//        errlogPrintf("queuedPVSet: %s pv name is empty\n", recname);
	    return -1;
	}
	memset(&item, 0, sizeof(item));
	strncpy(item.recname, recname, sizeof(item.recname));
	strncpy(item.name, pv_in, std::min(len_pv_in, max_len_pv_in));
	strncpy(item.value, value_in, std::min(len_value_in, max_len_value_in));
	item.timeout = timeout;
	int stat = the_queue.trySend(&item, sizeof(item));
	if (stat == -1)
	{
        errlogSevPrintf(errlogInfo, "queuedPVSet: %s unable to queue message\n", recname);
	}
	if (first_call)
	{
		epicsGuard<epicsMutex> _lock(the_lock);
		if (first_call)
		{
	        first_call = false;
            if (epicsThreadCreate("queuedPVThread",
                          epicsThreadPriorityMedium,
                          epicsThreadGetStackSize(epicsThreadStackMedium),
                          (EPICSTHREADFUNC)queuedPVThread, NULL) == 0)
            {
                errlogSevPrintf(errlogMajor, "queuedPVSet: %s unable to start thread for queuedPVThread\n", recname);
                return -1;
            }
        }			
	}
    return stat;
}

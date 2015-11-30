#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <registryFunction.h>
#include <aSubRecord.h>
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

static epicsMessageQueue the_queue(30, sizeof(PVItem));

static epicsEvent callback_event;

static void queuedCallback(void *var, pvType type, unsigned count, pvValue *value, void *arg, pvStat status)
{
    PVItem* pitem = (PVItem*)arg;
//	printf("queuedCallback %s \"%s\" = \"%s\"\n", pitem->recname, pitem->name, pitem->value);
    callback_event.signal();
}

static void queuedPVThread(void* arg)
{
    PVItem item;
	pvSystem *sys = newPvSystem( "ca" );
    while( true ) 
	{
		if ( the_queue.receive(&item, sizeof(item)) == sizeof(item) )
		{
			try
			{
	            pvVariable *var = sys->newVariable( item.name );
		        sys->pend(0.1, TRUE);
//		        printf("queuedPVThread %s set \"%s\" = \"%s\" STARTING\n", item.recname, item.name, item.value);
                if (var->putCallback(pvTypeCHAR, strlen(item.value), (pvValue*)item.value, queuedCallback, &item) != pvStatOK)
				{
		            errlogPrintf("queuedPVThread: %s set \"%s\" = \"%s\" PUT ERROR\n", item.recname, item.name, item.value);
				}
		        sys->pend(0.1, TRUE);
//				while( !callback_event.tryWait() )
//				{
//		            sys->pend(0.1, TRUE);
//				}
				if ( !callback_event.wait(item.timeout) )
				{
		            errlogPrintf("queuedPVThread: %s set \"%s\" = \"%s\" WAIT TIMEOUT after %f\n", item.recname, item.name, item.value, item.timeout);
				}
//				else
//				{
//					printf("queuedPVThread %s set \"%s\" = \"%s\" DONE\n", item.recname, item.name, item.value);
//				}
		        delete var;
			}
			catch(const std::exception& ex)
			{
                errlogPrintf("queuedPVThread: %s exception %s\n", item.recname, ex.what());
			}
//		    epicsThreadSleep(0.1);
		}
	}
    delete sys;
}

int queuedPVSetImpl(const char* recname, const char* pv_in, epicsUInt32 len_pv_in, epicsUInt32 max_len_pv_in, const char* value_in, epicsUInt32 max_len_value_in, epicsUInt32 len_value_in, double timeout)
{
    PVItem item;
	if (max_len_pv_in >= sizeof(item.name))
	{
        errlogPrintf("queuedPVSet: %s pv to set too long\n", recname);
	    return -1;
	}
	if (max_len_value_in >= sizeof(item.value))
	{
        errlogPrintf("queuedPVSet: %s pv value too long\n", recname);
	    return -1;
	}
	if (pv_in == NULL || pv_in[0] == '\0' || len_pv_in == 0)
	{
        errlogPrintf("queuedPVSet: %s pv name is empty\n", recname);
	    return -1;
	}
	memset(&item, 0, sizeof(item));
	strncpy(item.recname, recname, sizeof(item.recname));
	strncpy(item.name, pv_in, std::min(len_pv_in, max_len_pv_in));
	strncpy(item.value, value_in, std::min(len_value_in, max_len_value_in));
	item.timeout = timeout;
	int stat = the_queue.trySend(&item, sizeof(item));
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
                errlogPrintf("queuedPVSet: %s unable to start thread for queuedPVThread\n", recname);
                return -1;
            }
        }			
	}
    return stat;
}

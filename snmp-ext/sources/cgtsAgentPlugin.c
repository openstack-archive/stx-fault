/*
* Copyright (c) 2016 Wind River Systems, Inc.
*
* SPDX-License-Identifier: Apache-2.0
*
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "cgtsAgentPlugin.h"


#define CGTS_ALM_TABLE_NAME         "wrsAlarmActiveTable"
#define CGTS_EVENT_TABLE_NAME       "wrsEventTable"

#define _UNREGISTER_MIB(var, miboid) \
	do{ \
        size_t varsize = sizeof(struct variable2); \
        struct variable2 *vp; \
        oid myoid[MAX_OID_LEN]; \
        size_t length; \
        int i, result; \
        int numvars = sizeof(var)/varsize; \
        int miboidlen = sizeof(miboid)/sizeof(oid); \
        for (i=0; i < numvars; i++) { \
            vp = (struct variable2 *)((char *)var + varsize * i); \
            memcpy(myoid, miboid, miboidlen * sizeof(oid)); \
            memcpy(myoid+miboidlen, vp->name, vp->namelen * sizeof(oid)); \
            length = miboidlen + vp->namelen; \
            result = unregister_mib(myoid, length); \
            DEBUGMSGOID(("cgtsAgentPlugin", myoid, length)); \
            if (result != MIB_UNREGISTERED_OK) { \
                snmp_log(LOG_ERR, "%s: Unregistering failed:%d\n", \
                         __FUNCTION__, result); \
            } else { \
                DEBUGMSGTL(("cgtsAgentPlugin", "Unregistering succeeded\n")); \
            } \
        } \
    }while(0);


static TFmAlarmSessionT alm_handle;

/*
 * * the OID we want to register our integer at.  This should be the
 * * OID node for the entire table.  In our case this is the
 * * wrsAlarmActiveTable oid definition
 */
oid    cgtsAlarmTable_oid[] =
		{ 1, 3, 6, 1, 4, 1, 731, 1, 1, 1, 1, 1};

oid    cgtsEventLogTable_oid[] =
		{ 1, 3, 6, 1, 4, 1, 731, 1, 1, 1, 1, 4};


struct variable2 alarm_variables[] = {
		{ALARM_INDEX, ASN_INTEGER, NETSNMP_OLDAPI_RONLY,
		var_alarms, 2, {1, 1}},
		{ALARM_UUID, ASN_OCTET_STR, NETSNMP_OLDAPI_RONLY,
		var_alarms, 2, {1, 2}},
		{ALARM_ID, ASN_OCTET_STR, NETSNMP_OLDAPI_RONLY,
		var_alarms, 2, {1, 3}},
		{ALARM_INSTANCE_ID, ASN_OCTET_STR, NETSNMP_OLDAPI_RONLY,
		var_alarms, 2, {1, 4}},
		{ALARM_TIME, ASN_OCTET_STR, NETSNMP_OLDAPI_RONLY,
		var_alarms, 2, {1, 5}},
		{ALARM_SEVERITY, ASN_INTEGER, NETSNMP_OLDAPI_RONLY,
		var_alarms, 2, {1, 6}},
		{ALARM_REASONTEXT, ASN_OCTET_STR, NETSNMP_OLDAPI_RONLY,
		var_alarms, 2, {1, 7}},
		{ALARM_EVENTTYPE, ASN_INTEGER, NETSNMP_OLDAPI_RONLY,
		var_alarms, 2, {1, 8}},
		{ALARM_PROBABLECAUSE, ASN_INTEGER, NETSNMP_OLDAPI_RONLY,
		var_alarms, 2, {1, 9}},
		{ALARM_REPAIRACTION , ASN_OCTET_STR, NETSNMP_OLDAPI_RONLY,
		var_alarms, 2, {1, 10}},
		{ALARM_SERVICEAFFECTING, ASN_INTEGER, NETSNMP_OLDAPI_RONLY,
		var_alarms, 2, {1, 11}},
		{ALARM_SUPPRESSION, ASN_INTEGER, NETSNMP_OLDAPI_RONLY,
		var_alarms, 2, {1, 12}},
};

struct variable2 event_log_variables[] = {
		{EVENT_INDEX, ASN_INTEGER, NETSNMP_OLDAPI_RONLY,
		var_events, 2, {1, 1}},
		{EVENT_UUID, ASN_OCTET_STR, NETSNMP_OLDAPI_RONLY,
		var_events, 2, {1, 2}},
		{EVENT_EVENT_ID, ASN_OCTET_STR, NETSNMP_OLDAPI_RONLY,
		var_events, 2, {1, 3}},
		{EVENT_STATE, ASN_INTEGER, NETSNMP_OLDAPI_RONLY,
		var_events, 2, {1, 4}},
		{EVENT_INSTANCE_ID, ASN_OCTET_STR, NETSNMP_OLDAPI_RONLY,
		var_events, 2, {1, 5}},
		{EVENT_TIME, ASN_OCTET_STR, NETSNMP_OLDAPI_RONLY,
		var_events, 2, {1, 6}},
		{EVENT_SEVERITY, ASN_INTEGER, NETSNMP_OLDAPI_RONLY,
		var_events, 2, {1, 7}},
		{EVENT_REASONTEXT, ASN_OCTET_STR, NETSNMP_OLDAPI_RONLY,
		var_events, 2, {1, 8}},
		{EVENT_EVENTTYPE, ASN_INTEGER, NETSNMP_OLDAPI_RONLY,
		var_events, 2, {1, 9}},
		{EVENT_PROBABLECAUSE, ASN_INTEGER, NETSNMP_OLDAPI_RONLY,
		var_events, 2, {1, 10}},
		{EVENT_REPAIRACTION , ASN_OCTET_STR, NETSNMP_OLDAPI_RONLY,
		var_events, 2, {1, 11}},
		{EVENT_SERVICEAFFECTING, ASN_INTEGER, NETSNMP_OLDAPI_RONLY,
		var_events, 2, {1, 12}},
		{EVENT_SUPPRESSION, ASN_INTEGER, NETSNMP_OLDAPI_RONLY,
		var_events, 2, {1, 13}},
};


TFmAlarmSessionT getAlarmSession()
{
	return alm_handle;
}

/*
 * our initialization routine, automatically called by the agent
  * (to get called, the function name must match init_FILENAME())
 */
void
init_cgtsAgentPlugin(void)
{
	snmp_log(LOG_INFO,"init_cgtsAgentPlugin start");
	snmp_log(LOG_INFO,"MIB registration: %s",CGTS_ALM_TABLE_NAME);
	REGISTER_MIB(CGTS_ALM_TABLE_NAME, alarm_variables,
			variable2, cgtsAlarmTable_oid);

	snmp_log(LOG_INFO,"MIB registration: %s",CGTS_EVENT_TABLE_NAME);
	REGISTER_MIB(CGTS_EVENT_TABLE_NAME, event_log_variables,
			variable2, cgtsEventLogTable_oid);

	snmp_log(LOG_INFO,"get alarm database handler");
	if (fm_snmp_util_create_session(&alm_handle, NULL) != true){
		snmp_log(LOG_ERR,"failed to get alarm database handler");
		exit(-1);
	}
	/*
	 * a debugging statement.  Run the agent with -DcgtsAgentPlugin to see
	 * the output of this debugging statement.
	*/
	DEBUGMSGTL(("cgtsAgentPlugin", "Done initializing.\n"));
}

void
deinit_cgtsAgentPlugin(void)
{
	snmp_log(LOG_INFO,"deinit_cgtsAgentPlugin start");
	_UNREGISTER_MIB(alarm_variables, cgtsAlarmTable_oid);
	_UNREGISTER_MIB(event_log_variables, cgtsEventLogTable_oid);
	fm_snmp_util_destroy_session(alm_handle);
}




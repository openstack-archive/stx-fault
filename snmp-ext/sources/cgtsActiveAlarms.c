/*
* Copyright (c) 2013-2014 Wind River Systems, Inc.
*
* SPDX-License-Identifier: Apache-2.0
*
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/net-snmp-features.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <fmDbAPI.h>
#include "cgtsAgentPlugin.h"


netsnmp_feature_require(date_n_time)

#define MINLOADFREQ 1     /* min reload frequency in seconds */

static long Active_Alarm_Count = 0;
static struct activealarm *alarm_list;
static struct activealarm *alarmaddr, savealarm, *savealarmaddr;
static int saveIndex = 0;
static char saveUuid[36];
static long LastLoad = 0;     /* ET in secs at last table load */
extern long long_return;


int Active_Alarm_Get_Count(void);

static void
Alarm_Scan_Init()
{
	struct timeval et;       /* elapsed time */
	struct activealarm  **activealarm_ptr;
	SFmAlarmQueryT aquery;
	size_t i = 0;

	saveIndex = 0;
	netsnmp_get_monotonic_clock(&et);
	if ( et.tv_sec < LastLoad + MINLOADFREQ ) {
		DEBUGMSG(("cgtsAgentPlugin", "Skip reload" ));
		alarmaddr = alarm_list;
		return;
	}
	LastLoad = et.tv_sec;

	/*
	 * free old list:
	 */
	while (alarm_list) {
		struct activealarm   *old = alarm_list;
		alarm_list = alarm_list->next;
		free(old);
	}
	alarmaddr = 0;
	activealarm_ptr = &alarm_list;

	/*
	 * query active alarm list from DB
	 */
	if (fm_snmp_util_get_all_alarms(getAlarmSession(), &aquery) != true){
		DEBUGMSG(("cgtsAgentPlugin", "get_all_alarms from db failed\n"));
		return;
	}
	DEBUGMSG(("cgtsAgentPlugin", "get_all_alarms returns %lu alarms\n", aquery.num));
	for (i = 0; i < aquery.num; ++i){
		struct activealarm   *almnew;
		/*populate alarm_list*/
		almnew = (struct activealarm *) calloc(1, sizeof(struct activealarm));
		if (almnew == NULL)
			break;              /* alloc error */
		*activealarm_ptr = almnew;
		activealarm_ptr = &almnew->next;
		memset(&almnew->alarmdata, 0 , sizeof(almnew->alarmdata));
		memcpy(&almnew->alarmdata, aquery.alarm + i, sizeof(almnew->alarmdata));
	}
	alarmaddr = alarm_list;
	free(aquery.alarm);
}

static int
Alarm_Scan_NextAlarm(int *Index,
		char *Name,
		struct activealarm *Aalm)
{
	struct activealarm alm;
	while (alarmaddr) {
		alm = *alarmaddr;
		strlcpy(saveUuid, alm.alarmdata.uuid, sizeof(saveUuid));
		if (Index)
			*Index = ++saveIndex;
		if (Aalm)
			*Aalm = alm;
		if (Name)
			strcpy(Name, saveUuid);

		savealarm = alm;
		savealarmaddr = alarmaddr;
		alarmaddr = alm.next;
		return 1;
	}
	return 0;
}

static int
Alarm_Scan_By_Index(int Index,
		char *Name,
		struct activealarm *Aalm)
{
    int i;

    DEBUGMSGTL(("cgtsAgentPlugin","Alarm_Scan_By_Index"));
    Alarm_Scan_Init();
    while (Alarm_Scan_NextAlarm(&i, Name, Aalm)) {
        if (i == Index)
            break;
    }
    if (i != Index)
        return (-1);            /* Error, doesn't exist */
    return (0);                 /* DONE */
}

static int
header_alarmEntry(struct variable *vp,
               oid * name,
               size_t * length,
               int exact, size_t * var_len,
               WriteMethod ** write_method)
{
#define ALM_ENTRY_NAME_LENGTH	14
    oid             newname[MAX_OID_LEN];
    register int    index;
    int             result, count;

    DEBUGMSGTL(("cgtsAgentPlugin", "header_alarmEntry: "));
    DEBUGMSGOID(("cgtsAgentPlugin", name, *length));
    DEBUGMSG(("cgtsAgentPlugin", "exact %d\n", exact));

    memcpy((char *) newname, (char *) vp->name,
           (int) vp->namelen * sizeof(oid));
    /*
     * find "next" alarm
     */
    count = Active_Alarm_Get_Count();
    DEBUGMSG(("cgtsAgentPlugin", "count %d\n", count));
    for (index = 1; index <= count; index++) {
        newname[ALM_ENTRY_NAME_LENGTH] = (oid) index;
        result =
            snmp_oid_compare(name, *length, newname,
                             (int) vp->namelen + 1);
        if ((exact && (result == 0)) || (!exact && (result < 0)))
            break;
    }
    if (index > count) {
    	DEBUGMSGTL(("cgtsAgentPlugin", "... index out of range\n"));
        return MATCH_FAILED;
    }

    memcpy((char *) name, (char *) newname,
           ((int) vp->namelen + 1) * sizeof(oid));
    *length = vp->namelen + 1;
    *write_method = 0;
    *var_len = sizeof(long);    /* default to 'long' results */

    DEBUGMSGTL(("cgtsAgentPlugin", "... get ALM data "));
    DEBUGMSGOID(("cgtsAgentPlugin", name, *length));
    DEBUGMSG(("cgtsAgentPlugin", "\n"));

    DEBUGMSG(("cgtsAgentPlugin","Return index: %d\n", index));
    return index;
}


int
Active_Alarm_Get_Count(void)
{
	static time_t   scan_time = 0;
	time_t          time_now = time(NULL);

	if (!Active_Alarm_Count || (time_now > scan_time + 60)) {
		scan_time = time_now;
		Alarm_Scan_Init();
		Active_Alarm_Count = 0;
		while (Alarm_Scan_NextAlarm(NULL, NULL, NULL) != 0) {
			Active_Alarm_Count++;
		}
	}
	return (Active_Alarm_Count);
}

u_char *
var_alarms(struct variable *vp,
            oid * name,
            size_t * length,
            int exact, size_t * var_len,
            WriteMethod ** write_method)
{
	static struct activealarm alrm;
	static char     Name[36];
	char           *cp;
	int index = 0;

	DEBUGMSGTL(("cgtsAgentPlugin", "var_alarms"));
	index = header_alarmEntry(vp, name, length, exact, var_len, write_method);
	if (index == MATCH_FAILED)
		return NULL;

	Alarm_Scan_By_Index(index, Name, &alrm);

	switch (vp->magic) {
	case ALARM_INDEX:
		long_return = index;
		return (u_char *) & long_return;
	case ALARM_UUID:
		cp = Name;
		*var_len = strlen(cp);
		return (u_char *) cp;
	case ALARM_ID:
		cp = alrm.alarmdata.alarm_id;
		*var_len = strlen(cp);
		return (u_char *) cp;
	case ALARM_INSTANCE_ID:
		cp = alrm.alarmdata.entity_instance_id;
		*var_len = strlen(cp);
		return (u_char *) cp;
	case ALARM_TIME:{
		time_t when = alrm.alarmdata.timestamp/SECOND_PER_MICROSECOND;
		cp = (char *) date_n_time(&when, var_len );
		return (u_char *) cp;
	}
	case ALARM_SEVERITY:
		long_return = alrm.alarmdata.severity;
		return (u_char *) & long_return;
	case ALARM_REASONTEXT:
		cp = alrm.alarmdata.reason_text;
		*var_len = strlen(cp);
		return (u_char *) cp;
	case ALARM_EVENTTYPE:
		long_return = alrm.alarmdata.alarm_type;
		return (u_char *) & long_return;
	case ALARM_PROBABLECAUSE:
		long_return = alrm.alarmdata.probable_cause;
		return (u_char *) & long_return;
	case ALARM_REPAIRACTION:
		cp = alrm.alarmdata.proposed_repair_action;
		*var_len = strlen(cp);
		return (u_char *) cp;
	case ALARM_SERVICEAFFECTING:
		long_return = alrm.alarmdata.service_affecting;
		return (u_char *) & long_return;
	case ALARM_SUPPRESSION:
		long_return = alrm.alarmdata.suppression;
		return (u_char *) & long_return;
	default:
		DEBUGMSGTL(("snmpd", "unknown sub-id %d in var_alarms\n",
				vp->magic));
	}
	return NULL;
}


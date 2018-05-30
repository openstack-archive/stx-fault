/*
* Copyright (c) 2013-2014 Wind River Systems, Inc.
*
* SPDX-License-Identifier: Apache-2.0
*
*/

#ifndef CGTSAGENTPLUGIN_H
#define CGTSAGENTPLUGIN_H

#include <fmAPI.h>
#include <fmDbAPI.h>
#include <net-snmp/agent/snmp_vars.h>

extern FindVarMethod var_alarms;

extern FindVarMethod var_events;

/* Active alarm Table */
#define ALARM_INDEX                1
#define ALARM_UUID                 2
#define ALARM_ID                   3
#define ALARM_INSTANCE_ID          4
#define ALARM_TIME                 5
#define ALARM_SEVERITY             6
#define ALARM_REASONTEXT           7
#define ALARM_EVENTTYPE            8
#define ALARM_PROBABLECAUSE        9
#define ALARM_REPAIRACTION         10
#define ALARM_SERVICEAFFECTING     11
#define ALARM_SUPPRESSION          12

/* Event Log Table */
#define EVENT_INDEX                1
#define EVENT_UUID                 2
#define EVENT_EVENT_ID             3
#define EVENT_STATE                4
#define EVENT_INSTANCE_ID          5
#define EVENT_TIME                 6
#define EVENT_SEVERITY             7
#define EVENT_REASONTEXT           8
#define EVENT_EVENTTYPE            9
#define EVENT_PROBABLECAUSE        10
#define EVENT_REPAIRACTION         11
#define EVENT_SERVICEAFFECTING     12
#define EVENT_SUPPRESSION          13


#define SECOND_PER_MICROSECOND  1000000

struct activealarm {
	SFmAlarmDataT alarmdata;
	struct activealarm *next;
};

/*
 * function declarations 
 */
#ifdef __cplusplus
extern "C" {
#endif
void  init_cgtsAgentPlugin(void);
void  deinit_cgtsAgentPlugin(void);
TFmAlarmSessionT getAlarmSession();
#ifdef __cplusplus
}
#endif

#endif                          /* CGTSAGENTPLUGIN_H */

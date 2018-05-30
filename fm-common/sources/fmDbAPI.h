//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef FMDBAPI_H_
#define FMDBAPI_H_

#include <stdbool.h>
#include "fmAPI.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FM_SNMP_TRAP_UTIL_SESSION_NULL NULL

typedef void * TFmAlarmSessionT;

typedef struct {
	SFmAlarmDataT *alarm;
	size_t num;
} SFmAlarmQueryT;


bool fm_snmp_util_create_session(TFmAlarmSessionT *handle, const char *db_conn);

void fm_snmp_util_destroy_session(TFmAlarmSessionT handle);

/*
 * Used for applications running on the controller only.  Pass in an struct
 * and the API will allocate some alarms and return the number of alarms
 *
 * It is up to the caller to free the SFmAlamarQueryT->alarms entry when done
 */
bool fm_snmp_util_get_all_alarms(TFmAlarmSessionT handle,
		SFmAlarmQueryT *query);

bool fm_snmp_util_get_all_event_logs(TFmAlarmSessionT handle,
		SFmAlarmQueryT *query);

#ifdef __cplusplus
}
#endif



#endif /* FMDBAPI_H_ */

//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//


#ifndef FMALARMUTILS_H_
#define FMALARMUTILS_H_

#include "fmAPI.h"
#include "fmMsg.h"

#include <string>
#include <vector>

enum EFmAlarmIndexMap {
	FM_ALM_IX_UUID=0,
	FM_ALM_IX_ALARM_ID,
	FM_ALM_IX_ALARM_STATE,
	FM_ALM_IX_ENTITY_ID,
	FM_ALM_IX_INSTANCE_ID,
	FM_ALM_IX_TIMESTAMP,
	FM_ALM_IX_SEVERITY,
	FM_ALM_IX_REASON,
	FM_ALM_IX_ALARM_TYPE,
	FM_ALM_IX_PROBABLE_CAUSE,
	FM_ALM_IX_REPAIR_ACTION,
	FM_ALM_IX_SERVICE_AFFECT,
	FM_ALM_IX_SUPPRESSION,
	FM_ALM_IX_INHIBIT_ALARM,
	FM_ALM_IX_MAX
};

enum EFmLogIndexMap {
	FM_LOG_IX_UUID=0,
	FM_LOG_IX_LOG_ID,
	FM_LOG_IX_ENTITY_ID,
	FM_LOG_IX_INSTANCE_ID,
	FM_LOG_IX_TIMESTAMP,
	FM_LOG_IX_SEVERITY,
	FM_LOG_IX_REASON,
	FM_LOG_IX_LOG_TYPE,
	FM_LOG_IX_PROBABLE_CAUSE,
	FM_LOG_IX_SERVICE_AFFECT,
	FM_LOG_IX_MAX
};

enum EFmEventLogIndexMap {
	FM_EVENT_LOG_IX_UUID=0,
	FM_EVENT_LOG_IX_EVENT_ID,
	FM_EVENT_LOG_IX_STATE,
	FM_EVENT_LOG_IX_ENTITY_ID,
	FM_EVENT_LOG_IX_INSTANCE_ID,
	FM_EVENT_LOG_IX_TIMESTAMP,
	FM_EVENT_LOG_IX_SEVERITY,
	FM_EVENT_LOG_IX_REASON,
	FM_EVENT_LOG_IX_EVENT_TYPE,
	FM_EVENT_LOG_IX_PROBABLE_CAUSE,
	FM_EVENT_LOG_IX_REPAIR_ACTION,
	FM_EVENT_LOG_IX_SERVICE_AFFECT,
	FM_EVENT_LOG_IX_SUPPRESSION,
	FM_EVENT_LOG_IX_MAX
};

bool fm_alarm_set_field(EFmAlarmIndexMap field, SFmAlarmDataT *a, std::string &val);
bool fm_alarm_get_field(EFmAlarmIndexMap field, const SFmAlarmDataT *a, std::string &val);
bool fm_log_set_field(EFmLogIndexMap field, SFmAlarmDataT *a, std::string &val);
bool fm_event_log_set_field(EFmEventLogIndexMap field, SFmAlarmDataT *a, std::string &val);

void fm_formatted_str_to_vector(const std::string &s, std::vector<std::string> &alarm);

bool fm_alarm_to_string(const SFmAlarmDataT *alarm, std::string &str);
bool fm_alarm_from_string(const std::string &str,SFmAlarmDataT *alarm);

/**
 * This will create an alarm list from an alarm - will translate to string.
 * The indexes of this API are based on EFmAlarmIndexMap
 */
void fm_alarm_to_list(const SFmAlarmDataT *a, std::vector<std::string> &list);

bool fm_alarm_filter_to_string(const AlarmFilter *alarm, std::string &str);
bool fm_alarm_filter_from_string(const std::string &str, AlarmFilter *alarm);

/**
 * Generate a FM UUID
 */
void fm_uuid_create(fm_uuid_t &uuid);

/**
 * General utilities to conver alarm fields to and from strings
 */
EFmErrorT fm_error_from_string(const std::string &str);
std::string fm_error_from_int(EFmErrorT id);

void fm_log_request(fm_buff_t &req, bool failed=false);
void fm_log_response(fm_buff_t &req, fm_buff_t &resp, bool failed=false);

#endif /* FMALARMUTILS_H_ */

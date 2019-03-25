//
// Copyright (c) 2016-2018 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0

#include <stdlib.h>
#include <string>

#include "fmConstants.h"
#include "fmLog.h"
#include "fmDbAlarm.h"
#include "fmEventSuppression.h"

bool CFmEventSuppressionOperation::get_event_suppressed(CFmDBSession &sess, SFmAlarmDataT &data, bool &is_event_suppressed) {

	fm_db_result_t result;

	if (get_single_event_suppression(sess, data.alarm_id, result)) {
		if (result.size() > 0){
	        fm_db_single_result_t event_suppression = result[0];
		    std::string db_suppression_status = event_suppression[FM_EVENT_SUPPRESSION_COLUMN_SUPPRESSION_STATUS];

		    if (db_suppression_status.compare(FM_EVENT_SUPPRESSION_SUPPRESSED) == 0) {
		        is_event_suppressed = true;
		    } else {
		    	is_event_suppressed = false;
		    }
		} else
			return false;
	} else
		return false;

	return true;
}

bool CFmEventSuppressionOperation::get_single_event_suppression(CFmDBSession &sess, const char *alarm_id, fm_db_result_t & event_suppression) {
	std::string sql;
	std::string separator = ", ";

	sql = "SELECT ";
	sql += FM_EVENT_SUPPRESSION_COLUMN_CREATED_AT + separator;
	sql += FM_EVENT_SUPPRESSION_COLUMN_UPDATED_AT + separator;
	sql += FM_EVENT_SUPPRESSION_COLUMN_DELETED_AT + separator;
	sql += FM_EVENT_SUPPRESSION_COLUMN_ID + separator;
	sql += FM_EVENT_SUPPRESSION_COLUMN_UUID + separator;
	sql += FM_EVENT_SUPPRESSION_COLUMN_ALARM_ID + separator;
	sql += FM_EVENT_SUPPRESSION_COLUMN_DESCRIPTION + separator;
	sql += FM_EVENT_SUPPRESSION_COLUMN_SUPPRESSION_STATUS;

	sql += " FROM ";
	sql += FM_EVENT_SUPPRESSION_TABLE_NAME;

	sql += " WHERE ";
	sql += FM_EVENT_SUPPRESSION_COLUMN_ALARM_ID;
	sql += " = '";
	sql += std::string(alarm_id);
	sql += "'";

	FM_DEBUG_LOG("CMD:(%s)\n", sql.c_str());
	if ((sess.query(sql.c_str(), event_suppression)) != true){
		return false;
	}
	return true;
}

bool CFmEventSuppressionOperation::set_table_notify_listen(CFmDBSession &sess){

	std::string sql;
	fm_db_result_t rule_name;

	// Verify if rule already in table:
	sql = "SELECT rulename FROM pg_rules WHERE rulename='watch_event_supression'";

	if ((sess.query(sql.c_str(), rule_name)) != true){
		FM_DEBUG_LOG("Failed to query the existing rule");
		return false;
	}

	if (rule_name.size() == 0 ) {
		sql.clear();
		sql = "CREATE RULE watch_event_supression AS ON UPDATE TO ";
		sql += FM_EVENT_SUPPRESSION_TABLE_NAME;
		sql += " DO (NOTIFY ";
		sql += FM_EVENT_SUPPRESSION_TABLE_NAME;
		sql += ")";

		if (sess.cmd(sql.c_str(), false) < 0){
			FM_INFO_LOG("Failed to set rule CMD: (%s)", sql.c_str());
			return false;
		}

	}

	sql.clear();
	sql = "LISTEN ";
	sql += FM_EVENT_SUPPRESSION_TABLE_NAME;

	FM_DEBUG_LOG("CMD:(%s)\n", sql.c_str());
	// no row affected by LISTEN command
	return sess.cmd(sql.c_str(), false) < 0 ? false : true;
}

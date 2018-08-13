//
// Copyright (c) 2014-2018 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef FMDBCONSTANTS_H_
#define FMDBCONSTANTS_H_

/* DB connection status */
#define DB_DISCONNECTED       0
#define DB_CONNECTED          1

#define FM_MAX_SQL_STATEMENT_MAX    4096

#define FM_DB_TABLE_COUNT_COLUMN                 "count"

/* Alarm table name */
#define FM_ALARM_TABLE_NAME                       "alarm"

/* Event log table name */
#define FM_EVENT_LOG_TABLE_NAME                   "event_log"

/* Event suppression table name */
#define FM_EVENT_SUPPRESSION_TABLE_NAME           "event_suppression"

/* Event suppression table sync script */
#define FM_DB_SYNC_EVENT_SUPPRESSION              "/usr/bin/fm_db_sync_event_suppression.py"

/* Alarm Table Columns */
#define FM_ALARM_COLUMN_CREATED_AT                "created_at"
#define FM_ALARM_COLUMN_UPDATED_AT                "updated_at"
#define FM_ALARM_COLUMN_DELETED_AT                "deleted_at"
#define FM_ALARM_COLUMN_ID                        "id"
#define FM_ALARM_COLUMN_UUID                      "uuid"
#define FM_ALARM_COLUMN_ALARM_ID                  "alarm_id"
#define FM_ALARM_COLUMN_ALARM_STATE               "alarm_state"
#define FM_ALARM_COLUMN_ENTITY_TYPE_ID            "entity_type_id"
#define FM_ALARM_COLUMN_ENTITY_INSTANCE_ID        "entity_instance_id"
#define FM_ALARM_COLUMN_TIMESTAMP                 "timestamp"
#define FM_ALARM_COLUMN_SEVERITY                  "severity"
#define FM_ALARM_COLUMN_REASON_TEXT               "reason_text"
#define FM_ALARM_COLUMN_ALARM_TYPE                "alarm_type"
#define FM_ALARM_COLUMN_PROBABLE_CAUSE            "probable_cause"
#define FM_ALARM_COLUMN_PROPOSED_REPAIR_ACTION    "proposed_repair_action"
#define FM_ALARM_COLUMN_SERVICE_AFFECTING         "service_affecting"
#define FM_ALARM_COLUMN_SUPPRESSION               "suppression"
#define FM_ALARM_COLUMN_INHIBIT_ALARMS            "inhibit_alarms"
#define FM_ALARM_COLUMN_MASKED                    "masked"

/* Event Log Table Columns */
#define FM_EVENT_LOG_COLUMN_CREATED_AT               "created_at"
#define FM_EVENT_LOG_COLUMN_UPDATED_AT               "updated_at"
#define FM_EVENT_LOG_COLUMN_DELETED_AT               "deleted_at"
#define FM_EVENT_LOG_COLUMN_ID                       "id"
#define FM_EVENT_LOG_COLUMN_UUID                     "uuid"
#define FM_EVENT_LOG_COLUMN_EVENT_ID                 "event_log_id"
#define FM_EVENT_LOG_COLUMN_STATE                    "state"
#define FM_EVENT_LOG_COLUMN_ENTITY_TYPE_ID           "entity_type_id"
#define FM_EVENT_LOG_COLUMN_ENTITY_INSTANCE_ID       "entity_instance_id"
#define FM_EVENT_LOG_COLUMN_TIMESTAMP                "timestamp"
#define FM_EVENT_LOG_COLUMN_SEVERITY                 "severity"
#define FM_EVENT_LOG_COLUMN_REASON_TEXT              "reason_text"
#define FM_EVENT_LOG_COLUMN_EVENT_TYPE               "event_log_type"
#define FM_EVENT_LOG_COLUMN_PROBABLE_CAUSE           "probable_cause"
#define FM_EVENT_LOG_COLUMN_PROPOSED_REPAIR_ACTION   "proposed_repair_action"
#define FM_EVENT_LOG_COLUMN_SERVICE_AFFECTING        "service_affecting"
#define FM_EVENT_LOG_COLUMN_SUPPRESSION              "suppression"

/* Event Suppression Table Columns */
#define FM_EVENT_SUPPRESSION_COLUMN_CREATED_AT             "created_at"
#define FM_EVENT_SUPPRESSION_COLUMN_UPDATED_AT             "updated_at"
#define FM_EVENT_SUPPRESSION_COLUMN_DELETED_AT             "deleted_at"
#define FM_EVENT_SUPPRESSION_COLUMN_ID                     "id"
#define FM_EVENT_SUPPRESSION_COLUMN_UUID                   "uuid"
#define FM_EVENT_SUPPRESSION_COLUMN_ALARM_ID               "alarm_id"
#define FM_EVENT_SUPPRESSION_COLUMN_DESCRIPTION            "description"
#define FM_EVENT_SUPPRESSION_COLUMN_SUPPRESSION_STATUS     "suppression_status"

#define FM_EVENT_SUPPRESSION_SUPPRESSED     "suppressed"
#define FM_EVENT_SUPPRESSION_UNSUPPRESSED   "unsuppressed"
#define FM_EVENT_SUPPRESSION_NONE           "None"


#define FM_ENTITY_ROOT_KEY           "system="
#define FM_ENTITY_REGION_KEY         "region="

/* config keys */
#define FM_SQL_CONNECTION            "sql_connection"
#define FM_EVENT_LOG_MAX_SIZE        "event_log_max_size"
#define FM_SYSTEM_NAME               "system_name"
#define FM_REGION_NAME               "region_name"
#define FM_DEBUG_FLAG                "debug"
#define FM_STRING_TRUE               "True"

#define CLEAR_ALL_REASON_TEXT        "System initiated hierarchical alarm clear"

#endif /* FMDBCONSTANTS_H_ */

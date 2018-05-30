//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include <string>

#define FM_ALARM_CLEAR  6
#define FM_ALARM_HIERARCHICAL_CLEAR 7
#define FM_ALARM_MESSAGE 8

#define FM_WARM_START 9

#define FM_CUSTOMER_LOG 10

/* Trap Destination table name */
#define FM_TRAPDEST_TABLE_NAME       "i_trap_destination"

#define FM_TRAPDEST_IP_COLUMN        "ip_address"

#define FM_TRAPDEST_COMM_COLUMN      "community"

/* MIB Trap definitions */
const std::string WRS_ALARM_MIB = "WRS-ALARM-MIB";

const std::string ALARM_CRITICAL = "wrsAlarmCritical";
const std::string ALARM_MAJOR = "wrsAlarmMajor";
const std::string ALARM_MINOR = "wrsAlarmMinor";
const std::string ALARM_WARNING = "wrsAlarmWarning";
const std::string ALARM_MSG = "wrsAlarmMessage";
const std::string ALARM_CLEAR = "wrsAlarmClear";
const std::string ALARM_HIERARCHICAL_CLEAR = "wrsAlarmHierarchicalClear";

const std::string ALARM_ID = "wrsAlarmActiveAlarmId";
const std::string ALARM_INSTANCE_ID = "wrsAlarmActiveEntityInstanceId";
const std::string ALARM_DATE_TIME = "wrsAlarmActiveDateAndTime";
const std::string ALARM_SEVERITY = "wrsAlarmActiveAlarmSeverity";
const std::string ALARM_REASON_TEXT = "wrsAlarmActiveReasonText";
const std::string ALARM_EVENT_TYPE = "wrsAlarmActiveEventType";
const std::string ALARM_CAUSE = "wrsAlarmActiveProbableCause";
const std::string ALARM_REPAIR_ACTION = "wrsAlarmActiveProposedRepairAction";
const std::string ALARM_SERVICE_AFFECTING = "wrsAlarmActiveServiceAffecting";
const std::string ALARM_SUPPRESSION = "wrsAlarmActiveSuppressionAllowed";

const std::string CUSTOMER_LOG_ID = "wrsCustomerLogId";
const std::string CUSTOMER_LOG_INSTANCE_ID = "wrsCustomerLogEntityInstanceId";
const std::string CUSTOMER_LOG_DATE_TIME = "wrsCustomerLogDateAndTime";
const std::string CUSTOMER_LOG_SEVERITY = "wrsCustomerLogSeverity";
const std::string CUSTOMER_LOG_REASON_TEXT = "wrsCustomerLogReasonText";
const std::string CUSTOMER_LOG_EVENT_TYPE = "wrsCustomerLogEventType";
const std::string CUSTOMER_LOG_CAUSE = "wrsCustomerLogProbableCause";
const std::string CUSTOMER_LOG_SERVICE_AFFECTING = "wrsCustomerLogServiceAffecting";

const std::string SNMPv2_MIB = "SNMPv2-MIB";
const std::string WARM_START = "warmStart";

const std::string TRAP_CMD = "/usr/bin/snmptrap -v 2c";
const std::string CLEAR_REASON_TEXT = "System initiated hierarchical alarm clear";

const std::string SEP = " ";
const std::string SCOPE = "::";
const std::string STR_TYPE = " s ";
const std::string INT_TYPE = " i ";
const std::string OPTION_COMM = " -c ";

const std::string DC_COMM_STR = "dcorchAlarmAggregator";
const std::string CONF_PATH_ENV = "SNMPCONFPATH=";
const std::string CONF_DIR = "/etc/snmp";

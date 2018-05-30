//
// Copyright (c) 2017 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef _FM_API_H
#define _FM_API_H

#include <string.h>
#include <stdio.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif


#define FM_MAX_BUFFER_LENGTH  255

/* unsigned 64-bit data, 8-byte alignment, Time in microsecond */
typedef unsigned long long int  FMTimeT __attribute__((__aligned__(8)));

typedef unsigned char    FMBoolTypeT;

#define FM_TRUE      1   //any non-zero value is also considered to be True

#define FM_FALSE     0

typedef enum{
  FM_ALARM_STATE_CLEAR = 0,
  FM_ALARM_STATE_SET = 1,
  FM_ALARM_STATE_MSG = 2,
  FM_ALARM_STATE_LOG = 3,   // Use only for SNMP Agent
  FM_ALARM_STATE_MAX = 4
}EFmAlarmStateT;

typedef enum{
  FM_ALARM_SEVERITY_CLEAR = 0,
  FM_ALARM_SEVERITY_WARNING = 1,
  FM_ALARM_SEVERITY_MINOR = 2,
  FM_ALARM_SEVERITY_MAJOR = 3,
  FM_ALARM_SEVERITY_CRITICAL = 4,
  FM_ALARM_SEVERITY_MAX = 5
}EFmAlarmSeverityT;

typedef enum{
  FM_ALARM_TYPE_UNKNOWN = 0,
  FM_ALARM_COMM = 1,               //communication
  FM_ALARM_QOS = 2,                //qos
  FM_ALARM_PROCESSING_ERROR = 3,   //processing-error
  FM_ALARM_EQUIPMENT = 4 ,         //equipment
  FM_ALARM_ENVIRONMENTAL = 5,      //environmental
  FM_ALARM_INTERGRITY = 6,         //integrity-violation
  FM_ALARM_OPERATIONAL = 7,        //operational-violation
  FM_ALARM_PHYSICAL = 8,           //physical-violation
  FM_ALARM_SECURITY = 9,           //security-service-or-mechanism-violation
  FM_ALARM_TIME = 10,              //time-domain-violation
  FM_ALARM_TYPE_MAX = 11
}EFmAlarmTypeT;

typedef enum{
  FM_ALARM_CAUSE_UNKNOWN = 0,
  FM_ALARM_ADAPTOR_ERROR = 1,              //adaptor-error
  FM_ALARM_APP_SUBSYS_FAILURE = 2,         //application-subsystem-failure
  FM_ALARM_BANDWIDTH_REDUCED = 3,          //bandwidth-reduced
  FM_ALARM_CALL_ERROR = 4 ,                //call-establishment-error
  FM_ALARM_COMM_PROTOCOL_ERROR = 5,        //communication-protocol-error
  FM_ALARM_COMM_SUBSYS_FAILURE = 6,        //communication-subsystem-failure
  FM_ALARM_CONFIG_ERROR = 7,               //configuration-or-customization-error
  FM_ALARM_CONGESTION = 8,                 //congestion
  FM_ALARM_CORRUPT_DATA = 9,               //corrupt-data
  FM_ALARM_CPU_LIMITED_EXCEEDED = 10,      //cpu-cycles-limit-exceeded
  FM_ALARM_DATASET_ERROR = 11,             //dataset-or-modem-error
  FM_ALARM_DEGRADED_SIGNAL = 12,           //degraded-signal
  FM_ALARM_DTE_DCE_INTERFACE_ERROR = 13,   //dte-dce-interface-error
  FM_ALARM_DOOR_OPEN = 14,                 //enclosure-door-open',
  FM_ALARM_EQUIPMENT_MALFUNCTION = 15,     //equipment-malfunction
  FM_ALARM_EXCESSIVE_VIBRATION = 16,       //excessive-vibration'
  FM_ALARM_FILE_ERROR = 17,                //file-error
  FM_ALARM_FIRE_DETECTED = 18,             //fire-detected
  FM_ALARM_FLOOD_DETECTED = 19,            //flood-detected
  FM_ALARM_FRAMING_ERROR = 20,             //framing-error
  FM_ALARM_COOLING_PROBLEM = 21,           //heating-ventilation-cooling-system-problem
  FM_ALARM_HUMIDITY_UNACCEPTABLE = 22,     //humidity-unacceptable
  FM_ALARM_IO_DEVICE_ERROR = 23,           //io-device-error
  FM_ALARM_INPUT_DEVICE_ERROR = 24,        //input-device-error
  FM_ALARM_LAN_ERROR = 25,                 //lan-error
  FM_ALARM_LEAK_DETECTED = 26,             //leak-detected
  FM_ALARM_LOCAL_TX_ERROR = 27,            //local-node-transmission-error
  FM_ALARM_LOSS_OF_FRAME = 28,             //loss-of-frame
  FM_ALARM_LOSS_OF_SIGNAL = 29,             //loss-of-signal
  FM_ALARM_MATERIAL_SUPPlY_EXHAUSTED = 30,  //material-supply-exhausted
  FM_ALARM_MULTIPLEXER_PROBLEM = 31,        //multiplexer-problem
  FM_ALARM_OUT_OF_MEMORY = 32,              //out-of-memory',
  FM_ALARM_OUTPUT_DEVICE_ERROR = 33,        //output-device-error
  FM_ALARM_PERMFORMANCE_DEGRADED = 34,      //performance-degraded
  FM_ALARM_POWER_PROBLEM = 35,              //power-problem
  FM_ALARM_PROCESSOR_PROBLEM = 36,          //processor-problem
  FM_ALARM_PUMP_FAILURE = 37,               //pump-failure
  FM_ALARM_Q_SIZE_EXCEEDED = 38,            //queue-size-exceeded
  FM_ALARM_RX_FAILURE = 39,                 //receive-failure
  FM_ALARM_RXER_FAILURE = 40,               //receiver-failure
  FM_ALARM_REMOTE_TX_ERROR = 41,            //remote-node-transmission-error
  FM_ALARM_RESOURCE_NEAR_CAPACITY = 42,     //resource-at-or-nearing-capacity
  FM_ALARM_RESPONSE_TIME_EXCESSIVE = 43,    //response-time-excessive
  FM_ALARM_RETX_RATE_EXCESSIVE = 44,        //retransmission-rate-excessive
  FM_ALARM_SOFTWARE_ERROR = 45,              //software-error
  FM_ALARM_PROGRAM_TERMINATED = 46,    //software-program-abnormally-terminated
  FM_ALARM_PROGRAM_ERROR = 47,         //software-program-error
  FM_ALARM_STORAGE_PROBLEM = 48,       //storage-capacity-problem
  FM_ALARM_TEMP_UNACCEPTABLE = 49,     //temperature-unacceptable
  FM_ALARM_THRESHOLD_CROSSED = 50,     //threshold-crossed
  FM_ALARM_TIMING_PROBLEM = 51,        //timing-problem
  FM_ALARM_TOXIC_LEAK_DETECTED = 52,   //toxic-leak-detected
  FM_ALARM_TRANSMIT_FAILURE = 53,      //transmit-failure
  FM_ALARM_TRANSMITTER_FAILURE = 54,   //transmitter-failure
  FM_ALARM_UNDERLYING_RESOURCE_UNAVAILABLE = 55,//underlying-resource-unavailable
  FM_ALARM_VERSION_MISMATCH = 56,      //version-mismatch
  FM_ALARM_DUPLICATE_INFO = 57,       //duplicate-information
  FM_ALARM_INFO_MISSING = 58,         //information-missing
  FM_ALARM_INFO_MODIFICATION = 59,    //information-modification-detected
  FM_ALARM_INFO_OUT_OF_SEQ = 60,     //information-out-of-sequence
  FM_ALARM_UNEXPECTED_INFO = 61,     //unexpected-information
  FM_ALARM_DENIAL_OF_SERVICE = 62,   //denial-of-service
  FM_ALARM_OUT_OF_SERVICE = 63,      //out-of-service
  FM_ALARM_PROCEDURAL_ERROR = 64,    //procedural-error
  FM_ALARM_UNSPECIFIED_REASON = 65,  //unspecified-reason
  FM_ALARM_CABLE_TAMPER = 66,        //cable-tamper
  FM_ALARM_INTRUSION_DETECTION = 67,  //intrusion-detection
  FM_ALARM_AUTH_FAILURE = 68,        //authentication-failure
  FM_ALARM_BREACH_CONFIDENT = 69,    //breach-of-confidentiality
  FM_ALARM_NON_REPUD_FAILURE = 70,   //non-repudiation-failure
  FM_ALARM_UNAUTH_ACCESS_ATTEMP = 71,  //unauthorized-access-attempt
  FM_ALARM_DELAYED_INFO = 72,         //delayed-information
  FM_ALARM_KEY_EXPIRED = 73,         //key-expired
  FM_ALARM_OUT_OF_HR_ACTIVITY = 74,  //out-of-hours-activity
  FM_ALARM_CAUSE_MAX = 75
}EFmAlarmProbableCauseT;


typedef char fm_uuid_t[FM_MAX_BUFFER_LENGTH];
typedef char fm_ent_inst_t[FM_MAX_BUFFER_LENGTH];
typedef char fm_alarm_id[FM_MAX_BUFFER_LENGTH];

typedef struct{
  fm_uuid_t uuid;       //generated by FM system
  char alarm_id[FM_MAX_BUFFER_LENGTH];   //structured id for the fault
  EFmAlarmStateT alarm_state;                 //state of the fault
  char entity_type_id[FM_MAX_BUFFER_LENGTH];//type of the object raising fault
  char entity_instance_id[FM_MAX_BUFFER_LENGTH];//instance information of the object raising fault
  FMTimeT timestamp;                 //time in UTC at which the fault state is last updated
  EFmAlarmSeverityT severity;  //severity of the fault
  char reason_text[FM_MAX_BUFFER_LENGTH];
  EFmAlarmTypeT alarm_type;
  EFmAlarmProbableCauseT probable_cause;
  char proposed_repair_action[FM_MAX_BUFFER_LENGTH];
  FMBoolTypeT service_affecting;
  FMBoolTypeT suppression;  //'allowed' or 'not-allowed'
  FMBoolTypeT inhibit_alarms; //hierarchical suppression of alarms if it is set to true
}SFmAlarmDataT;

typedef enum{
  FM_ERR_OK = 0,
  FM_ERR_ALARM_EXISTS = 1,
  FM_ERR_INVALID_ATTRIBUTE = 2,
  FM_ERR_NOCONNECT=3,
  FM_ERR_NOMEM=4,
  FM_ERR_COMMUNICATIONS=5,
  FM_ERR_NOT_ENOUGH_SPACE=6,
  FM_ERR_INVALID_REQ=7,
  FM_ERR_SERVER_NO_MEM=8,
  FM_ERR_SCRIPT_FAILURE=9,
  FM_ERR_ENTITY_NOT_FOUND = 10,
  FM_ERR_DB_OPERATION_FAILURE = 11,
  FM_ERR_DB_CONNECT_FAILURE = 12,
  FM_ERR_INVALID_PARAMETER = 13,
  FM_ERR_RESOURCE_UNAVAILABLE = 14,
  FM_ERR_REQUEST_PENDING = 15,
  FM_ERR_MAX
}EFmErrorT;


typedef struct {
  char alarm_id[FM_MAX_BUFFER_LENGTH];
  fm_ent_inst_t  entity_instance_id;
}AlarmFilter ;


EFmErrorT fm_set_fault(const SFmAlarmDataT *alarm, fm_uuid_t *uuid);

EFmErrorT fm_clear_fault(AlarmFilter *filter);

EFmErrorT fm_clear_all(fm_ent_inst_t *inst_id);

EFmErrorT fm_get_fault(AlarmFilter *filter, SFmAlarmDataT *alarm);

EFmErrorT fm_get_faults(fm_ent_inst_t *inst_id, SFmAlarmDataT *alarm,
		                unsigned int *max_alarms_to_get);

EFmErrorT fm_get_faults_by_id(fm_alarm_id *alarm_id, SFmAlarmDataT *alarm,
						unsigned int *max_alarms_to_get);


/*
 * APIs that enqueue the request and return ok for acknowledgment.
 * It is up to the client to query and find out whether
 * the alarm is raised or cleared successfully
 */
EFmErrorT fm_set_fault_async(const SFmAlarmDataT *alarm, fm_uuid_t *uuid);

EFmErrorT fm_clear_fault_async(AlarmFilter *filter);

EFmErrorT fm_clear_all_async(fm_ent_inst_t *inst_id);

//used by fmManager
EFmErrorT fm_server_create(const char *fn) ;


#ifdef __cplusplus
}
#endif

#endif /* _FM_API_H */

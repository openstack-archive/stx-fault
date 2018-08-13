//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include <stdlib.h>
#include <pthread.h>
#include <map>
#include <vector>
#include <uuid/uuid.h>

#include "fmAlarmUtils.h"
#include "fmDbUtils.h"
#include "fmLog.h"


typedef std::map<int,std::string> itos_t;
typedef std::map<std::string,int> stoi_t;
typedef std::vector<std::string> strvect_t;

static pthread_mutex_t mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

static itos_t state_to_str;
static itos_t severity_to_str;
static itos_t type_to_str;
static itos_t cause_to_str;
static itos_t bool_to_str;
static itos_t err_to_str;


static stoi_t state_to_int;
static stoi_t severity_to_int;
static stoi_t type_to_int;
static stoi_t cause_to_int;
static stoi_t bool_to_int;
static stoi_t err_to_int;


#define STRCP_TO(charb,str)\
		if (((str).length()==0) ||	\
			((str).length()==1 && ((str).c_str())[0]==' ')) { \
			memset(charb,0,sizeof(charb)); \
		} else {  \
			strncpy((charb),str.c_str(),sizeof(charb)); \
		}

void add_both_tables(int id, const char *str, itos_t &t1,stoi_t &t2 ) {
	t1[id]=str;
	t2[str]=id;
}

static void init_tables() {
	pthread_mutex_lock(&mutex);
	static bool has_inited=false;

	while (!has_inited) {
		add_both_tables(FM_ALARM_STATE_CLEAR,"clear",state_to_str,state_to_int);
		add_both_tables(FM_ALARM_STATE_SET,"set",state_to_str,state_to_int);
		add_both_tables(FM_ALARM_STATE_MSG,"msg",state_to_str,state_to_int);
		add_both_tables(FM_ALARM_STATE_LOG,"log",state_to_str,state_to_int);

		add_both_tables(FM_ALARM_SEVERITY_CLEAR,"not-applicable",severity_to_str,severity_to_int);
		add_both_tables(FM_ALARM_SEVERITY_WARNING,"warning",severity_to_str,severity_to_int);
		add_both_tables(FM_ALARM_SEVERITY_MINOR,"minor",severity_to_str,severity_to_int);
		add_both_tables(FM_ALARM_SEVERITY_MAJOR,"major",severity_to_str,severity_to_int);
		add_both_tables(FM_ALARM_SEVERITY_CRITICAL,"critical",severity_to_str,severity_to_int);

		add_both_tables(FM_ALARM_TYPE_UNKNOWN,"other",type_to_str,type_to_int);
		add_both_tables(FM_ALARM_COMM,"communication",type_to_str,type_to_int);
		add_both_tables(FM_ALARM_QOS,"qos",type_to_str,type_to_int);
		add_both_tables(FM_ALARM_PROCESSING_ERROR,"processing-error",type_to_str,type_to_int);
		add_both_tables(FM_ALARM_EQUIPMENT,"equipment",type_to_str,type_to_int);
		add_both_tables(FM_ALARM_ENVIRONMENTAL,"environmental",type_to_str,type_to_int);
		add_both_tables(FM_ALARM_INTERGRITY,"integrity-violation",type_to_str,type_to_int);
		add_both_tables(FM_ALARM_OPERATIONAL,"operational-violation",type_to_str,type_to_int);
		add_both_tables(FM_ALARM_PHYSICAL,"physical-violation",type_to_str,type_to_int);
		add_both_tables(FM_ALARM_SECURITY,"security-service-or-mechanism-violation",type_to_str,type_to_int);
		add_both_tables(FM_ALARM_TIME,"time-domain-violation",type_to_str,type_to_int);

		add_both_tables(  FM_ALARM_CAUSE_UNKNOWN ,"not-applicable",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_ADAPTOR_ERROR ,"adaptor-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_APP_SUBSYS_FAILURE ,"application-subsystem-failure",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_BANDWIDTH_REDUCED ,"bandwidth-reduced",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_CALL_ERROR ,"call-establishment-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_COMM_PROTOCOL_ERROR ,"communication-protocol-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_COMM_SUBSYS_FAILURE ,"communication-subsystem-failure",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_CONFIG_ERROR ,"configuration-or-customization-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_CONGESTION ,"congestion",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_CORRUPT_DATA ,"corrupt-data",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_CPU_LIMITED_EXCEEDED ,"cpu-cycles-limit-exceeded",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_DATASET_ERROR ,"dataset-or-modem-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_DEGRADED_SIGNAL ,"degraded-signal",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_DTE_DCE_INTERFACE_ERROR ,"dte-dce-interface-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_DOOR_OPEN ,"enclosure-door-open",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_EQUIPMENT_MALFUNCTION ,"equipment-malfunction",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_EXCESSIVE_VIBRATION ,"excessive-vibration'",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_FILE_ERROR ,"file-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_FIRE_DETECTED ,"fire-detected",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_FLOOD_DETECTED ,"flood-detected",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_FRAMING_ERROR ,"framing-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_COOLING_PROBLEM ,"heating-ventilation-cooling-system-problem",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_HUMIDITY_UNACCEPTABLE ,"humidity-unacceptable",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_IO_DEVICE_ERROR ,"io-device-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_INPUT_DEVICE_ERROR ,"input-device-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_LAN_ERROR ,"lan-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_LEAK_DETECTED ,"leak-detected",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_LOCAL_TX_ERROR ,"local-node-transmission-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_LOSS_OF_FRAME ,"loss-of-frame",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_LOSS_OF_SIGNAL ,"loss-of-signal",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_MATERIAL_SUPPlY_EXHAUSTED ,"material-supply-exhausted",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_MULTIPLEXER_PROBLEM ,"multiplexer-problem",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_OUT_OF_MEMORY ,"out-of-memory",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_OUTPUT_DEVICE_ERROR ,"output-device-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_PERMFORMANCE_DEGRADED ,"performance-degraded",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_POWER_PROBLEM ,"power-problem",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_PROCESSOR_PROBLEM ,"processor-problem",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_PUMP_FAILURE ,"pump-failure",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_Q_SIZE_EXCEEDED ,"queue-size-exceeded",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_RX_FAILURE ,"receive-failure",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_RXER_FAILURE ,"receiver-failure",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_REMOTE_TX_ERROR ,"remote-node-transmission-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_RESOURCE_NEAR_CAPACITY ,"resource-at-or-nearing-capacity",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_RESPONSE_TIME_EXCESSIVE ,"response-time-excessive",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_RETX_RATE_EXCESSIVE ,"retransmission-rate-excessive",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_SOFTWARE_ERROR ,"software-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_PROGRAM_TERMINATED ,"software-program-abnormally-terminated",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_PROGRAM_ERROR ,"software-program-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_STORAGE_PROBLEM ,"storage-capacity-problem",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_TEMP_UNACCEPTABLE ,"temperature-unacceptable",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_THRESHOLD_CROSSED ,"threshold-crossed",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_TIMING_PROBLEM ,"timing-problem",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_TOXIC_LEAK_DETECTED ,"toxic-leak-detected",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_TRANSMIT_FAILURE ,"transmit-failure",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_TRANSMITTER_FAILURE ,"transmitter-failure",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_UNDERLYING_RESOURCE_UNAVAILABLE ,"underlying-resource-unavailable",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_VERSION_MISMATCH ,"version-mismatch",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_DUPLICATE_INFO ,"duplicate-information",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_INFO_MISSING ,"information-missing",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_INFO_MODIFICATION ,"information-modification-detected",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_INFO_OUT_OF_SEQ ,"information-out-of-sequence",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_UNEXPECTED_INFO ,"unexpected-information",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_DENIAL_OF_SERVICE ,"denial-of-service",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_OUT_OF_SERVICE ,"out-of-service",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_PROCEDURAL_ERROR ,"procedural-error",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_UNSPECIFIED_REASON ,"unspecified-reason",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_CABLE_TAMPER ,"cable-tamper",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_INTRUSION_DETECTION ,"intrusion-detection",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_AUTH_FAILURE ,"authentication-failure",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_BREACH_CONFIDENT ,"breach-of-confidentiality",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_NON_REPUD_FAILURE ,"non-repudiation-failure",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_UNAUTH_ACCESS_ATTEMP ,"unauthorized-access-attempt",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_DELAYED_INFO ,"delayed-information",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_KEY_EXPIRED ,"key-expired",cause_to_str,cause_to_int);
		add_both_tables(  FM_ALARM_OUT_OF_HR_ACTIVITY ,"out-of-hours-activity",cause_to_str,cause_to_int);

		add_both_tables(  FM_ERR_OK, "Ok", err_to_str, err_to_int);
		add_both_tables(  FM_ERR_ALARM_EXISTS, "FM_ERR_ALARM_EXISTS", err_to_str, err_to_int);
		add_both_tables(  FM_ERR_INVALID_ATTRIBUTE, "FM_ERR_INVALID_ATTRIBUTE", err_to_str, err_to_int);
		add_both_tables(  FM_ERR_ENTITY_NOT_FOUND, "FM_ERR_ENTITY_NOT_FOUND", err_to_str, err_to_int);
		add_both_tables(  FM_ERR_DB_OPERATION_FAILURE, "FM_ERR_DB_OPERATION_FAILURE", err_to_str, err_to_int);
		add_both_tables(  FM_ERR_SCRIPT_FAILURE, "FM_ERR_SCRIPT_FAILURE",err_to_str, err_to_int);
		add_both_tables(  FM_ERR_NOCONNECT, "FM_ERR_NOCONNECT", err_to_str, err_to_int);
		add_both_tables(  FM_ERR_NOMEM, "FM_ERR_NOMEM", err_to_str, err_to_int);
		add_both_tables(  FM_ERR_COMMUNICATIONS, "FM_ERR_COMMUNICATIONS", err_to_str, err_to_int);
		add_both_tables(  FM_ERR_NOT_ENOUGH_SPACE, "FM_ERR_NOT_ENOUGH_SPACE", err_to_str, err_to_int);
		add_both_tables(  FM_ERR_INVALID_REQ, "FM_ERR_INVALID_REQ", err_to_str, err_to_int);
		add_both_tables(  FM_ERR_SERVER_NO_MEM, "FM_ERR_SERVER_NO_MEM", err_to_str, err_to_int);
		add_both_tables(  FM_ERR_INVALID_PARAMETER, "FM_ERR_INVALID_PARAMETER",err_to_str, err_to_int);
		add_both_tables(  FM_ERR_RESOURCE_UNAVAILABLE, "FM_ERR_RESOURCE_UNAVAILABLE",err_to_str, err_to_int);

		add_both_tables(  0 ,"False",bool_to_str,bool_to_int);
		add_both_tables(  1 ,"True",bool_to_str,bool_to_int);

		has_inited=true;
	}
	pthread_mutex_unlock(&mutex);
}

static void add_s(std::string &s) { s+="###"; };

static std::string tostr(int id, const itos_t &t ){
	itos_t::const_iterator it = t.find(id);
	if (it!=t.end()) return it->second;
	return "unknown";
}
static int toint(const std::string &s, const stoi_t &t) {
	stoi_t::const_iterator it = t.find(s);
	if (it!=t.end()) return it->second ;
	return 0;
}

static std::string chkstr(const std::string &s) {
	if (s.length()==0) return " ";
	return s;
}

static void str_to_vector(const std::string &s, std::vector<std::string> &alarm) {
	size_t offset = 0;
	alarm.clear();
	while (true) {
		size_t beg = (offset==0) ? 0 : s.find("###",offset);
		if (beg==std::string::npos) break;
		size_t e = s.find("###",beg+3);
		std::string cont = s.substr(beg+3,e-(beg+3));
		alarm.push_back(cont);
		offset=e;
	}
}

static void fm_set_uuid(SFmAlarmDataT *a, std::string &s, bool is_get) {
	if(is_get) s = chkstr(a->uuid);
	else STRCP_TO(a->uuid,s);
}

static void fm_tr_alarm_id(SFmAlarmDataT *a, std::string &s, bool is_get) {
	if(is_get) s = chkstr(a->alarm_id);
	else STRCP_TO(a->alarm_id,s);
}

static void fm_alarm_state(SFmAlarmDataT *a, std::string &s, bool is_get) {
	if(is_get) s = tostr(a->alarm_state,state_to_str);
	else a->alarm_state = (EFmAlarmStateT)toint(s,state_to_int);
}

static void fm_entity_id(SFmAlarmDataT *a, std::string &s, bool is_get) {
	if(is_get) s = chkstr(a->entity_type_id);
	else STRCP_TO(a->entity_type_id,s);
}

static void fm_instance_id(SFmAlarmDataT *a, std::string &s, bool is_get) {
	if(is_get) s = chkstr(a->entity_instance_id);
	else STRCP_TO(a->entity_instance_id,s);
}

static void fm_timestamp(SFmAlarmDataT *a, std::string &s, bool is_get) {
	if(is_get) {
		fm_db_util_make_timestamp_string(s, a->timestamp);
	} else  {
		fm_db_util_get_timestamp(s.c_str(), a->timestamp);
	}
}

static void fm_alarm_severity(SFmAlarmDataT *a, std::string &s, bool is_get) {
	if(is_get) s = tostr(a->severity,severity_to_str);
	else a->severity = (EFmAlarmSeverityT)toint(s,severity_to_int);
}

static void fm_reason_text(SFmAlarmDataT *a, std::string &s, bool is_get) {
	if(is_get) s = chkstr(a->reason_text);
	else STRCP_TO(a->reason_text,s);
}

static void fm_alarm_type(SFmAlarmDataT *a, std::string &s, bool is_get) {
	if(is_get) s =tostr(a->alarm_type,type_to_str);
	else a->alarm_type = (EFmAlarmTypeT)toint(s,type_to_int);
}

static void fm_prop_cause(SFmAlarmDataT *a, std::string &s, bool is_get) {
	if(is_get) s = tostr(a->probable_cause,cause_to_str);
	else a->probable_cause = (EFmAlarmProbableCauseT)toint(s,cause_to_int);
}

static void fm_repair(SFmAlarmDataT *a, std::string &s, bool is_get) {
	if(is_get) s = chkstr(a->proposed_repair_action);
	else STRCP_TO(a->proposed_repair_action,s);
}

static void fm_service_affect(SFmAlarmDataT *a, std::string &s, bool is_get) {
	if(is_get) s = tostr(a->service_affecting,bool_to_str);
	else a->service_affecting = (((s == "t") || (s == "True"))? 1 :0);
}

static void fm_suppression(SFmAlarmDataT *a, std::string &s, bool is_get) {
	if(is_get) s = tostr(a->suppression,bool_to_str);
	else a->suppression = (((s == "t") || (s == "True"))? 1 :0);
}

static void fm_inhibit_alarm(SFmAlarmDataT *a, std::string &s, bool is_get) {
	if(is_get) s = tostr(a->inhibit_alarms,bool_to_str);
	else a->inhibit_alarms = (((s == "t") || (s == "True"))? 1 :0);
}

typedef void (*set_get_field_type)(SFmAlarmDataT *a, std::string &s, bool is_get);

struct alarm_struct_update_t{
	EFmAlarmIndexMap id;
	set_get_field_type func;
} fm_alarm_convert_func [] = {
		{ FM_ALM_IX_UUID, fm_set_uuid },
		{ FM_ALM_IX_ALARM_ID, fm_tr_alarm_id },
		{ FM_ALM_IX_ALARM_STATE, fm_alarm_state },
		{ FM_ALM_IX_ENTITY_ID, fm_entity_id },
		{ FM_ALM_IX_INSTANCE_ID, fm_instance_id },
		{ FM_ALM_IX_TIMESTAMP, fm_timestamp },
		{ FM_ALM_IX_SEVERITY, fm_alarm_severity },
		{ FM_ALM_IX_REASON, fm_reason_text },
		{ FM_ALM_IX_ALARM_TYPE, fm_alarm_type },
		{ FM_ALM_IX_PROBABLE_CAUSE, fm_prop_cause },
		{ FM_ALM_IX_REPAIR_ACTION, fm_repair },
		{ FM_ALM_IX_SERVICE_AFFECT, fm_service_affect },
		{ FM_ALM_IX_SUPPRESSION, fm_suppression },
		{ FM_ALM_IX_INHIBIT_ALARM, fm_inhibit_alarm }
};


static set_get_field_type find_func_set_get(EFmAlarmIndexMap id) {
	size_t ix = 0;
	size_t mx = sizeof (fm_alarm_convert_func)/sizeof(*fm_alarm_convert_func);
	for ( ; ix < mx ; ++ix ) {
		if (fm_alarm_convert_func[ix].id==id)
			return fm_alarm_convert_func[ix].func;
	}
	return NULL;
}

bool fm_alarm_set_field(EFmAlarmIndexMap field, SFmAlarmDataT *a, std::string &val) {
	init_tables();
	set_get_field_type p = find_func_set_get(field);
	if (p==NULL) return false;
	p(a,val,false);

	return true;
}

struct log_struct_update_t{
	EFmLogIndexMap id;
	set_get_field_type func;
} fm_log_convert_func [] = {
		{ FM_LOG_IX_UUID, fm_set_uuid },
		{ FM_LOG_IX_LOG_ID, fm_tr_alarm_id },
		{ FM_LOG_IX_ENTITY_ID, fm_entity_id },
		{ FM_LOG_IX_INSTANCE_ID, fm_instance_id },
		{ FM_LOG_IX_TIMESTAMP, fm_timestamp },
		{ FM_LOG_IX_SEVERITY, fm_alarm_severity },
		{ FM_LOG_IX_REASON, fm_reason_text },
		{ FM_LOG_IX_LOG_TYPE, fm_alarm_type },
		{ FM_LOG_IX_PROBABLE_CAUSE, fm_prop_cause },
		{ FM_LOG_IX_SERVICE_AFFECT, fm_service_affect }
};

static set_get_field_type fm_log_find_func_set_get(EFmLogIndexMap id) {
	size_t ix = 0;
	size_t mx = sizeof (fm_log_convert_func)/sizeof(*fm_log_convert_func);
	for ( ; ix < mx ; ++ix ) {
		if (fm_log_convert_func[ix].id==id)
			return fm_log_convert_func[ix].func;
	}
	return NULL;
}
bool fm_log_set_field(EFmLogIndexMap field, SFmAlarmDataT *a, std::string &val) {
	init_tables();
	set_get_field_type p = fm_log_find_func_set_get(field);
	if (p==NULL) return false;
	p(a,val,false);

	return true;
}

struct event_log_struct_update_t{
	EFmEventLogIndexMap id;
	set_get_field_type func;
} fm_event_log_convert_func [] = {
		{ FM_EVENT_LOG_IX_UUID, fm_set_uuid },
		{ FM_EVENT_LOG_IX_EVENT_ID, fm_tr_alarm_id },
		{ FM_EVENT_LOG_IX_STATE, fm_alarm_state },
		{ FM_EVENT_LOG_IX_ENTITY_ID, fm_entity_id },
		{ FM_EVENT_LOG_IX_INSTANCE_ID, fm_instance_id },
		{ FM_EVENT_LOG_IX_TIMESTAMP, fm_timestamp },
		{ FM_EVENT_LOG_IX_SEVERITY, fm_alarm_severity },
		{ FM_EVENT_LOG_IX_REASON, fm_reason_text },
		{ FM_EVENT_LOG_IX_EVENT_TYPE, fm_alarm_type },
		{ FM_EVENT_LOG_IX_PROBABLE_CAUSE, fm_prop_cause },
		{ FM_EVENT_LOG_IX_REPAIR_ACTION, fm_repair },
		{ FM_EVENT_LOG_IX_SERVICE_AFFECT, fm_service_affect },
		{ FM_EVENT_LOG_IX_SUPPRESSION, fm_suppression }
};

static set_get_field_type fm_event_log_find_func_set_get(EFmEventLogIndexMap id) {
	size_t ix = 0;
	size_t mx = sizeof (fm_event_log_convert_func)/sizeof(*fm_event_log_convert_func);
	for ( ; ix < mx ; ++ix ) {
		if (fm_event_log_convert_func[ix].id==id)
			return fm_event_log_convert_func[ix].func;
	}
	return NULL;
}
bool fm_event_log_set_field(EFmEventLogIndexMap field, SFmAlarmDataT *a, std::string &val) {
	init_tables();
	set_get_field_type p = fm_event_log_find_func_set_get(field);
	if (p==NULL) return false;
	p(a,val,false);

	return true;
}


bool fm_alarm_get_field(EFmAlarmIndexMap field, const SFmAlarmDataT *a, std::string &val) {
	init_tables();
	set_get_field_type p = find_func_set_get(field);
	if (p==NULL) return false;
	p((SFmAlarmDataT*)a,val,true);
	return true;
}

static std::string fm_alarm_to_string(const SFmAlarmDataT *a) {
	std::string s;
	size_t ix = 0;
	size_t mx = FM_ALM_IX_INHIBIT_ALARM ;
	std::string field;
	for ( ; ix <= mx ; ++ix ) {
		fm_alarm_get_field(((EFmAlarmIndexMap)ix),a,field);
		s+=field;
		add_s(s);
	}
	return s;
}

void fm_alarm_to_list(const SFmAlarmDataT *a, std::vector<std::string> &list) {
	size_t ix = 0;
	size_t mx = FM_ALM_IX_INHIBIT_ALARM ;
	std::string field;
	for ( ; ix <= mx ; ++ix ) {
		fm_alarm_get_field(((EFmAlarmIndexMap)ix),a,field);
		list.push_back(field);
	}
}

void fm_formatted_str_to_vector(const std::string &s, std::vector<std::string> &alarm) {
	str_to_vector(s,alarm);
}

/**
 * public APIs
 */

EFmErrorT fm_error_from_string(const std::string &str){
	return (EFmErrorT)toint(str,err_to_int);
}

std::string fm_error_from_int(EFmErrorT id){
	return tostr((int)id,err_to_str);
}

bool fm_alarm_filter_to_string(const AlarmFilter *filter, std::string &str) {
	init_tables();

	str+= chkstr(filter->alarm_id);
	add_s(str);
	str+= chkstr(filter->entity_instance_id);
	add_s(str);
	return true;
}

bool fm_alarm_filter_from_string(const std::string &str, AlarmFilter *filter) {
	strvect_t s;
	str_to_vector(str,s);
	if (s.size()<2) {
		FM_ERROR_LOG("Alarm filter wrong format: %s",str.c_str());
		return false;
	}
	init_tables();

	STRCP_TO(filter->alarm_id,s[0]);
	STRCP_TO(filter->entity_instance_id,s[1]);
	return true;
}

bool fm_alarm_to_string(const SFmAlarmDataT *alarm, std::string &str) {
	init_tables();
	str+= fm_alarm_to_string(alarm);
	return str.size()>0;
}

bool fm_alarm_from_string(const std::string &alstr, SFmAlarmDataT *a) {
	strvect_t s;
	str_to_vector(alstr, s);

	if (s.size()<(FM_ALM_IX_MAX)) {	//includes adjustment for last entry + 1 (for starting at 0)
		return false;
	}

	init_tables();

	size_t ix = 0;
	size_t mx = s.size();
	for ( ; ix < mx ; ++ix ) {
		fm_alarm_set_field((EFmAlarmIndexMap)ix,a,s[ix]);
	}

	return true;
}

void fm_uuid_create(fm_uuid_t &uuid){
	uuid_t uu;

	memset(uuid, 0, sizeof(uuid));
	uuid_generate(uu);
	uuid_unparse_lower(uu, uuid);
}

void fm_log_request(fm_buff_t &req, bool failed){
	SFmMsgHdrT *hdr = ptr_to_hdr(req);
	std::string description;

	if (failed) {
		description.assign("Failed to send FM");
	} else {
		description.assign("Sending FM");
	}
	switch(hdr->action) {
	case EFmCreateFault: {
		SFmAlarmDataT *data = (SFmAlarmDataT * )ptr_to_data(req);
		FM_INFO_LOG("%s raise alarm request: alarm_id (%s), entity_id (%s)",
				description.c_str(), data->alarm_id, data->entity_instance_id);
	}
		break;
	case EFmDeleteFault: {
		AlarmFilter *filter = (AlarmFilter * )ptr_to_data(req);
		FM_INFO_LOG("%s clear alarm request: alarm_id (%s), entity_id (%s)",
				description.c_str(), filter->alarm_id, filter->entity_instance_id);
	}
		break;
	case EFmDeleteFaults: {
		fm_ent_inst_t *entity_id = (fm_ent_inst_t*)ptr_to_data(req);
		fm_ent_inst_t &eid = *entity_id;
		FM_INFO_LOG("%s clear all request: entity_id (%s)", description.c_str(), eid);
	}
		break;
	default:
		FM_ERROR_LOG("Unexpected API action:%u\n", hdr->action);
		break;
	}
}


void fm_log_response(fm_buff_t &req, fm_buff_t &resp, bool failed){
	SFmMsgHdrT *hdr = ptr_to_hdr(req);
	SFmMsgHdrT *resp_hdr = ptr_to_hdr(resp);

	switch(hdr->action) {
	case EFmCreateFault: {
		SFmAlarmDataT *data = (SFmAlarmDataT * )ptr_to_data(req);
		if (failed) {
			FM_WARNING_LOG("Failed to get response for FM raise alarm: alarm_id (%s), entity_id (%s)",
					data->alarm_id, data->entity_instance_id);
		} else {
			FM_INFO_LOG("FM Response for raise alarm: (%d), alarm_id (%s), entity_id (%s)",
					resp_hdr->msg_rc, data->alarm_id, data->entity_instance_id);
		}
	}
		break;
	case EFmDeleteFault: {
		AlarmFilter *filter = (AlarmFilter * )ptr_to_data(req);
		if (failed) {
			FM_WARNING_LOG("Failed to get response for FM clear alarm: alarm_id (%s), entity_id (%s)",
					filter->alarm_id, filter->entity_instance_id);
		} else {
			FM_INFO_LOG("FM Response for clear alarm: (%d), alarm_id (%s), entity_id (%s)",
					resp_hdr->msg_rc, filter->alarm_id, filter->entity_instance_id);
		}
	}
		break;
	case EFmDeleteFaults: {
		fm_ent_inst_t *entity_id = (fm_ent_inst_t*)ptr_to_data(req);
		fm_ent_inst_t &eid = *entity_id;
		if (failed) {
			FM_WARNING_LOG("Failed to get response for FM clear all: entity_id (%s)", eid);
		} else {
			FM_INFO_LOG("FM Response for clear all: (%d), entity_id (%s)",
					resp_hdr->msg_rc, eid);
		}
	}
		break;
	default:
		FM_ERROR_LOG("Unexpected API action:%u\n", hdr->action);
		break;
	}
}


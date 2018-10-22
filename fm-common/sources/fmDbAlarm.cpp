//
// Copyright (c) 2014-2018 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include <stdlib.h>
#include <string>
#include <map>

#include "fmLog.h"
#include "fmDbAlarm.h"
#include "fmAlarmUtils.h"
#include "fmConstants.h"
#include "fmDbUtils.h"

typedef std::map<int,std::string> itos_t;
typedef std::map<std::string,int> stoi_t;

static pthread_mutex_t mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

static itos_t db_alarm_field_ix_to_str;
static stoi_t db_alarm_field_str_to_ix;

static const char * field_map[] = {
    FM_ALARM_COLUMN_CREATED_AT, //0
    FM_ALARM_COLUMN_UPDATED_AT,
    FM_ALARM_COLUMN_DELETED_AT,
    FM_ALARM_COLUMN_ID,
    FM_ALARM_COLUMN_UUID,
    FM_ALARM_COLUMN_ALARM_ID,
    FM_ALARM_COLUMN_ALARM_STATE,
    FM_ALARM_COLUMN_ENTITY_TYPE_ID,
    FM_ALARM_COLUMN_ENTITY_INSTANCE_ID,
    FM_ALARM_COLUMN_TIMESTAMP,
    FM_ALARM_COLUMN_SEVERITY,
    FM_ALARM_COLUMN_REASON_TEXT,
    FM_ALARM_COLUMN_ALARM_TYPE,
    FM_ALARM_COLUMN_PROBABLE_CAUSE,
    FM_ALARM_COLUMN_PROPOSED_REPAIR_ACTION,
    FM_ALARM_COLUMN_SERVICE_AFFECTING,
    FM_ALARM_COLUMN_SUPPRESSION,
    FM_ALARM_COLUMN_INHIBIT_ALARMS,
    FM_ALARM_COLUMN_MASKED //18
};

#define RETRUN_FALSE return false
#define CONTINUE continue
#define STRCP_TO(buffer, source, return_action) \
do{ \
    if (snprintf(buffer, sizeof(buffer), "%s", source.c_str()) < 0) { \
        FM_WARNING_LOG("STRCP_TO fail because of decode error."); \
        return_action; \
    } \
}while(0)

void add_both_tables(const char *str, int id, itos_t &t1,stoi_t &t2 ) {
	t1[id]=str;
	t2[str]=id;
}

static std::string tostr(int id, const itos_t &t ){
	itos_t::const_iterator it = t.find(id);
	if (it!=t.end()) return it->second;
	return "unknown";
}
/*
static int toint(const std::string &s, const stoi_t &t) {
	stoi_t::const_iterator it = t.find(s);
	if (it!=t.end()) return it->second ;
	return 0;
}
*/
static void init_tables() {
	pthread_mutex_lock(&mutex);
	static bool has_inited=false;

	while (!has_inited) {
		add_both_tables(FM_ALARM_COLUMN_UUID,FM_ALM_IX_UUID,db_alarm_field_ix_to_str,db_alarm_field_str_to_ix);
		add_both_tables(FM_ALARM_COLUMN_ALARM_ID,FM_ALM_IX_ALARM_ID,db_alarm_field_ix_to_str,db_alarm_field_str_to_ix);
		add_both_tables(FM_ALARM_COLUMN_ALARM_STATE,FM_ALM_IX_ALARM_STATE,db_alarm_field_ix_to_str,db_alarm_field_str_to_ix);
		add_both_tables(FM_ALARM_COLUMN_ENTITY_TYPE_ID,FM_ALM_IX_ENTITY_ID,db_alarm_field_ix_to_str,db_alarm_field_str_to_ix);
		add_both_tables(FM_ALARM_COLUMN_ENTITY_INSTANCE_ID,FM_ALM_IX_INSTANCE_ID,db_alarm_field_ix_to_str,db_alarm_field_str_to_ix);
		add_both_tables(FM_ALARM_COLUMN_TIMESTAMP,FM_ALM_IX_TIMESTAMP,db_alarm_field_ix_to_str,db_alarm_field_str_to_ix);
		add_both_tables(FM_ALARM_COLUMN_SEVERITY,FM_ALM_IX_SEVERITY,db_alarm_field_ix_to_str,db_alarm_field_str_to_ix);

		add_both_tables(FM_ALARM_COLUMN_REASON_TEXT,FM_ALM_IX_REASON,db_alarm_field_ix_to_str,db_alarm_field_str_to_ix);
		add_both_tables(FM_ALARM_COLUMN_ALARM_TYPE,FM_ALM_IX_ALARM_TYPE,db_alarm_field_ix_to_str,db_alarm_field_str_to_ix);
		add_both_tables(FM_ALARM_COLUMN_PROBABLE_CAUSE,FM_ALM_IX_PROBABLE_CAUSE,db_alarm_field_ix_to_str,db_alarm_field_str_to_ix);
		add_both_tables(FM_ALARM_COLUMN_PROPOSED_REPAIR_ACTION,FM_ALM_IX_REPAIR_ACTION,db_alarm_field_ix_to_str,db_alarm_field_str_to_ix);
		add_both_tables(FM_ALARM_COLUMN_SERVICE_AFFECTING,FM_ALM_IX_SERVICE_AFFECT,db_alarm_field_ix_to_str,db_alarm_field_str_to_ix);
		add_both_tables(FM_ALARM_COLUMN_SUPPRESSION,FM_ALM_IX_SUPPRESSION,db_alarm_field_ix_to_str,db_alarm_field_str_to_ix);
		add_both_tables(FM_ALARM_COLUMN_INHIBIT_ALARMS,FM_ALM_IX_INHIBIT_ALARM,db_alarm_field_ix_to_str,db_alarm_field_str_to_ix);
		has_inited = true;
	}
	pthread_mutex_unlock(&mutex);
}

void append(std::string &str, const std::string &what) {
	str+=what;
	str+="###";
}

bool CFmDbAlarm::import_data(data_type &m ) {
	m_alarm_data = m;
	return true;
}

bool CFmDbAlarm::create_data(SFmAlarmDataT *alarm) {
	init_tables();
	m_alarm_data.clear();
	size_t ix = FM_ALM_IX_UUID;
	size_t mx = FM_ALM_IX_MAX;
	std::string field;
	for ( ; ix < mx ; ++ix ) {
		std::string field_n = tostr(ix,db_alarm_field_ix_to_str);
		fm_alarm_get_field((EFmAlarmIndexMap)ix,alarm,field);
		m_alarm_data[field_n] = field;
	}
	return true;
}

bool CFmDbAlarm::export_data(CFmDbAlarm::data_type &m) {
	m = m_alarm_data;
	return true;
}

bool CFmDbAlarm::export_data(SFmAlarmDataT *alarm) {
	init_tables();
	memset(alarm,0,sizeof(*alarm));

	size_t ix = FM_ALM_IX_UUID;
	size_t mx =FM_ALM_IX_MAX;
	std::string field;
	for ( ; ix < mx ; ++ix ) {
		std::string field_n = tostr(ix,db_alarm_field_ix_to_str);
		if (m_alarm_data.find(field_n)==m_alarm_data.end()) return false;
		fm_alarm_set_field((EFmAlarmIndexMap)ix,alarm,m_alarm_data[field_n]);
	}
	return true;
}

std::string CFmDbAlarm::find_field(const char *field) {
	if (field==NULL) return "";
	if (m_alarm_data.find(field)==m_alarm_data.end()) return "";
	return m_alarm_data[field];
}

std::string CFmDbAlarm::to_formatted_db_string(const char ** list, size_t len) {
	std::string details;

	if (list == NULL) {
		list = &field_map[0];
		len = sizeof(field_map)/sizeof(*field_map);
	}

	size_t ix = 0;
	for ( ; ix < len ; ++ix ) {
		FM_DB_UT_NAME_VAL(details,
				list[ix],
				m_alarm_data[list[ix]]);
		if (ix < (len-1))
			details += "', ";
	}
	return details;
}

void CFmDbAlarm::print() {
	std::string str = to_formatted_db_string();
	FM_INFO_LOG("%s\n",str.c_str());
}

bool CFmDbAlarmOperation::create_alarm(CFmDBSession &sess,CFmDbAlarm &a) {

	CFmDbAlarm::data_type data;
	if (!a.export_data(data)) return false;

	CFmDbAlarm::data_type::iterator it =
			data.find(FM_ALARM_COLUMN_DELETED_AT);
	if (it != data.end()){
		data.erase(it);
	}
	it = data.find(FM_ALARM_COLUMN_ID);
	if (it != data.end()){
		data.erase(it);
	}

	std::string query;
	FM_DB_UT_NAME_VAL(query,FM_ALARM_COLUMN_ALARM_ID,
			data[FM_ALARM_COLUMN_ALARM_ID]);

	query += " AND ";
	FM_DB_UT_NAME_VAL(query,FM_ALARM_COLUMN_ENTITY_INSTANCE_ID,
			data[FM_ALARM_COLUMN_ENTITY_INSTANCE_ID]);

	std::string sql;
	fm_db_util_build_sql_query((const char*)FM_ALARM_TABLE_NAME, query.c_str(), sql);

	fm_db_result_t result;

	if ((sess.query(sql.c_str(), result)) != true){
		return false;
	}

	data[FM_ALARM_COLUMN_UUID] = a.find_field(FM_ALARM_COLUMN_UUID);
	data[FM_ALARM_COLUMN_MASKED] = "False";

	fm_db_util_sql_params sql_params;

	if (result.size() == 0){
		fm_db_util_build_sql_insert((const char*)FM_ALARM_TABLE_NAME, data, sql_params);
	}else{
		fm_db_single_result_t alm = result[0];
		fm_db_util_build_sql_update((const char*)FM_ALARM_TABLE_NAME,
				alm[FM_ALARM_COLUMN_ID],data, sql_params);
	}
	sql_params.n_params = data.size();
	FM_DEBUG_LOG("execute CMD (%s)\n", sql_params.db_cmd.c_str());
	return sess.params_cmd(sql_params);
}

bool CFmDbAlarmOperation::delete_alarms(CFmDBSession &sess, const char *id) {
	std::string sql;

	fm_db_util_build_sql_delete_all((const char*)FM_ALARM_TABLE_NAME, id, sql);
	FM_DEBUG_LOG("CMD:(%s)\n", sql.c_str());
	return sess.cmd(sql.c_str());
}

bool CFmDbAlarmOperation::delete_alarm(CFmDBSession &sess, AlarmFilter &af) {
	std::string sql;

	fm_db_util_build_sql_delete((const char*)FM_ALARM_TABLE_NAME, &af, sql);
	FM_DEBUG_LOG("CMD:(%s)\n", sql.c_str());
	return sess.cmd(sql.c_str());
}


bool CFmDbAlarmOperation::get_alarm(CFmDBSession &sess, AlarmFilter &af, fm_db_result_t & alarms) {
	std::string sql;
	char query[FM_MAX_SQL_STATEMENT_MAX];

	if (strlen(af.entity_instance_id) == 0){
		snprintf(query, sizeof(query),"%s = '%s' AND %s = ' '", FM_ALARM_COLUMN_ALARM_ID, af.alarm_id,
						FM_ALARM_COLUMN_ENTITY_INSTANCE_ID);
	}
	else{
		snprintf(query, sizeof(query),"%s = '%s' AND %s = '%s'", FM_ALARM_COLUMN_ALARM_ID, af.alarm_id,
				FM_ALARM_COLUMN_ENTITY_INSTANCE_ID, af.entity_instance_id);
	}
	fm_db_util_build_sql_query((const char*)FM_ALARM_TABLE_NAME, query, sql);
	FM_DEBUG_LOG("get_alarm:(%s)\n", sql.c_str());
	if ((sess.query(sql.c_str(), alarms)) != true){
		return false;
	}
	return true;
}

bool CFmDbAlarmOperation::get_alarms(CFmDBSession &sess,const char *id, fm_db_result_t & alarms) {
	std::string sql;

	char query[FM_MAX_SQL_STATEMENT_MAX];
	fm_db_result_t res;
	res.clear();

	sql = FM_DB_SELECT_FROM_TABLE(FM_ALARM_TABLE_NAME);
	sql += " ";

	sql += " INNER JOIN ";
	sql += FM_EVENT_SUPPRESSION_TABLE_NAME;
	sql += " ON ";
	sql += FM_ALARM_TABLE_NAME;
	sql += ".";
	sql += FM_ALARM_COLUMN_ALARM_ID;
	sql += " = ";
	sql += FM_EVENT_SUPPRESSION_TABLE_NAME;
	sql += ".";
	sql += FM_EVENT_SUPPRESSION_COLUMN_ALARM_ID;

	sql += " WHERE ";
	sql += FM_EVENT_SUPPRESSION_TABLE_NAME;
	sql += ".";
	sql += FM_EVENT_SUPPRESSION_COLUMN_SUPPRESSION_STATUS;
	sql += " = '";
	sql += FM_EVENT_SUPPRESSION_UNSUPPRESSED;
	sql += "'";

	if (id != NULL){
		snprintf(query, sizeof(query),"%s like '%s%s'", FM_ALARM_COLUMN_ENTITY_INSTANCE_ID, id,"%");
		if (NULL!=query) {
			sql += " AND ";
			sql += query;
		}
	}

	FM_DEBUG_LOG("CMD:(%s)\n", sql.c_str());
	if ((sess.query(sql.c_str(), alarms)) != true)
		return false;

	return true;
}

bool CFmDbAlarmOperation::get_alarms_by_id(CFmDBSession &sess,const char *id, fm_db_result_t & alarms) {

	fm_alarm_id alm_id = {0};
	char query[FM_MAX_SQL_STATEMENT_MAX];
	std::string sql;
        STRCP_TO(alm_id, id, RETRUN_FALSE);
	if(snprintf(query, sizeof(query),"%s = '%s'", FM_ALARM_COLUMN_ALARM_ID, id) < 0
            return false;

	fm_db_util_build_sql_query((const char*)FM_ALARM_TABLE_NAME, query, sql);
	FM_DEBUG_LOG("CMD:(%s)\n", sql.c_str());
	if ((sess.query(sql.c_str(), alarms)) != true){
		return false;
	}

	return true;
}

bool CFmDbAlarmOperation::get_all_alarms(CFmDBSession &sess, SFmAlarmDataT **alarms, size_t *len ) {
    fm_db_result_t res;

    *len = 0;
    *alarms = NULL;
    if (!get_alarms(sess, NULL, res))
        return false;

    std::string sname = fm_db_util_get_system_name();

    unsigned int found_num_alarms = res.size();

    if (found_num_alarms < 1)
        return false;

    SFmAlarmDataT *p =
            (SFmAlarmDataT*)malloc(found_num_alarms*sizeof(SFmAlarmDataT));

    if (p==NULL)
        return false;

    size_t ix = 0;
    for ( ; ix < found_num_alarms; ++ix ){
        CFmDbAlarm dbAlm;
        CFmDbAlarm::convert_to(res[ix],p+ix);
        std::string eid = (p+ix)->entity_instance_id;
        eid = sname + "." + eid;
        STRCP_TO((p+ix)->entity_instance_id, eid, CONTINUE);
    }
    (*alarms) = p;
    *len = found_num_alarms;

    return true;
}

bool CFmDbAlarmOperation::get_history(CFmDBSession &sess,fm_db_result_t & alarms) {
	std::string sql;
	std::string separator = ", ";
	std::string alias = " as ";

	sql = "SELECT * FROM ";
	sql += FM_EVENT_LOG_TABLE_NAME;

	sql += " INNER JOIN ";
	sql += FM_EVENT_SUPPRESSION_TABLE_NAME;
	sql += " ON ";
	sql += FM_EVENT_LOG_TABLE_NAME;
	sql += ".";
	sql += FM_EVENT_LOG_COLUMN_EVENT_ID;
	sql += " = ";
	sql += FM_EVENT_SUPPRESSION_TABLE_NAME;
	sql += ".";
	sql += FM_EVENT_SUPPRESSION_COLUMN_ALARM_ID;

	sql += " WHERE ";
	sql += FM_EVENT_SUPPRESSION_TABLE_NAME;
	sql += ".";
	sql += FM_EVENT_SUPPRESSION_COLUMN_SUPPRESSION_STATUS;
	sql += " = '";
	sql += FM_EVENT_SUPPRESSION_UNSUPPRESSED;

	sql += "' AND (";
	sql += FM_EVENT_LOG_COLUMN_STATE;
	sql += " = 'set' OR ";
	sql += FM_EVENT_LOG_COLUMN_STATE;
	sql += " = 'clear' ) ";

	sql += std::string(FM_DB_ORDERBY_TIME());

	FM_DEBUG_LOG("CMD:(%s)\n", sql.c_str());
	if ((sess.query(sql.c_str(), alarms)) != true){
		return false;
	}
	return true;
}

bool CFmDbAlarmOperation::mask_unmask_alarms(CFmDBSession &sess,
		SFmAlarmDataT &a, bool mask){

	std::string sql;

	char query[FM_MAX_SQL_STATEMENT_MAX];
	fm_db_result_t res;
	res.clear();

	snprintf(query, sizeof(query),"%s like '%s%s' and suppression=True",
			FM_ALARM_COLUMN_ENTITY_INSTANCE_ID, a.entity_instance_id,"%");


	fm_db_util_build_sql_query((const char*)FM_ALARM_TABLE_NAME, query, sql);

	FM_DEBUG_LOG("CMD:(%s)\n", sql.c_str());
	if ((sess.query(sql.c_str(), res)) != true){
		return false;
	}
	fm_db_result_t::iterator it = res.begin();
	fm_db_result_t::iterator end = res.end();
	for (; it != end; ++it){
		if (((*it)[FM_ALARM_COLUMN_ALARM_ID]==std::string(a.alarm_id)) &&
			((*it)[FM_ALARM_COLUMN_ENTITY_INSTANCE_ID]==std::string(a.entity_instance_id))){
			FM_INFO_LOG("Skip the alarm that inhibits others (%s), (%s)\n",
					a.alarm_id, a.entity_instance_id);
			continue;
		}
		std::map<std::string,std::string> data;
		data.clear();
		fm_db_util_sql_params sql_params;

		fm_db_util_build_sql_update((const char*)FM_ALARM_TABLE_NAME,
						(*it)[FM_ALARM_COLUMN_ID],data, sql_params, mask);
		sql_params.n_params = data.size();
		FM_DEBUG_LOG("execute CMD (%s)\n", sql_params.db_cmd.c_str());
		sess.params_cmd(sql_params);
	}
	return true;
}

bool CFmDbAlarmOperation::get_all_history_alarms(CFmDBSession &sess, SFmAlarmDataT **alarms, size_t *len ) {
    fm_db_result_t res;

    *len = 0;
    *alarms = NULL;
    if (!get_history(sess,res)) return false;

    std::string sname = fm_db_util_get_system_name();

    unsigned int found_num_alarms = res.size();

    if (found_num_alarms < 1) return false;

    SFmAlarmDataT *p =
            (SFmAlarmDataT*)malloc(found_num_alarms*sizeof(SFmAlarmDataT));

    if (p==NULL){
        return false;
    }
    size_t ix = 0;
    for ( ; ix < found_num_alarms; ++ix ){
        CFmDbAlarm dbAlm;
        CFmDbAlarm::convert_to(res[ix],p+ix);
        std::string eid = (p+ix)->entity_instance_id;
        eid = sname + "." + eid;
        STRCP_TO((p+ix)->entity_instance_id, eid, CONTINUE);
    }
    (*alarms) = p;
    *len = found_num_alarms;
    return true;
}

bool CFmDbAlarmOperation::add_alarm_history(CFmDBSession &sess,
		SFmAlarmDataT &a, bool set){

	if (set){
		a.alarm_state = (a.alarm_state == FM_ALARM_STATE_CLEAR) ?
				FM_ALARM_STATE_SET : a.alarm_state;
	}else{
		a.alarm_state = FM_ALARM_STATE_CLEAR;
	}

	CFmDbAlarm alm;
	if (!alm.create_data(&a)) return false;

	CFmDbAlarm::data_type data;
	if (!alm.export_data(data)) return false;

	CFmDbAlarm::data_type::iterator it =
			data.find(FM_ALARM_COLUMN_DELETED_AT);
	if (it != data.end()){
		data.erase(it);
	}

	it = data.find(FM_ALARM_COLUMN_UPDATED_AT);
	if (it != data.end()){
		data.erase(it);
	}

	it = data.find(FM_ALARM_COLUMN_MASKED);
	if (it != data.end()){
		data.erase(it);
	}

	it = data.find(FM_ALARM_COLUMN_INHIBIT_ALARMS);
	if (it != data.end()){
		data.erase(it);
	}

	it = data.find(FM_ALARM_COLUMN_ID);
	if (it != data.end()){
		data.erase(it);
	}

	int id =0;
	std::string sql;

	if (false == fm_db_util_get_next_log_id(sess, id)) {
		return false;
	}
	if (0 != id) {
		data[FM_ALARM_COLUMN_ID] = fm_db_util_int_to_string(id);
	}

	// get the current time for clear event
	if (!set){
		std::string time_str;
		fm_db_util_make_timestamp_string(time_str);
		data[FM_ALARM_COLUMN_TIMESTAMP] = time_str;
		//set the same time in the alarm data that will be used for logging
		fm_db_util_get_timestamp(time_str.c_str(), a.timestamp);
	}
	sql.clear();

	fm_db_util_sql_params sql_params;
	fm_db_util_event_log_build_sql_insert(data, sql_params);
	FM_DEBUG_LOG("Add row (%s)\n", sql_params.db_cmd.c_str());
	return sess.params_cmd(sql_params);
}


//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include <stdlib.h>
#include <string>
#include <map>

#include "fmLog.h"
#include "fmDbAlarm.h"
#include "fmDbEventLog.h"
#include "fmAlarmUtils.h"
#include "fmConstants.h"
#include "fmDbUtils.h"

typedef std::map<int,std::string> itos_t;
typedef std::map<std::string,int> stoi_t;

static pthread_mutex_t mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

static itos_t db_event_log_field_ix_to_str;
static stoi_t db_event_log_field_str_to_ix;

static const char * field_map[] = {
	FM_EVENT_LOG_COLUMN_CREATED_AT, //0
	FM_EVENT_LOG_COLUMN_UPDATED_AT,
	FM_EVENT_LOG_COLUMN_DELETED_AT,
	FM_EVENT_LOG_COLUMN_ID,
	FM_EVENT_LOG_COLUMN_UUID,
	FM_EVENT_LOG_COLUMN_EVENT_ID,
	FM_EVENT_LOG_COLUMN_STATE,
	FM_EVENT_LOG_COLUMN_ENTITY_TYPE_ID,
	FM_EVENT_LOG_COLUMN_ENTITY_INSTANCE_ID,
	FM_EVENT_LOG_COLUMN_TIMESTAMP,
	FM_EVENT_LOG_COLUMN_SEVERITY,
	FM_EVENT_LOG_COLUMN_REASON_TEXT,
	FM_EVENT_LOG_COLUMN_EVENT_TYPE,
	FM_EVENT_LOG_COLUMN_PROBABLE_CAUSE,
	FM_EVENT_LOG_COLUMN_PROPOSED_REPAIR_ACTION,
	FM_EVENT_LOG_COLUMN_SERVICE_AFFECTING,
	FM_EVENT_LOG_COLUMN_SUPPRESSION,
};

static const int alarm_to_event_log_index[] = {
	FM_EVENT_LOG_IX_UUID,
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
	-1,
	FM_EVENT_LOG_IX_MAX
};

void CFmDbEventLog::add_both_tables(const char *str, int id, itos_t &t1,stoi_t &t2 ) {
	t1[id]=str;
	t2[str]=id;
}

static std::string tostr(int id, const itos_t &t ){
	itos_t::const_iterator it = t.find(id);
	if (it!=t.end()) return it->second;
	return "unknown";
}

void CFmDbEventLog::init_tables() {
	pthread_mutex_lock(&mutex);
	static bool has_inited=false;

	while (!has_inited) {
		add_both_tables(FM_EVENT_LOG_COLUMN_UUID,FM_EVENT_LOG_IX_UUID,db_event_log_field_ix_to_str,db_event_log_field_str_to_ix);
		add_both_tables(FM_EVENT_LOG_COLUMN_EVENT_ID,FM_EVENT_LOG_IX_EVENT_ID,db_event_log_field_ix_to_str,db_event_log_field_str_to_ix);
		add_both_tables(FM_EVENT_LOG_COLUMN_STATE,FM_EVENT_LOG_IX_STATE,db_event_log_field_ix_to_str,db_event_log_field_str_to_ix);
		add_both_tables(FM_EVENT_LOG_COLUMN_ENTITY_TYPE_ID,FM_EVENT_LOG_IX_ENTITY_ID,db_event_log_field_ix_to_str,db_event_log_field_str_to_ix);
		add_both_tables(FM_EVENT_LOG_COLUMN_ENTITY_INSTANCE_ID,FM_EVENT_LOG_IX_INSTANCE_ID,db_event_log_field_ix_to_str,db_event_log_field_str_to_ix);
		add_both_tables(FM_EVENT_LOG_COLUMN_TIMESTAMP,FM_EVENT_LOG_IX_TIMESTAMP,db_event_log_field_ix_to_str,db_event_log_field_str_to_ix);
		add_both_tables(FM_EVENT_LOG_COLUMN_SEVERITY,FM_EVENT_LOG_IX_SEVERITY,db_event_log_field_ix_to_str,db_event_log_field_str_to_ix);
		add_both_tables(FM_EVENT_LOG_COLUMN_REASON_TEXT,FM_EVENT_LOG_IX_REASON,db_event_log_field_ix_to_str,db_event_log_field_str_to_ix);
		add_both_tables(FM_EVENT_LOG_COLUMN_EVENT_TYPE,FM_EVENT_LOG_IX_EVENT_TYPE,db_event_log_field_ix_to_str,db_event_log_field_str_to_ix);
		add_both_tables(FM_EVENT_LOG_COLUMN_PROBABLE_CAUSE,FM_EVENT_LOG_IX_PROBABLE_CAUSE,db_event_log_field_ix_to_str,db_event_log_field_str_to_ix);
		add_both_tables(FM_EVENT_LOG_COLUMN_PROPOSED_REPAIR_ACTION, FM_EVENT_LOG_IX_REPAIR_ACTION, db_event_log_field_ix_to_str,db_event_log_field_str_to_ix);
		add_both_tables(FM_EVENT_LOG_COLUMN_SERVICE_AFFECTING,FM_EVENT_LOG_IX_SERVICE_AFFECT,db_event_log_field_ix_to_str,db_event_log_field_str_to_ix);
		add_both_tables(FM_EVENT_LOG_COLUMN_SUPPRESSION,FM_EVENT_LOG_IX_SUPPRESSION,db_event_log_field_ix_to_str,db_event_log_field_str_to_ix);
		has_inited = true;
	}
	pthread_mutex_unlock(&mutex);
}

void CFmDbEventLog::append(std::string &str, const std::string &what) {
	str+=what;
	str+="###";
}

bool CFmDbEventLog::import_data(data_type &m ) {
	m_event_log_data = m;
	return true;
}

bool CFmDbEventLog::create_data(SFmAlarmDataT *alarm) {
	init_tables();
	m_event_log_data.clear();
	size_t ix = FM_ALM_IX_UUID;
	size_t mx = FM_ALM_IX_MAX;
	std::string field;
	int log_idx;
	for ( ; ix < mx ; ++ix ) {
		log_idx = alarm_to_event_log_index[ix];
		if (log_idx == -1)
			continue;
		std::string field_n = tostr(log_idx,db_event_log_field_ix_to_str);
		if (field_n == "unknown")
			continue; // ignore these fields
		fm_alarm_get_field((EFmAlarmIndexMap)ix,alarm,field);
		m_event_log_data[field_n] = field;
	}
	return true;
}

bool CFmDbEventLog::export_data(CFmDbEventLog::data_type &m) {
	m = m_event_log_data;
	return true;
}

bool CFmDbEventLog::export_data(SFmAlarmDataT *data) {
	init_tables();
	memset(data,0,sizeof(*data));

	size_t ix = FM_EVENT_LOG_IX_UUID;
	size_t mx = FM_EVENT_LOG_IX_MAX;
	std::string field;
	for ( ; ix < mx ; ++ix ) {
		std::string field_n = tostr(ix,db_event_log_field_ix_to_str);
		if (m_event_log_data.find(field_n)==m_event_log_data.end()) return false;
		fm_event_log_set_field((EFmEventLogIndexMap)ix,data,m_event_log_data[field_n]);
	}
	return true;
}

std::string CFmDbEventLog::find_field(const char *field) {
	if (field==NULL) return "";
	if (m_event_log_data.find(field)==m_event_log_data.end()) return "";
	return m_event_log_data[field];
}

std::string CFmDbEventLog::to_formatted_db_string(const char ** list, size_t len) {
	std::string details;

	if (list == NULL) {
		list = &field_map[0];
		len = sizeof(field_map)/sizeof(*field_map);
	}

	size_t ix = 0;
	for ( ; ix < len ; ++ix ) {
		FM_DB_UT_NAME_VAL(details,
				list[ix],
				m_event_log_data[list[ix]]);
		if (ix < (len-1))
			details += "', ";
	}
	return details;
}

void CFmDbEventLog::print() {
	std::string str = to_formatted_db_string();
	FM_INFO_LOG("%s\n",str.c_str());
}

bool CFmDbEventLogOperation::create_event_log(CFmDBSession &sess,CFmDbEventLog &a) {

	CFmDbEventLog::data_type data;
	if (!a.export_data(data)) return false;

	CFmDbEventLog::data_type::iterator it =
			data.find(FM_EVENT_LOG_COLUMN_DELETED_AT);
	if (it != data.end()){
		data.erase(it);
	}

    it = data.find(FM_EVENT_LOG_COLUMN_UPDATED_AT);
    if (it != data.end()){
            data.erase(it);
    }

	it = data.find(FM_EVENT_LOG_COLUMN_ID);
	if (it != data.end()){
		data.erase(it);
	}

    int id =0;
    std::string sql;

    if ( false == fm_db_util_get_next_log_id(sess, id)) {
    	return false;
    }
    if (0 != id) {
    	data[FM_EVENT_LOG_COLUMN_ID] = fm_db_util_int_to_string(id);
    }

    fm_db_util_sql_params sql_params;
    fm_db_util_build_sql_insert((const char*)FM_EVENT_LOG_TABLE_NAME, data, sql_params);
    sql_params.n_params = data.size();
    FM_DEBUG_LOG("Add row (%s)\n", sql_params.db_cmd.c_str());
    return sess.params_cmd(sql_params);
}

bool CFmDbEventLogOperation::get_event_log(CFmDBSession &sess, AlarmFilter &af, fm_db_result_t & logs) {
	std::string sql;
	char query[FM_MAX_SQL_STATEMENT_MAX];

	if (strlen(af.entity_instance_id) == 0){
		snprintf(query, sizeof(query),"%s = '%s' AND %s = ' '", FM_EVENT_LOG_COLUMN_EVENT_ID, af.alarm_id,
						FM_EVENT_LOG_COLUMN_ENTITY_INSTANCE_ID);
	}
	else{
		snprintf(query, sizeof(query),"%s = '%s' AND %s = '%s'", FM_EVENT_LOG_COLUMN_EVENT_ID, af.alarm_id,
				FM_EVENT_LOG_COLUMN_ENTITY_INSTANCE_ID, af.entity_instance_id);
	}
	fm_db_util_build_sql_query((const char*)FM_EVENT_LOG_TABLE_NAME, query, sql);
	FM_DEBUG_LOG("get_log:(%s)\n", sql.c_str());
	if ((sess.query(sql.c_str(), logs)) != true){
		return false;
	}
	return true;
}

bool CFmDbEventLogOperation::get_event_logs(CFmDBSession &sess, fm_db_result_t & logs) {
	std::string sql;

	sql = FM_DB_SELECT_FROM_TABLE(FM_EVENT_LOG_TABLE_NAME);

	sql += " LEFT OUTER JOIN ";
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

	sql += "' OR ";
	sql += FM_EVENT_LOG_COLUMN_STATE;
	sql += " = 'log' ";

	// select statement built

	FM_DEBUG_LOG("CMD:(%s)\n", sql.c_str());
	if ((sess.query(sql.c_str(), logs)) != true){
		return false;
	}
	return true;
}

bool CFmDbEventLogOperation::get_event_logs_by_id(CFmDBSession &sess,const char *id, fm_db_result_t & logs) {
	char query[FM_MAX_SQL_STATEMENT_MAX];
	std::string sql;

	snprintf(query, sizeof(query),"%s = '%s'", FM_EVENT_LOG_COLUMN_EVENT_ID, id);

	fm_db_util_build_sql_query((const char*)FM_EVENT_LOG_TABLE_NAME, query, sql);
	FM_DEBUG_LOG("CMD:(%s)\n", sql.c_str());
	if ((sess.query(sql.c_str(), logs)) != true){
		return false;
	}
	return true;
}

bool CFmDbEventLogOperation::get_all_event_logs(CFmDBSession &sess, SFmAlarmDataT **logs, size_t *len ) {
	fm_db_result_t res;

	*len = 0;
	*logs = NULL;

	if (!get_event_logs(sess, res)) return false;

	std::string sname = fm_db_util_get_system_name();

	unsigned int found_num_logs = res.size();

	if (found_num_logs < 1) return false;

	SFmAlarmDataT *p =
			(SFmAlarmDataT*)malloc(found_num_logs*sizeof(SFmAlarmDataT));

	if (p==NULL){
		return false;
	}
	size_t ix = 0;
	for ( ; ix < found_num_logs; ++ix ){
		CFmDbEventLog dbEventLog;
		CFmDbEventLog::convert_to(res[ix],p+ix);
		std::string eid = (p+ix)->entity_instance_id;
		eid = sname + "." + eid;
        if(snprintf((p+ix)->entity_instance_id, sizeof((p+ix)->entity_instance_id), "%s", eid.c_str()) < 0)
            return false;
	}
	(*logs) = p;
	*len = found_num_logs;
	return true;
}

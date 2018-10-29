//
// Copyright (c) 2014-2018 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sstream>
#include <map>
#include <iostream>
#include <assert.h>
#include <arpa/inet.h>
#include <python2.7/Python.h>


#include "fmMutex.h"
#include "fmAPI.h"
#include "fmLog.h"
#include "fmFile.h"
#include "fmDb.h"
#include "fmDbUtils.h"
#include "fmDbAPI.h"
#include "fmConstants.h"
#include "fmAlarmUtils.h"
#include "fmConfig.h"



static pthread_mutex_t mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;


void FM_DB_UT_NAME_VAL(
	std::string &result,
	const std::string &lhs, const std::string &rhs) {
	result+= lhs;
	result+= " = '";
	result+=rhs;
	result+="'";
}

void FM_DB_UT_NAME_PARAM(
	std::string &result,
	const std::string &lhs, const std::string &rhs) {
	result+= lhs;
	result+= "=";
	result+=rhs;
}

static inline void append_str(std::string &s, const std::string &app) {
	if (s.size()!=0) s+= ", ";
	s+=app;
}

static inline std::string add_quote(const std::string &s) {
	return std::string("'")+s+"'";
}

static int get_oldest_id(CFmDBSession &sess, const char* db_table){
	fm_db_result_t res;
	int id = 0;
	char sql[FM_MAX_SQL_STATEMENT_MAX];

	memset(sql, 0, sizeof(sql));
	snprintf(sql, sizeof(sql), "SELECT %s FROM %s order by %s limit 1",
			FM_ALARM_COLUMN_ID, db_table,
			FM_ALARM_COLUMN_CREATED_AT);

	if (sess.query(sql, res)){
		if (res.size() > 0){
			std::map<std::string,std::string>  entry = res[0];
			id = fm_db_util_string_to_int(entry[FM_ALARM_COLUMN_ID]);
		}
	}
	return id;
}


static inline CFmDBSession & FmDbSessionFromHandle(TFmAlarmSessionT *p){
	return *((CFmDBSession*)p);
}

static void format_time_string(std::string &tstr, struct timespec *ts,
		bool snmp){

	char buff[200];

	struct tm t;
	int ret = 0;

	//tzset();
	memset(&t, 0, sizeof(t));
	memset(buff, 0, sizeof(buff));
	if (gmtime_r(&(ts->tv_sec), &t) == NULL)
		return ;

	if (snmp){
		ret = strftime(buff, sizeof(buff), "%Y-%m-%d,%H:%M:%S.0,+0:0", &t);
	}else{
		ret = strftime(buff,sizeof(buff), "%F %T", &t);
	}

	if (ret == 0) return ;

	tstr+=buff;

	if (!snmp){
		//append the microseconds
		snprintf(buff,sizeof(buff), ".%06ld", ts->tv_nsec/1000);
		tstr+=buff;
	}
}

std::string fm_db_util_replace_single_quote(std::string const& original){
	std::string result;
	std::string::const_iterator it = original.begin();
	for (; it != original.end(); ++it){
		if (*it == '\''){
			result.push_back('\'');
		}
		result.push_back(*it);
	}
	return result;
}

std::string fm_db_util_int_to_string(int val){
	std::string result;
	std::stringstream ss;
	ss << val;
	ss >> result;
	return result;
}

int fm_db_util_string_to_int(std::string val){
	int result;
	std::stringstream ss;
	ss << val;
	ss >> result;
	return result;
}

void fm_db_util_make_timestamp_string(std::string &tstr, FMTimeT tm,
	bool snmp){
	struct timespec ts;
	if (tm != 0){
		ts.tv_sec = tm / 1000000;
		ts.tv_nsec = (tm % 1000000) * 1000;
	}else{
		clock_gettime(CLOCK_REALTIME, &ts);
	}
	tstr.clear();
	format_time_string(tstr,&ts, snmp);
}

bool fm_db_util_get_timestamp(const char *str, FMTimeT &ft){
	struct timespec ts;
	memset(&ts, 0, sizeof(ts));
	// only check if the year is present
	if (strlen(str) < 10) {
		//get the current time
		clock_gettime(CLOCK_REALTIME, &ts);
	}else {
		struct tm t;
		memset(&t, 0, sizeof(t));
		strptime(str, "%F %T", &t);
		ts.tv_sec = mktime(&t);
		//now get the nanoseconds
		char *tstr, *tobe_free = strdup(str);
		tstr = tobe_free;

		strsep(&tstr, ".");
		if (tstr != NULL) {
			ts.tv_nsec = atol(tstr)*1000;
		}
		free(tobe_free);
	}
	ft = ts.tv_sec*1000000 + ts.tv_nsec/1000;
	return true;
}

bool fm_db_util_build_sql_query(const char* db_table,
		const char* db_query, std::string &db_cmd){
	db_cmd = FM_DB_SELECT_FROM_TABLE(db_table);

	if (NULL!=db_query) {
		db_cmd += " ";
		db_cmd += FM_DB_WHERE();
		db_cmd += " ";
		db_cmd += db_query;
	}
	return true;
}

bool fm_db_util_build_sql_insert(const char* db_table,
		std::map<std::string,std::string> &map,
		fm_db_util_sql_params &params) {

	std::string keys;
	std::string values;
	std::string time_str;

	fm_db_util_make_timestamp_string(time_str);
	map[FM_ALARM_COLUMN_CREATED_AT] = time_str;

	std::map<std::string,std::string>::iterator at =
			map.find(FM_ALARM_COLUMN_UPDATED_AT);
	if (at != map.end()){
		map.erase(at);
	}

	std::map<std::string,std::string>::iterator it = map.begin();
	std::map<std::string,std::string>::iterator end = map.end();

	size_t i = 0;
	std::string cmd_params;
	char str[80];
	std::string param;
	for ( ; it != end ; ++it ) {
		param.clear();
		memset(str, 0, sizeof(str));
		append_str(keys,it->first);
		if (it->first == FM_ALARM_COLUMN_ID){
			params.id = htonl(atoi(it->second.c_str()));
			params.param_values.push_back((char*)&params.id);
			params.param_lengths.push_back(sizeof(int));
			params.param_format.push_back(1);
			sprintf(str,"$%lu::int,", ++i);
		} else if ((it->first == FM_ALARM_COLUMN_CREATED_AT) ||
		           (it->first == FM_ALARM_COLUMN_TIMESTAMP)){
			params.param_values.push_back(it->second.c_str());
			params.param_lengths.push_back(strlen(it->second.c_str()));
			params.param_format.push_back(0);
			sprintf(str, "$%lu::timestamp,", ++i);
		} else if ((it->first == FM_ALARM_COLUMN_SERVICE_AFFECTING) ||
		           (it->first == FM_ALARM_COLUMN_SUPPRESSION) ||
		           (it->first == FM_ALARM_COLUMN_INHIBIT_ALARMS) ||
		           (it->first == FM_ALARM_COLUMN_MASKED)){
			params.scratch[it->first] = (it->second == "True") ? 1 : 0;
			params.param_values.push_back((char *)&(params.scratch[it->first]));
			params.param_lengths.push_back(sizeof(bool));
			params.param_format.push_back(1);
			sprintf(str,"$%lu::boolean,", ++i);
		} else {
			params.param_values.push_back(it->second.c_str());
			params.param_lengths.push_back(strlen(it->second.c_str()));
			params.param_format.push_back(0);
			sprintf(str,"$%lu::varchar,", ++i);
		}

		param.assign(str);
		cmd_params += param;
	}
	cmd_params.resize(cmd_params.size()-1);
	params.db_cmd = "INSERT INTO ";
	params.db_cmd += db_table;
	params.db_cmd += " ( ";
	params.db_cmd += keys;
	params.db_cmd += ") VALUES (";
	params.db_cmd += cmd_params;
	params.db_cmd += ");";

	return true;
}


bool fm_db_util_event_log_build_sql_insert(std::map<std::string,std::string> &map,
		fm_db_util_sql_params &params) {

	std::string keys;
	std::string values;
	std::string time_str;

	fm_db_util_make_timestamp_string(time_str);
	map[FM_ALARM_COLUMN_CREATED_AT] = time_str;

	std::map<std::string,std::string>::iterator at =
			map.find(FM_ALARM_COLUMN_UPDATED_AT);
	if (at != map.end()){
		map.erase(at);
	}

	std::map<std::string,std::string>::iterator it = map.begin();
	std::map<std::string,std::string>::iterator end = map.end();

	size_t i = 0;
	std::string cmd_params;
	char str[80];
	std::string param;
	params.n_params = 0;

	for ( ; it != end ; ++it ) {
		param.clear();
		memset(str, 0, sizeof(str));
		if (it->first == FM_ALARM_COLUMN_ID){
			append_str(keys,it->first);
			params.id = htonl(atoi(it->second.c_str()));
			params.param_values.push_back((char*)&params.id);
			params.param_lengths.push_back(sizeof(int));
			params.param_format.push_back(1);
			sprintf(str,"$%lu::int,", ++i);
		} else if ((it->first == FM_ALARM_COLUMN_CREATED_AT) ||
		           (it->first == FM_ALARM_COLUMN_TIMESTAMP)){
			append_str(keys,it->first);
			params.param_values.push_back(it->second.c_str());
			params.param_lengths.push_back(strlen(it->second.c_str()));
			params.param_format.push_back(0);
			sprintf(str, "$%lu::timestamp,", ++i);
		} else if ((it->first == FM_ALARM_COLUMN_SERVICE_AFFECTING) ||
		           (it->first == FM_ALARM_COLUMN_SUPPRESSION) ||
		           (it->first == FM_ALARM_COLUMN_INHIBIT_ALARMS) ||
		           (it->first == FM_ALARM_COLUMN_MASKED)){
			append_str(keys,it->first);
			params.scratch[it->first] = (it->second == "True") ? 1 : 0;
			params.param_values.push_back((char *)&(params.scratch[it->first]));
			params.param_lengths.push_back(sizeof(bool));
			params.param_format.push_back(1);
			sprintf(str,"$%lu::boolean,", ++i);
		} else if ((it->first == FM_ALARM_COLUMN_ALARM_ID) ||
				   (it->first == FM_EVENT_LOG_COLUMN_ID)){
			std::string event_log_id = FM_EVENT_LOG_COLUMN_EVENT_ID;
			append_str(keys,event_log_id);
			params.param_values.push_back(it->second.c_str());
			params.param_lengths.push_back(strlen(it->second.c_str()));
			params.param_format.push_back(0);
			sprintf(str,"$%lu::varchar,", ++i);
		} else if ((it->first == FM_ALARM_COLUMN_ALARM_TYPE) ||
				   (it->first == FM_EVENT_LOG_COLUMN_EVENT_TYPE)){
			std::string event_log_type = FM_EVENT_LOG_COLUMN_EVENT_TYPE;
			append_str(keys,event_log_type);
			params.param_values.push_back(it->second.c_str());
			params.param_lengths.push_back(strlen(it->second.c_str()));
			params.param_format.push_back(0);
			sprintf(str,"$%lu::varchar,", ++i);
		}else if (it->first == FM_ALARM_COLUMN_ALARM_STATE) {
			std::string event_state = FM_EVENT_LOG_COLUMN_STATE;
			append_str(keys,event_state);
			params.param_values.push_back(it->second.c_str());
			params.param_lengths.push_back(strlen(it->second.c_str()));
			params.param_format.push_back(0);
			sprintf(str,"$%lu::varchar,", ++i);
		}
		else {
			append_str(keys,it->first);
			params.param_values.push_back(it->second.c_str());
			params.param_lengths.push_back(strlen(it->second.c_str()));
			params.param_format.push_back(0);
			sprintf(str,"$%lu::varchar,", ++i);
		}

		params.n_params++;
		param.assign(str);
		cmd_params += param;
	}

	cmd_params.resize(cmd_params.size()-1);
	params.db_cmd = "INSERT INTO ";
	params.db_cmd += FM_EVENT_LOG_TABLE_NAME;
	params.db_cmd += " ( ";
	params.db_cmd += keys;
	params.db_cmd += ") VALUES (";
	params.db_cmd += cmd_params;
	params.db_cmd += ");";

	return true;
}


bool fm_db_util_build_sql_update(const char* db_table,
		const std::string & id, std::map<std::string,std::string> &map,
		fm_db_util_sql_params &params, FMBoolTypeT masked){
	std::string time_str;
	params.db_cmd = "UPDATE ";
	params.db_cmd += db_table;
	params.db_cmd += " SET ";

	fm_db_util_make_timestamp_string(time_str);
	map[FM_ALARM_COLUMN_UPDATED_AT] = time_str;
	map[FM_ALARM_COLUMN_MASKED] = masked ? "True" : "False";

	std::map<std::string,std::string>::iterator at =
			map.find(FM_ALARM_COLUMN_CREATED_AT);
	if (at != map.end()){
		map.erase(at);
	}

	std::map<std::string,std::string>::iterator it = map.begin();
	std::map<std::string,std::string>::iterator end = map.end();

	size_t i = 0;
	char str[80];
	std::string param;
	for ( ; it != end ; ++it ) {
		param.clear();
		memset(str, 0, sizeof(str));
		if (it != map.begin())
			params.db_cmd.append(", ");

		if ((it->first == FM_ALARM_COLUMN_UPDATED_AT) ||
		    (it->first == FM_ALARM_COLUMN_TIMESTAMP)){
			params.param_values.push_back(it->second.c_str());
			params.param_lengths.push_back(strlen(it->second.c_str()));
			params.param_format.push_back(0);
			sprintf(str, "$%lu::timestamp", ++i);
		} else if ((it->first == FM_ALARM_COLUMN_SERVICE_AFFECTING) ||
		           (it->first == FM_ALARM_COLUMN_SUPPRESSION) ||
		           (it->first == FM_ALARM_COLUMN_INHIBIT_ALARMS) ||
		           (it->first == FM_ALARM_COLUMN_MASKED)){
			params.scratch[it->first] = (it->second == "True") ? 1 : 0;
			params.param_values.push_back((char *)&(params.scratch[it->first]));
			params.param_lengths.push_back(sizeof(bool));
			params.param_format.push_back(1);
			sprintf(str,"$%lu::boolean", ++i);
		} else {
			params.param_values.push_back(it->second.c_str());
			params.param_lengths.push_back(strlen(it->second.c_str()));
			params.param_format.push_back(0);
			sprintf(str,"$%lu::varchar", ++i);
		}
		param.assign(str);
		FM_DB_UT_NAME_PARAM(params.db_cmd, it->first, param);
	}

	params.db_cmd += " WHERE ";
	FM_DB_UT_NAME_VAL(params.db_cmd, FM_ALARM_COLUMN_ID, id);

	return true;
}

bool fm_db_util_build_sql_delete(const char* db_table, AlarmFilter *db_data,
		std::string &db_cmd){

	if (db_data==NULL) return false;

	char sql[FM_MAX_SQL_STATEMENT_MAX];

	if (strlen(db_data->entity_instance_id) == 0){
		snprintf(sql, sizeof(sql), "DElETE FROM %s WHERE %s = '%s' AND %s = ' '",
			db_table, FM_ALARM_COLUMN_ALARM_ID, db_data->alarm_id,
			FM_ALARM_COLUMN_ENTITY_INSTANCE_ID);
	}
	else{
		snprintf(sql, sizeof(sql), "DElETE FROM %s WHERE %s = '%s' AND %s = '%s'",
			db_table, FM_ALARM_COLUMN_ALARM_ID, db_data->alarm_id,
			FM_ALARM_COLUMN_ENTITY_INSTANCE_ID, db_data->entity_instance_id);
	}
	db_cmd.assign(sql);
	return true;
}

bool fm_db_util_build_sql_delete_row(const char* db_table, int id,
		std::string &db_cmd){

	if (id == 0) return false;

	char sql[FM_MAX_SQL_STATEMENT_MAX];

	snprintf(sql, sizeof(sql), "DElETE FROM %s WHERE %s = %d",
			db_table, FM_ALARM_COLUMN_ID, id);
	db_cmd.assign(sql);
	return true;
}

bool fm_db_util_build_sql_delete_all(const char* db_table, const char *id,
		std::string &db_cmd){

	char sql[FM_MAX_SQL_STATEMENT_MAX];

	snprintf(sql, sizeof(sql), "DElETE FROM %s WHERE %s like '%s%s'", db_table,
			FM_ALARM_COLUMN_ENTITY_INSTANCE_ID, id,"%");
	db_cmd.assign(sql);
	return true;
}


int & fm_get_alarm_history_max_size(){
	static int max_size = 0;

	if (max_size == 0){
		std::string val;
		std::string key = FM_EVENT_LOG_MAX_SIZE;
		if (fm_get_config_key(key, val)){
			max_size = fm_db_util_string_to_int(val);
		}else{
			FM_ERROR_LOG("Fail to get config value for (%s)\n", key.c_str());
		}
	}
	return max_size;
}

int & fm_get_log_max_size(){
	static int max_size = 0;

	if (max_size == 0){
		std::string val;
		std::string key = FM_EVENT_LOG_MAX_SIZE;
		if (fm_get_config_key(key, val)){
			max_size = fm_db_util_string_to_int(val);
		}else{
			FM_ERROR_LOG("Fail to get config value for (%s)\n", key.c_str());
		}
	}
	return max_size;
}

std::string fm_db_util_get_system_info(const std::string prefix, std::string key){
	std::string val;
	std::string name = "";
	if (fm_get_config_key(key, val)){
		name = prefix + val;
	}
	return name;
}

std::string fm_db_util_get_system_name(){
	return fm_db_util_get_system_info(FM_ENTITY_ROOT_KEY, FM_SYSTEM_NAME);
}

std::string fm_db_util_get_region_name(){
        return fm_db_util_get_system_info(FM_ENTITY_REGION_KEY, FM_REGION_NAME);
}

bool fm_db_util_get_row_counts(CFmDBSession &sess,
		const char* db_table, int &counts){

	fm_db_result_t res;
	std::string cmd =
			FM_DB_SELECT_ROW_COUNTS_FROM_TABLE(std::string(db_table));

	res.clear();
	if (sess.query(cmd.c_str(), res)){
		if (res.size() > 0){
			std::map<std::string,std::string>  entry = res[0];
			counts = fm_db_util_string_to_int(entry[FM_DB_TABLE_COUNT_COLUMN]);
			FM_DEBUG_LOG("Table (%s) row counts: (%llu)",
					db_table, counts);
			return true;
		}
	}
	return false;
}

bool fm_db_util_get_next_log_id(CFmDBSession &sess, int &id){

	pthread_mutex_lock(&mutex);
	static bool has_max_rows = false;
	static int max = 0;
	int active = 0;

	if (max == 0){
		max = fm_get_log_max_size();
	}

	if (!has_max_rows){
		fm_db_util_get_row_counts(sess, FM_EVENT_LOG_TABLE_NAME, active);
		FM_INFO_LOG("Active row accounts:(%d), max:(%d)\n", active, max);
		if (active >= max) {
			has_max_rows = true;
		}
	}

	if (has_max_rows){
		std::string sql;
		id = get_oldest_id(sess, FM_EVENT_LOG_TABLE_NAME);
		fm_db_util_build_sql_delete_row(FM_EVENT_LOG_TABLE_NAME, id, sql);
		FM_DEBUG_LOG("Delete row (%s)\n", sql.c_str());
		if (false == sess.cmd(sql.c_str())){
			FM_INFO_LOG("Fail to delete the old event log: (%d)", id);
			pthread_mutex_unlock(&mutex);
			return false;
		}
	}
	FM_DEBUG_LOG("Return next log id: (%d)\n", id);
	pthread_mutex_unlock(&mutex);

	return true;
}

bool fm_db_util_create_session(CFmDBSession **sess, std::string key){
	TFmAlarmSessionT handle;
	const char *db_conn = NULL;
	std::string val;

	if (fm_get_config_key(key, val) != true){
		FM_ERROR_LOG("Failed to get config for key: (%s)\n", key.c_str());
		return false;
	}

	db_conn = val.c_str();
	if (fm_snmp_util_create_session(&handle, db_conn)){
		*sess = (CFmDBSession*)handle;
		return true;
	}
	return false;
}

bool fm_db_util_sync_event_suppression(void){

	bool return_value = true;
	const char *db_conn = NULL;
	std::string val;
	std::string key = FM_SQL_CONNECTION;

	if (fm_get_config_key(key, val) != true){
		FM_ERROR_LOG("NEW Failed to get config for key: (%s)\n", key.c_str());
		return false;
	}

	db_conn = val.c_str();

	FILE* file;
	int argc;
	char * argv[2];

	FM_INFO_LOG("Starting event suppression synchronization...\n");

	argc = 2;
	argv[0] = (char*)FM_DB_SYNC_EVENT_SUPPRESSION;
	argv[1] = (char*)db_conn;

	Py_SetProgramName(argv[0]);
	Py_Initialize();
	PySys_SetArgv(argc, argv);
	file = fopen(FM_DB_SYNC_EVENT_SUPPRESSION,"r");
	PyRun_SimpleFile(file, FM_DB_SYNC_EVENT_SUPPRESSION);
	fclose(file);
	Py_Finalize();

	FM_INFO_LOG("Completed event suppression synchronization.\n");

	return return_value;
}



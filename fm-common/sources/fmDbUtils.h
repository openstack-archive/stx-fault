//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef FMDBUTILS_H_
#define FMDBUTILS_H_

#include "fmAPI.h"
#include "fmDbAlarm.h"
#include "fmDbEventLog.h"
#include "fmDb.h"

#include <string>
#include <memory>
#include <vector>

static inline const char *FM_DB_AND() { return "AND"; }
static inline const char *FM_DB_WHERE() { return "WHERE"; }
static inline const char *FM_DB_SELECT_FROM() { return "SELECT * FROM"; }
static inline const char *FM_DB_SELECT_COUNTS() {
	return "SELECT COUNT(*) FROM";
}

static inline const char *FM_DB_ORDERBY_TIME() { return "order by timestamp desc"; }

static inline std::string FM_DB_SELECT_FROM_TABLE(const std::string &table){
	return std::string(FM_DB_SELECT_FROM()) + " " +table;
}

static inline std::string FM_DB_SELECT_ROW_COUNTS_FROM_TABLE(
		const std::string &table){
	return std::string(FM_DB_SELECT_COUNTS()) + " " +table;
}

void FM_DB_UT_NAME_VAL( std::string &result,
		const std::string &lhs, const std::string &rhs);

void FM_DB_UT_NAME_PARAM( std::string &result,
		const std::string &lhs, const std::string &rhs);

std::string fm_db_util_replace_single_quote(std::string const& original);

void fm_db_util_make_timestamp_string(std::string &tstr,
		FMTimeT tm=0, bool snmp=false);

bool fm_db_util_get_timestamp(const char *str, FMTimeT &ft);

bool fm_db_util_build_sql_query(const char* db_table,
		const char* db_query, std::string &db_cmd);

bool fm_db_util_build_sql_insert(const char* db_table,
		std::map<std::string,std::string> &map,
		fm_db_util_sql_params &parms);

bool fm_db_util_event_log_build_sql_insert(std::map<std::string,std::string> &map,
		fm_db_util_sql_params &params);

bool fm_db_util_build_sql_update(const char* db_table,
		const std::string & id, std::map<std::string,std::string> &map,
		fm_db_util_sql_params &parms, FMBoolTypeT masked=0);

bool fm_db_util_build_sql_delete(const char* db_table,
		AlarmFilter *db_data, std::string &db_cmd);

bool fm_db_util_build_sql_delete_row(const char* db_table, int id,
		std::string &db_cmd);

bool fm_db_util_build_sql_delete_all(const char* db_table,
		const char *id, std::string &db_cmd);

bool fm_db_util_get_row_counts(CFmDBSession &sess, const char* db_table,
		int &counts);

bool fm_db_util_create_session(CFmDBSession **sess);

std::string fm_db_util_get_system_name(CFmDBSession &sess);

std::string fm_db_util_get_region_name(CFmDBSession &sess);

void fm_db_util_set_conf_file(const char *fn);

bool fm_db_util_get_config(std::string &key, std::string &val);

bool fm_db_util_get_next_log_id(CFmDBSession &sess, int &id);

std::string fm_db_util_int_to_string(int val);

int fm_db_util_string_to_int(std::string val);

bool fm_db_util_sync_event_suppression(void);

#endif /* FMDBUTILS_H_ */

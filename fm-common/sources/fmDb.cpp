//
// Copyright (c) 2016-2018 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "fmLog.h"
#include "fmAPI.h"
#include "fmTime.h"
#include "fmAlarmUtils.h"
#include "fmDbUtils.h"
#include "fmDb.h"
#include "fmConstants.h"
#include "fmThread.h"


CFmDBSession::CFmDBSession() {
	m_conn.uri = NULL;
	m_conn.pgconn = NULL;
	m_conn.status = DB_DISCONNECTED;
}

CFmDBSession::~CFmDBSession() {
	if (m_conn.pgconn != NULL){
		PQfinish(m_conn.pgconn);
	}
}

bool CFmDBSession::connect(const char *uri){
	const char *val = NULL;
	PGconn  *pgconn = NULL;
	size_t retries = 30, count = 0, my_sleep = 2; //2 seconds

	while (count < retries){
		/* Make a connection to the database */
		pgconn = PQconnectdb(uri);
		/* Check to see that the backend connection was successfully made */
		if ((pgconn == NULL) || (PQstatus(pgconn) != CONNECTION_OK)){
			FM_INFO_LOG("failed to connected to DB: (%s), retry (%d of %d) ",
					uri, count+1, retries);
			if (pgconn != NULL) {
				FM_INFO_LOG("Postgress error message: (%s).", PQerrorMessage(pgconn));
				PQfinish(pgconn);
			}
			sleep(my_sleep);
			count++;
		}else {
			break;
		}
	}

	if (count == retries) return false;

	m_conn.status = DB_CONNECTED;
	m_conn.pgconn = pgconn;
	m_conn.uri = uri;

	val = get_parameter_status("standard_conforming_strings");
	m_conn.equote = (val && (0 == strcmp("off", val)));
	m_conn.server_version = PQserverVersion(m_conn.pgconn);
	m_conn.protocol = PQprotocolVersion(m_conn.pgconn);
	m_conn.encoding = get_parameter_status("client_encoding");

	return true;
}

bool CFmDBSession::check_conn(){
	if (PQstatus(m_conn.pgconn) != CONNECTION_OK) {
		FM_ERROR_LOG("DB connection NOT OK\n");
		disconnect();
		return connect(m_conn.uri);
	 }
	return true;
}

void CFmDBSession::disconnect(){
	if (m_conn.pgconn != NULL){
		PQfinish(m_conn.pgconn);
	}
	if (m_conn.status == DB_CONNECTED){
		m_conn.status = DB_DISCONNECTED;
	}
}

const char * CFmDBSession::get_parameter_status(const char *param){
	return PQparameterStatus(m_conn.pgconn, param);
}

bool CFmDBSession::query(const char *db_cmd,fm_db_result_t & result) {
	PGresult *res;
	int nfields, ntuples, i, j;

	if (check_conn() == false){
		FM_ERROR_LOG("Failed to reconnect: %s", PQerrorMessage(m_conn.pgconn));
		return false;
	}

	res = PQexec(m_conn.pgconn, db_cmd);
	if (PQresultStatus(res) != PGRES_TUPLES_OK){
		FM_ERROR_LOG("Status:(%s)\n", PQresStatus(PQresultStatus(res)));
		FM_ERROR_LOG("Failed to execute (%s) (%s)", db_cmd, PQresultErrorMessage(res));
		PQclear(res);
		return false;
	}

	nfields = PQnfields(res);
	ntuples = PQntuples(res);
	FM_DEBUG_LOG("Execute cmd:(%s) OK, entries found: (%u)\n", db_cmd, ntuples);

	for (i = 0; i < ntuples; ++i){
		fm_db_single_result_t single_result;

		for (j =0; j < nfields; ++j){
			char * key = PQfname(res, j);
			char * value = PQgetvalue(res, i, j);
			single_result[key] = value;
		}
		result.push_back(single_result);
	}

	PQclear(res);
	return true;
}

// return value: -1: if there is PQ operation failure
//                0: if cmd success and check_row == false
//              row: row number if cmd success and check_row == true
int CFmDBSession::cmd(const char *db_cmd, bool check_row){
	PGresult *res;
	int rc = -1;

	if (check_conn() == false){
		FM_ERROR_LOG("Failed to reconnect: %s", PQerrorMessage(m_conn.pgconn));
		return rc;
	}

	res = PQexec(m_conn.pgconn, db_cmd);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		FM_ERROR_LOG("Status:(%s)\n", PQresStatus(PQresultStatus(res)));
		FM_ERROR_LOG("Failed to execute (%s) (%s)", db_cmd, PQresultErrorMessage(res));
	} else if (check_row){
		rc = atoi(PQcmdTuples(res));
		FM_DEBUG_LOG("SQL command returned successful: %d rows affected.\n", rc);
	} else {
		rc = 0;
	}

	PQclear(res);
	return rc;
}

bool CFmDBSession::params_cmd(fm_db_util_sql_params & sql_params){
	PGresult *res, *last_res;
	bool rc = true;

	if (check_conn() == false){
		FM_ERROR_LOG("Failed to reconnect: %s", PQerrorMessage(m_conn.pgconn));
		return false;
	}

	res = PQexecParams(m_conn.pgconn, sql_params.db_cmd.c_str(), sql_params.n_params,
			NULL,(const char* const*)&(sql_params.param_values[0]),
			(const int*)&(sql_params.param_lengths[0]), (const int*)&(sql_params.param_format[0]), 1);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		FM_ERROR_LOG("Status:(%s)\n", PQresStatus(PQresultStatus(res)));
		FM_ERROR_LOG("Failed to execute (%s) (%s)", sql_params.db_cmd.c_str(),
				PQresultErrorMessage(res));
		rc = false;
	}
	if (rc){
		while ((last_res=PQgetResult(m_conn.pgconn)) != NULL){
			if (PQstatus(m_conn.pgconn) == CONNECTION_BAD){
				FM_INFO_LOG("POSTGRES DB connection is bad.");
				PQclear(last_res);
				break;
			}
			FM_INFO_LOG("Waiting for POSTGRES command to finish: (%d)", PQresultStatus(last_res));
			PQclear(last_res);
			fmThreadSleep(10);
		}
		int row = atoi(PQcmdTuples(res));
		FM_DEBUG_LOG("SQL command returned successful: %d rows affected.", row);
		if (row < 1) {
			rc = false;
			FM_ERROR_LOG("SQL command returned successful, but no row affected.");
		}
	}
	PQclear(res);
	return rc;

}

//
// Copyright (c) 2014-2018 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef FMDB_H_
#define FMDB_H_

#include "fmAPI.h"

#include <libpq-fe.h>

#include <string>
#include <vector>
#include <map>

struct fm_db_util_sql_params {
	int id;
	int n_params;
	std::string db_cmd;
	std::vector<const char*> param_values;
	std::vector<int> param_lengths;
	std::vector<int> param_format;
	std::map<std::string,bool> scratch;
};

typedef std::map<std::string,std::string> fm_db_single_result_t;
typedef std::vector<fm_db_single_result_t>  fm_db_result_t;

typedef struct{
	const char *uri;         /* Connection URI */
	int status;              /* status of the connection */
	int equote;              /* use E''-style quotes for escaped strings */
	int protocol;             /* protocol version */
	int server_version;       /* server version */
	const char *encoding;     /* client encoding */
	PGconn *pgconn;          /* the postgresql connection */
}SFmDBConn;

class CFmDBSession {
protected:
	SFmDBConn m_conn;
	const char *get_parameter_status(const char *param);

public:
	CFmDBSession();
	~CFmDBSession();

	bool connect(const char *uri);
	void disconnect();
	bool check_conn();
	bool reconnect();

	bool query(const char *db_cmd,fm_db_result_t & result);
	int  cmd(const char *db_cmd, bool check_row=true);
	bool params_cmd(fm_db_util_sql_params & sql_params);

	PGconn* get_pgconn(){
		return m_conn.pgconn;
	}

};



#endif /* FMDB_H_ */

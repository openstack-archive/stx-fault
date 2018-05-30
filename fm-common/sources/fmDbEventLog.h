//
// Copyright (c) 2016 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef FMDBEVENTLOG_H_
#define FMDBEVENTLOG_H_

#include <string>
#include <vector>
#include <stddef.h>
#include <map>

#include "fmAPI.h"
#include "fmDbConstants.h"
#include "fmDb.h"

typedef std::map<int,std::string> itos_t;
typedef std::map<std::string,int> stoi_t;

class CFmDbEventLog {
public:
	typedef std::map<std::string,std::string> data_type;

	bool create_data(SFmAlarmDataT *alarm);
	bool import_data(CFmDbEventLog::data_type &m);
	bool export_data(SFmAlarmDataT *alarm);
	bool export_data(CFmDbEventLog::data_type &m);

	void append(std::string &str, const std::string &what);
	void add_both_tables(const char *str, int id, itos_t &t1,stoi_t &t2 );
    void init_tables();

	std::string find_field(const char *field);

	void set_field(const std::string &lhs, const std::string &rhs) {
		m_event_log_data[lhs] = rhs;
	}

	std::string to_formatted_db_string(const char ** list=NULL, size_t len=0);
	void print();

	static inline bool convert_to(CFmDbEventLog::data_type &m, SFmAlarmDataT *alarm ) {
		CFmDbEventLog a;
		if (!a.import_data(m)) return false;
		return a.export_data(alarm);
	}
protected:

	data_type m_event_log_data;
};


class CFmDbEventLogOperation {

public:
	bool create_event_log(CFmDBSession &sess, CFmDbEventLog &a);

	bool get_event_log(CFmDBSession &sess, AlarmFilter &af, fm_db_result_t & alarms);
	bool get_event_logs(CFmDBSession &sess, fm_db_result_t & alarms) ;

	bool get_all_event_logs(CFmDBSession &sess, SFmAlarmDataT **alarms, size_t *len );
	bool get_event_logs_by_id(CFmDBSession &sess, const char *id, fm_db_result_t & alarms);


};

#endif /* FMDBEVENTLOG_H_ */

//
// Copyright (c) 2014-2018 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef FMDBALARM_H_
#define FMDBALARM_H_

#include <string>
#include <vector>
#include <stddef.h>
#include <map>

#include "fmAPI.h"
#include "fmConstants.h"
#include "fmDb.h"

class CFmDbAlarm {
public:
	typedef std::map<std::string,std::string> data_type;

	bool import_data(CFmDbAlarm::data_type &m);
	bool create_data(SFmAlarmDataT *alarm);
	bool export_data(SFmAlarmDataT *alarm);
	bool export_data(CFmDbAlarm::data_type &m);


	std::string find_field(const char *field);

	void set_field(const std::string &lhs, const std::string &rhs) {
		m_alarm_data[lhs] = rhs;
	}

	std::string to_formatted_db_string(const char ** list=NULL, size_t len=0);
	void print();

	static inline bool convert_to(CFmDbAlarm::data_type &m, SFmAlarmDataT *alarm ) {
		CFmDbAlarm a;
		if (!a.import_data(m)) return false;
		return a.export_data(alarm);
	}
protected:

	data_type m_alarm_data;
};


class CFmDbAlarmOperation {

public:
	bool create_alarm(CFmDBSession &sess, CFmDbAlarm &a);

	int delete_alarms(CFmDBSession &sess, const char *id);
	int delete_alarm(CFmDBSession &sess, AlarmFilter &af);

	bool delete_row(CFmDBSession &sess, const char* db_table);

	bool get_alarm(CFmDBSession &sess, AlarmFilter &af, fm_db_result_t & alarms);
	bool get_alarms(CFmDBSession &sess, const char *id, fm_db_result_t & alarms) ;
	bool get_history(CFmDBSession &sess, fm_db_result_t & alarms) ;

	bool get_all_alarms(CFmDBSession &sess, SFmAlarmDataT **alarms, size_t *len );

	bool get_all_history_alarms(CFmDBSession &sess, SFmAlarmDataT **alarms, size_t *len );

	bool get_alarms_by_id(CFmDBSession &sess, const char *id, fm_db_result_t & alarms);

	bool mask_unmask_alarms(CFmDBSession &sess, SFmAlarmDataT &a, bool mask = true);

	bool add_alarm_history(CFmDBSession &sess, SFmAlarmDataT &a, bool set);
};

#endif /* FMDBALARM_H_ */

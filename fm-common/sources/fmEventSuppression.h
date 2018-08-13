//
// Copyright (c) 2016 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//


#ifndef FMEVENTSUPPRESSION_H_
#define FMEVENTSUPPRESSION_H_


#include "fmDb.h"

class CFmEventSuppressionOperation {

public:

	bool get_event_suppressed(CFmDBSession &sess, SFmAlarmDataT &data, bool &is_event_suppressed);
	bool get_single_event_suppression(CFmDBSession &sess, const char *alarm_id, fm_db_result_t & event_suppression);
	bool set_table_notify_listen(CFmDBSession &sess);
};


#endif



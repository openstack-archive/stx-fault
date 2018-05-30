//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef FMLOG_H_
#define FMLOG_H_


#include <stdio.h>
#include <syslog.h>
#include "fmAPI.h"

extern "C" {
}

enum {
	fmLogLevelCritical=LOG_CRIT,
	fmLogLevelError=LOG_ERR,
	fmLogLevelWarning=LOG_WARNING,
	fmLogLevelNotice=LOG_NOTICE,
	fmLogLevelInfo = LOG_INFO,
	fmLogLevelDebug=LOG_DEBUG,
};

#define FM_LOG_LINE_TO_STRING_impl(x) #x

#define FM_LOG_LINE_TO_STRING(x) FM_LOG_LINE_TO_STRING_impl(x)

#define PRINTF(data,...) printf(data,## __VA_ARGS__)

#define FM_TRACE_LOG(data,...) \
        PRINTF(data,## __VA_ARGS__)
              
#define FM_LOG_MSG(level,data,...) \
        fmLogMsg(level, __FILE__ "(" \
        FM_LOG_LINE_TO_STRING(__LINE__) "): " \
        data, ## __VA_ARGS__ )


#define FM_INFO_LOG(data,...) \
        FM_LOG_MSG(fmLogLevelInfo,data,## __VA_ARGS__)

#define FM_DEBUG_LOG(data,...) \
        FM_LOG_MSG(fmLogLevelDebug,data,## __VA_ARGS__)

#define FM_NOTICE_LOG(data,...) \
        FM_LOG_MSG(fmLogLevelNotice,data,## __VA_ARGS__)

#define FM_WARNING_LOG(data,...) \
        FM_LOG_MSG(fmLogLevelWarning,data,## __VA_ARGS__)
	
#define FM_ERROR_LOG(data,...) \
        FM_LOG_MSG(fmLogLevelError,data,## __VA_ARGS__)
        
#define FM_CRITICAL_LOG(data,...) \
        FM_LOG_MSG(fmLogLevelCritical,data,## __VA_ARGS__)
	
#define FM_MAX_LOG_LENGTH 992


void fmLoggingInit();

void fmLogMsg(int level, const char *data, ...);

bool fmLogFileInit();

void fmLogAddEventLog(SFmAlarmDataT * data, bool is_event_suppressed);

//void fmLogAddEventLog(SFmAlarmDataT * data);


#endif



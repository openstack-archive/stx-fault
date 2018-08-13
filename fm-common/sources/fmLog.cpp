//
// Copyright (c) 2014-2018 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <syslog.h>

#include "fmLog.h"
#include "fmDbAlarm.h"
#include "fmDbEventLog.h"
#include "fmConfig.h"
#include "fmConstants.h"

static pthread_mutex_t mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

static char * escape_json(const char * s, char * buffer, const int bufLen);

void fmLoggingInit() {
	static bool has_inited=false;
	if (!has_inited){
		openlog(NULL,LOG_CONS | LOG_NDELAY,LOG_LOCAL1);
		setlogmask(LOG_UPTO (LOG_INFO));
	}
	std::string val;
	std::string key = FM_DEBUG_FLAG;
	if ((fm_get_config_key(key, val)) && (val.compare("True") == 0)){
		setlogmask(LOG_UPTO (LOG_DEBUG));
	} else {
		setlogmask(LOG_UPTO (LOG_INFO));
	}
	has_inited=true;
}

void fmLogMsg(int level, const char *data,...){

	va_list ap;

	va_start(ap, data);
	vsyslog(level,data,ap);
	va_end(ap);
}

// formats event into json form for logging
static char * formattedEvent(CFmDbEventLog::data_type event_map, char * output, int outputSize) {
    int bufLen = 1024;
    char * buffer = (char *) malloc(bufLen);

    int outputLen = 0;
    snprintf(output + outputLen, outputSize - outputLen, "{ \"%s\" : \"%s\", ", "event_log_id",       escape_json(event_map["event_log_id"].c_str(), buffer, bufLen));

    outputLen = strlen(output);
    snprintf(output + outputLen, outputSize - outputLen,   "\"%s\" : \"%s\", ", "reason_text",        escape_json(event_map["reason_text"].c_str(), buffer, bufLen));

    outputLen = strlen(output);
    snprintf(output + outputLen, outputSize - outputLen,   "\"%s\" : \"%s\", ", "entity_instance_id", escape_json(event_map["entity_instance_id"].c_str(), buffer, bufLen));

    outputLen = strlen(output);
    snprintf(output + outputLen, outputSize - outputLen,   "\"%s\" : \"%s\", ", "severity",           escape_json(event_map["severity"].c_str(), buffer, bufLen));

    outputLen = strlen(output);
    snprintf(output + outputLen, outputSize - outputLen,    "\"%s\" : \"%s\", ", "state",             escape_json(event_map["state"].c_str(), buffer, bufLen));

    outputLen = strlen(output);
    snprintf(output + outputLen, outputSize - outputLen,    "\"%s\" : \"%s\" }", "timestamp",         escape_json(event_map["timestamp"].c_str(), buffer, bufLen));

    free(buffer);
    return output;
}

// logs event to syslog
void fmLogEVT(int level, CFmDbEventLog::data_type event_map) {
    char * output = (char*) malloc(2048);
    char * msg = formattedEvent(event_map, output, 2048);
    fmLogMsg(level,msg);
    free(output);
}

void fmLogAddEventLog(SFmAlarmDataT * data, bool is_event_suppressed){
	pthread_mutex_lock(&mutex);
	CFmDbEventLog event;
	CFmDbEventLog::data_type event_map;

	event.create_data(data);
	event.export_data(event_map);

	fmLogEVT(fmLogLevelInfo | LOG_LOCAL5,  event_map);

	pthread_mutex_unlock(&mutex);
}

// Converts a string (s) to be json safe.  Special characters are escaped and written to buffer.
static char * escape_json(const char * s, char * buffer, const int bufLen) {

    int sLen = -1;
    int bufLeft = bufLen;
    int s_i = 0;
    int buf_i = 0;
    int ch;
    char tmp_buf[16];
    char * escapedChrs = NULL;
    int  escapedChrsLen = -1;

    if (s==NULL || buffer==NULL || bufLen<1) {
       return buffer;
    }

    sLen = strlen(s);
    buffer[0] = 0;

    if (s == 0) {
       return buffer;
    }

    while (s_i < sLen && bufLeft > 0) {
       ch = s[s_i];
       switch (ch) {
            case '\"':
                escapedChrs = (char *) "\\\"";
                break;
            case '\\':
                escapedChrs =  (char *) "\\\\";
                break;
            case '\b':
                escapedChrs = (char *) "\\b";
                break;
            case '\f':
                escapedChrs = (char *) "\\f";
                break;
            case '\n':
                escapedChrs = (char *) "\\n";
                break;
            case '\r':
                escapedChrs = (char *) "\\r";
                break;
            case '\t':
                escapedChrs = (char *) "\\t";
                break;
            default:
                if (ch < 0) {
                    ch = 0xFFFD; /* replacement character */
                }
                if (ch > 0xFFFF) {
                    /* beyond BMP (Basic Multilingual Plane); need a surrogate pair
                     * for reference, see: https://en.wikipedia.org/wiki/Plane_%28Unicode%29
                    */
                    snprintf(tmp_buf, sizeof(tmp_buf), "\\u%04X\\u%04X",
                             0xD800 + ((ch - 0x10000) >> 10),
                             0xDC00 + ((ch - 0x10000) & 0x3FF));
                } else if (ch < 0x20 || ch >= 0x7F) {
                    snprintf(tmp_buf, sizeof(tmp_buf), "\\u%04X", ch);
                } else {
                    tmp_buf[0] = ch;
                    tmp_buf[1] = 0;
                }
                escapedChrs = tmp_buf;
       }
       escapedChrsLen = strlen(escapedChrs);
       if (bufLeft <= escapedChrsLen) {
          break;
       }
       strncpy(&buffer[buf_i], escapedChrs, bufLeft );
       bufLeft -= escapedChrsLen;
       buf_i   += escapedChrsLen;
       s_i++;
    }

    return buffer;
}

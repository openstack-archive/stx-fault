//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include "fmAPI.h"
#include "fmAlarmUtils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>

void error(const char *add="") {
	printf("%s\n",add);
	exit(-1);
}

void ok(const char *s="") {
	printf("Ok\n%s%s",s,strlen(s)!=0 ? "\n":"");
	exit(0);
}

void ok(fm_uuid_t *uuid) {
	return ok(*uuid);
}

void create_alarm(const char *al) {
	SFmAlarmDataT alarm;
	EFmErrorT rc;

	memset(&alarm,0,sizeof(alarm));

	if (!fm_alarm_from_string(al,&alarm)) {
		error("Invalid alarm string..");
	}

	//clear uuid and let it be set by fm-mgr
	memset(alarm.uuid, 0, sizeof(*alarm.uuid));
	fm_uuid_t uuid;
	if ((rc = fm_set_fault(&alarm,&uuid))!=FM_ERR_OK) {
		std::string errstr = fm_error_from_int(rc);
		error(errstr.c_str());
	}
	ok(&uuid);
}

void delete_alarms(const char *id) {
	fm_ent_inst_t inst_id;
	EFmErrorT rc;

	memset(inst_id, 0 , sizeof(inst_id));
	strncpy(inst_id, id ,sizeof(inst_id)-1);

	if ((rc = fm_clear_all(&inst_id))!=FM_ERR_OK) {
		std::string errstr = fm_error_from_int(rc);
		error(errstr.c_str());
	}
	ok();
}

void delete_alarm(const char *f) {
	AlarmFilter af;
	EFmErrorT rc;

	memset(&af,0,sizeof(af));
	if (!fm_alarm_filter_from_string(f,&af)) {
		error("Invalid alarm filter");
	}
	if ((rc = fm_clear_fault(&af))!=FM_ERR_OK) {
		std::string errstr = fm_error_from_int(rc);
		error(errstr.c_str());
	}
	ok();
}

void get_alarm(const char *f) {
	AlarmFilter af;
	EFmErrorT rc;

	memset(&af, 0, sizeof(af));
	if (!fm_alarm_filter_from_string(f,&af)) {
		error("Invalid alarm filter");
	}

	SFmAlarmDataT ad;
	memset(&ad, 0 , sizeof(ad));

	if ((rc = fm_get_fault(&af,&ad))!=FM_ERR_OK) {
		std::string errstr = fm_error_from_int(rc);
		error(errstr.c_str());
	}
	std::string s;
	fm_alarm_to_string(&ad,s);
	ok(s.c_str());
}

void get_alarms(const char *id) {
	fm_ent_inst_t inst_id;
	EFmErrorT rc;

	memset(inst_id, 0 , sizeof(inst_id));
	strncpy(inst_id, id ,sizeof(inst_id)-1);

	unsigned int max_alarms=500;
	SFmAlarmDataT *p = (SFmAlarmDataT*)malloc(max_alarms*sizeof(SFmAlarmDataT));
	if (p==NULL) error("Not enough memory...");
	if ((rc=fm_get_faults(&inst_id,p,&max_alarms))!=FM_ERR_OK) {
		free(p);
		std::string errstr = fm_error_from_int(rc);
		error(errstr.c_str());
	}
	printf("Ok\n");

	size_t ix = 0;
	for ( ; ix < max_alarms; ++ix ) {
		std::string a;
		fm_alarm_to_string(p+ix,a);
		printf("%s\n",a.c_str());
	}
	free(p);
	exit(0);
}

void get_alarms_by_id(const char *id) {
	fm_alarm_id alm_id;
	EFmErrorT rc;

	memset(alm_id, 0 , sizeof(alm_id));
	strncpy(alm_id, id ,sizeof(alm_id)-1);

	unsigned int max_alarms=500;
	SFmAlarmDataT *p = (SFmAlarmDataT*)malloc(max_alarms*sizeof(SFmAlarmDataT));
	if (p==NULL) error("Not enough memory...");
	if ((rc=fm_get_faults_by_id(&alm_id,p,&max_alarms))!=FM_ERR_OK) {
		free(p);
		std::string errstr = fm_error_from_int(rc);
		error(errstr.c_str());
	}
	printf("Ok\n");

	size_t ix = 0;
	for ( ; ix < max_alarms; ++ix ) {
		std::string a;
		fm_alarm_to_string(p+ix,a);
		printf("%s\n",a.c_str());
	}
	free(p);
	exit(0);
}

int main(int argc, char**argv) {
	int c;

	if (argc < 3) {
		error("Wrong arguments");
	}
	while ( (c=getopt(argc,argv,"c:d:D:g:G:A:"))) {
		switch(c) {
		case 'c':
			create_alarm(optarg);
			break;
		case 'd':
			delete_alarm(optarg);
			break;
		case 'D':
			delete_alarms(optarg);
			break;
		case 'g':
			get_alarm(optarg);
			break;
		case 'G':
			get_alarms(optarg);
			break;
		case 'A':
			get_alarms_by_id(optarg);
			break;
		default:
			error("Invalid option...");
			break;
		}
	}

	return 0;
}



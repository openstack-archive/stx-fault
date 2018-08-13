//
// Copyright (c) 2018 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include "fmAPI.h"
#include "fmLog.h"
#include "fmFile.h"
#include "fmConfig.h"
#include "fmMutex.h"
#include "fmConstants.h"
#include "fmSnmpConstants.h"
#include "fmSnmpUtils.h"

typedef std::map<std::string,std::string> configParams;

static const char *conf = NULL;
static int config_loaded = false;

CFmMutex & getConfMutex(){
	static CFmMutex *m = new CFmMutex;
	return *m;
}

configParams &getConfigMap(){
	static configParams conf;
	return conf;
}

void fm_conf_set_file(const char *fn){
	conf = fn;
}

void fm_get_config_paramters(){
	CfmFile f;
	std::string delimiter = "=";
	std::string line, key, value;
	size_t pos = 0;

	if  (conf == NULL){
		FM_ERROR_LOG("The config file is not set\n");
		exit(-1);
	}

	if (!f.open(conf, CfmFile::READ, false)){
		FM_ERROR_LOG("Failed to open config file: %s\n", conf);
		exit(-1);
	}

	while (true){
		if (!f.read_line(line)) break;

		if (line.size() == 0) continue;

		if (line[0] == '#') continue;

		pos = line.find(delimiter);
		key = line.substr(0, pos);
		value = line.erase(0, pos + delimiter.length());
		getConfigMap()[key] = value;
		if (key.compare(FM_SNMP_TRAPDEST) == 0){
			set_trap_dest_list(value);
		}
		if (key.compare(FM_SQL_CONNECTION) != 0){
			// Don't log sql_connection, as it has a password
			FM_INFO_LOG("Config key (%s), value (%s)",
			            key.c_str(), value.c_str());
		}
	}
}

bool fm_get_config_key(std::string &key, std::string &val){
	configParams::iterator it;
	CFmMutexGuard m(getConfMutex());

	if (!config_loaded){
		fm_get_config_paramters();
		config_loaded = true;
	}

	it = getConfigMap().find(key);
	if (it != getConfigMap().end()){
		val = it->second;
		return true;
	}
	return false;
}

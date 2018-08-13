//
// Copyright (c) 2018 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef FMCONFIG_H_
#define FMCONFIG_H_

#include <string>

void fm_conf_set_file(const char *fn);

void fm_get_config_paramters();

bool fm_get_config_key(std::string &key, std::string &val);

#endif /* FMCONFIG_H_ */

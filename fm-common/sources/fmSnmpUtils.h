//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef __FM_SNMP_UTILS_H
#define __FM_SNMP_UTILS_H

#include <string>

#include "fmAPI.h"
#include "fmDb.h"

bool fm_snmp_util_gen_trap(int type, SFmAlarmDataT &data);

void set_trap_dest_list(std::string value);

#endif

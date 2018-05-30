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

bool fm_snmp_util_gen_trap(CFmDBSession &sess, int type, SFmAlarmDataT &data);

#endif

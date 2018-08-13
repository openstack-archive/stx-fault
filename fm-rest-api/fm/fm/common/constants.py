#
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#

import os
import tsconfig.tsconfig as tsc


FM_SUPPRESSED = 'suppressed'
FM_UNSUPPRESSED = 'unsuppressed'

DB_SUPPRESS_STATUS = 1
DB_MGMT_AFFECTING = 2
DB_DEGRADE_AFFECTING = 3


FM_LOCK_PATH = os.path.join(tsc.VOLATILE_PATH, "fm")

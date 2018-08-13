# vim: tabstop=4 shiftwidth=4 softtabstop=4

# Copyright (c) 2012 Intel Corporation.
# All Rights Reserved.
#
#    Licensed under the Apache License, Version 2.0 (the "License"); you may
#    not use this file except in compliance with the License. You may obtain
#    a copy of the License at
#
#         http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
#    WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
#    License for the specific language governing permissions and limitations
#    under the License.
#
# Copyright (c) 2013-2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#


import six
import uuid
from oslo_log import log
from oslo_concurrency import lockutils

from fm.common import constants

LOG = log.getLogger(__name__)


def generate_uuid():
    return str(uuid.uuid4())


def synchronized(name, external=True):
    if external:
        lock_path = constants.FM_LOCK_PATH
    else:
        lock_path = None
    return lockutils.synchronized(name,
                                  lock_file_prefix='fm-',
                                  external=external,
                                  lock_path=lock_path)


def safe_rstrip(value, chars=None):
    """Removes trailing characters from a string if that does not make it empty

    :param value: A string value that will be stripped.
    :param chars: Characters to remove.
    :return: Stripped value.

    """
    if not isinstance(value, six.string_types):
        LOG.warning("Failed to remove trailing character. "
                    "Returning original object. "
                    "Supplied object is not a string: %s,", value)
        return value

    return value.rstrip(chars) or value

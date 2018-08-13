#    Copyright 2013 IBM Corp.
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
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#


"""Utility methods for objects"""

import datetime
import iso8601
import uuid
import six

from fm.common.i18n import _
from fm.common import timeutils


def datetime_or_none(dt):
    """Validate a datetime or None value."""
    if dt is None:
        return None
    elif isinstance(dt, datetime.datetime):
        if dt.utcoffset() is None:
            # NOTE(danms): Legacy objects from sqlalchemy are stored in UTC,
            # but are returned without a timezone attached.
            # As a transitional aid, assume a tz-naive object is in UTC.
            return dt.replace(tzinfo=iso8601.iso8601.Utc())
        else:
            return dt
    raise ValueError('A datetime.datetime is required here')


def datetime_or_str_or_none(val):
    if isinstance(val, basestring):
        return timeutils.parse_isotime(val)
    return datetime_or_none(val)


def int_or_none(val):
    """Attempt to parse an integer value, or None."""
    if val is None:
        return val
    else:
        return int(val)


def str_or_none(val):
    """Attempt to stringify a value, or None."""
    if val is None:
        return val
    else:
        return six.text_type(val)


def uuid_or_none(val):
    """Attempt to dictify a value, or None."""
    if val is None:
        return None
    elif isinstance(val, basestring):
        return str(uuid.UUID(val.strip()))
    raise ValueError(_('Invalid UUID value %s') % val)


def dt_serializer(name):
    """Return a datetime serializer for a named attribute."""
    def serializer(self, name=name):
        if getattr(self, name) is not None:
            return timeutils.isotime(getattr(self, name))
        else:
            return None
    return serializer


def dt_deserializer(instance, val):
    """A deserializer method for datetime attributes."""
    if val is None:
        return None
    else:
        return timeutils.parse_isotime(val)

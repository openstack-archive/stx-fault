#
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#


import functools

from fm.objects import alarm
from fm.objects import event_log
from fm.objects import event_suppression


def objectify(klass):
    """Decorator to convert database results into specified objects.
    :param klass: database results class
    """

    def the_decorator(fn):
        @functools.wraps(fn)
        def wrapper(*args, **kwargs):
            result = fn(*args, **kwargs)
            try:
                return klass.from_db_object(result)
            except TypeError:
                return [klass.from_db_object(obj) for obj in result]

        return wrapper

    return the_decorator


alarm = alarm.Alarm
event_log = event_log.EventLog
event_suppression = event_suppression.EventSuppression

__all__ = (alarm,
           event_log,
           event_suppression)


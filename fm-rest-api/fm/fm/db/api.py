#
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#

"""
Base classes for storage engines
"""

import abc

from oslo_config import cfg
from oslo_db import api as db_api


_BACKEND_MAPPING = {'sqlalchemy': 'fm.db.sqlalchemy.api'}
IMPL = db_api.DBAPI.from_config(cfg.CONF, backend_mapping=_BACKEND_MAPPING,
                                lazy=True)


def get_instance():
    """Return a DB API instance."""
    return IMPL


class Connection(object):
    """Base class for storage system connections."""

    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def __init__(self):
        """Constructor."""

    @abc.abstractmethod
    def alarm_create(self, values):
        """Create a new alarm.

        :param values: A dict containing several items used to identify
                       and track the alarm.
        :returns: An alarm.
        """

    @abc.abstractmethod
    def alarm_get(self, uuid):
        """Return an alarm.

        :param uuid: The uuid of an alarm.
        :returns: An alarm.
        """

    @abc.abstractmethod
    def alarm_get_by_ids(self, alarm_id, entity_instance_id):
        """Return an alarm.

        :param alarm_id: The alarm_id of an alarm.
        :param entity_instance_id: The entity_instance_id of an alarm.
        :returns: An alarm.
        """

    @abc.abstractmethod
    def alarm_get_all(self, uuid=None, alarm_id=None, entity_type_id=None,
                      entity_instance_id=None, severity=None, alarm_type=None):
        """Return a list of alarms for the given filters.

        :param uuid: The uuid of an alarm.
        :param alarm_id: The alarm_id of an alarm.
        :param entity_type_id: The entity_type_id of an alarm.
        :param entity_instance_id: The entity_instance_id of an alarm.
        :param severity: The severity of an alarm.
        :param alarm_type: The alarm_type of an alarm.
        :returns:  alarms.
        """

    @abc.abstractmethod
    def alarm_get_list(self, limit=None, marker=None,
                       sort_key=None, sort_dir=None):
        """Return a list of alarms.

        :param limit: Maximum number of alarm to return.
        :param marker: the last item of the previous page; we return the next
                       result set.
        :param sort_key: Attribute by which results should be sorted.
        :param sort_dir: direction in which results should be sorted.
                         (asc, desc)
        """

    @abc.abstractmethod
    def alarm_update(self, id, values):
        """Update properties of an alarm.

        :param id: The id or uuid of an alarm.
        :param values: Dict of values to update.

        :returns: An alarm.
        """

    @abc.abstractmethod
    def alarm_destroy(self, id):
        """Destroy an alarm.

        :param id: The id or uuid of an alarm.
        """

    @abc.abstractmethod
    def alarm_destroy_by_ids(self, alarm_id, entity_instance_id):
        """Destroy an alarm.

        :param alarm_id: The alarm_id of an alarm.
        :param entity_instance_id: The entity_instance_id of an alarm.

        """

    @abc.abstractmethod
    def event_log_get(self, uuid):
        """Return an event_log.

        :param uuid: The uuid of an event_log.
        :returns: An event_log.
        """

    @abc.abstractmethod
    def event_log_get_all(self, uuid=None, event_log_id=None,
                          entity_type_id=None, entity_instance_id=None,
                          severity=None, event_log_type=None, start=None,
                          end=None, limit=None):
        """Return a list of event_log for the given filters.

        :param uuid: The uuid of an event_log.
        :param event_log_id: The id of an event_log.
        :param entity_type_id: The entity_type_id of an event_log.
        :param entity_instance_id: The entity_instance_id of an event_log.
        :param severity: The severity of an event_log.
        :param alarm_type: The alarm_type of an event_log.
        :param start: The event_logs that occurred after start
        :param end: The event_logs that occurred before end
        :returns:  event_log.
        """

    @abc.abstractmethod
    def event_log_get_list(self, limit=None, marker=None,
                           sort_key=None, sort_dir=None, evtType="ALL"):
        """Return a list of event_log.

        :param limit: Maximum number of event_log to return.
        :param marker: the last item of the previous page; we return the next
                       result set.
        :param sort_key: Attribute by which results should be sorted.
        :param sort_dir: direction in which results should be sorted.
                         (asc, desc)
        """

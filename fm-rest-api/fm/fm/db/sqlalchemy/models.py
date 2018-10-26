# vim: tabstop=4 shiftwidth=4 softtabstop=4
# -*- encoding: utf-8 -*-
#
# Copyright 2013 Hewlett-Packard Development Company, L.P.
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


import json
try:
    import urlparse
except ImportError:
    import urllib.parse as urlparse

from oslo_config import cfg

from sqlalchemy import Column, ForeignKey, Integer, Boolean
from sqlalchemy import String
from sqlalchemy import DateTime
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.types import TypeDecorator, VARCHAR
from oslo_db.sqlalchemy import models


def table_args():
    engine_name = urlparse.urlparse(cfg.CONF.database_connection).scheme
    if engine_name == 'mysql':
        return {'mysql_engine': 'InnoDB',
                'mysql_charset': "utf8"}
    return None


class JSONEncodedDict(TypeDecorator):
    """Represents an immutable structure as a json-encoded string."""

    impl = VARCHAR

    def process_bind_param(self, value, dialect):
        if value is not None:
            value = json.dumps(value)
        return value

    def process_result_value(self, value, dialect):
        if value is not None:
            value = json.loads(value)
        return value


class FmBase(models.TimestampMixin, models.ModelBase):

    metadata = None

    def as_dict(self):
        d = {}
        for c in self.__table__.columns:
            d[c.name] = self[c.name]
        return d


Base = declarative_base(cls=FmBase)


class Alarm(Base):
    __tablename__ = 'alarm'

    id = Column(Integer, primary_key=True, nullable=False)
    uuid = Column(String(255), unique=True, index=True)
    alarm_id = Column('alarm_id', String(255),
                      ForeignKey('event_suppression.alarm_id'),
                      nullable=True, index=True)
    alarm_state = Column(String(255))
    entity_type_id = Column(String(255), index=True)
    entity_instance_id = Column(String(255), index=True)
    timestamp = Column(DateTime(timezone=False))
    severity = Column(String(255), index=True)
    reason_text = Column(String(255))
    alarm_type = Column(String(255), index=True)
    probable_cause = Column(String(255))
    proposed_repair_action = Column(String(255))
    service_affecting = Column(Boolean, default=False)
    suppression = Column(Boolean, default=False)
    inhibit_alarms = Column(Boolean, default=False)
    masked = Column(Boolean, default=False)


class EventLog(Base):
    __tablename__ = 'event_log'

    id = Column(Integer, primary_key=True, nullable=False)
    uuid = Column(String(255), unique=True, index=True)
    event_log_id = Column('event_log_id', String(255),
                          ForeignKey('event_suppression.alarm_id'),
                          nullable=True, index=True)
    state = Column(String(255))
    entity_type_id = Column(String(255), index=True)
    entity_instance_id = Column(String(255), index=True)
    timestamp = Column(DateTime(timezone=False))
    severity = Column(String(255), index=True)
    reason_text = Column(String(255))
    event_log_type = Column(String(255), index=True)
    probable_cause = Column(String(255))
    proposed_repair_action = Column(String(255))
    service_affecting = Column(Boolean, default=False)
    suppression = Column(Boolean, default=False)


class EventSuppression(Base):
    __tablename__ = 'event_suppression'

    id = Column('id', Integer, primary_key=True, nullable=False)
    uuid = Column('uuid', String(36), unique=True)
    alarm_id = Column('alarm_id', String(255), unique=True)
    description = Column('description', String(255))
    suppression_status = Column('suppression_status', String(255))
    set_for_deletion = Column('set_for_deletion', Boolean)
    mgmt_affecting = Column('mgmt_affecting', String(255))
    degrade_affecting = Column('degrade_affecting', String(255))

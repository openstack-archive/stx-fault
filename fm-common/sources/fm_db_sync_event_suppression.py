#!/usr/bin/env python
# Copyright (c) 2016-2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#

import sys
import os
import json
import datetime
import uuid as uuid_gen
import socket
from inspect import getframeinfo
from inspect import stack

import yaml
import collections

import sqlalchemy
from sqlalchemy.orm import sessionmaker
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column, Boolean, Integer, String, DateTime

FM_LOG_EVENT_LOG_FILE = "/var/log/platform.log"

Base = declarative_base()


class EventSuppression(Base):
    __tablename__ = 'event_suppression'
    created_at = Column('created_at', DateTime)
    id = Column('id', Integer, primary_key=True, nullable=False)
    uuid = Column('uuid', String(36), unique=True)
    alarm_id = Column('alarm_id', String(255), unique=True)
    description = Column('description', String(255))
    suppression_status = Column('suppression_status', String(255))
    set_for_deletion = Column('set_for_deletion', Boolean)
    mgmt_affecting = Column('mgmt_affecting', String(255))
    degrade_affecting = Column('degrade_affecting', String(255))


class ialarm(Base):
    __tablename__ = 'alarm'
    id = Column(Integer, primary_key=True, nullable=False)
    alarm_id = Column('alarm_id', String(255), index=True)


class event_log(Base):
    __tablename__ = 'event_log'
    id = Column(Integer, primary_key=True, nullable=False)
    event_log_id = Column('event_log_id', String(255), index=True)
    state = Column(String(255))


def prettyDict(dict):
    output = json.dumps(dict, sort_keys=True, indent=4)
    return output


def logInfo(msg):
    try:
        timestamp = str(datetime.datetime.now())
        host_name = socket.gethostname()
        caller = getframeinfo(stack()[1][0])
        line_no = str(caller.lineno)
        output = "\n" + timestamp + " " + host_name + " fmManager: info " \
                 + current_file_name + "(" + line_no + "):" + " " + msg + "\n"
        with open(FM_LOG_EVENT_LOG_FILE, "a") as logFile:
            logFile.write(output)
    except Exception as e:
        print(e)


def get_events_yaml_filename():
    events_yaml_name = os.environ.get("EVENTS_YAML")
    if events_yaml_name is not None and os.path.isfile(events_yaml_name):
        return events_yaml_name
    return "/etc/fm/events.yaml"


#
# Main
#

if len(sys.argv) < 2:
    sys.exit("Postgres credentials required as argument.")

postgresql_credentials = str(sys.argv[1])

# Set up logging:
current_file_name = __file__
current_file_name = current_file_name[2:]  # remove leading characters "./"

# Set up sqlalchemy:
meta = sqlalchemy.MetaData()
engine = sqlalchemy.create_engine(postgresql_credentials)

meta.bind = engine

Session = sessionmaker(bind=engine)
session = Session()

# Convert events.yaml to dict:
EVENT_TYPES_FILE = get_events_yaml_filename()

if not os.path.isfile(EVENT_TYPES_FILE):
    exit(-1)

with open(EVENT_TYPES_FILE, 'r') as stream:
    event_types = yaml.load(stream)

for alarm_id in event_types:
    if isinstance(alarm_id, float):
        # force 3 digits after the decimal point,
        # to include trailing zero's (ex.: 200.010)
        formatted_alarm_id = "{:.3f}".format(alarm_id)
        event_types[formatted_alarm_id] = event_types.pop(alarm_id)

event_types = collections.OrderedDict(sorted(event_types.items()))

yaml_event_list = []
uneditable_descriptions = {'100.114', '200.007', '200.02', '200.021', '200.022', '800.002'}

# Parse events.yaml dict, and add any new alarm to event_suppression table:
for event_type in event_types:

    if event_types.get(event_type).get('Type') == "Alarm":
        event_created_at = datetime.datetime.now()
        event_uuid = str(uuid_gen.uuid4())

        string_event_type = str(event_type)

        yaml_event_list.append(string_event_type)

        if str(event_type) not in uneditable_descriptions:
            event_description = (event_types.get(event_type)
                                 .get('Description'))
        else:
            event_description = event_types.get(event_type).get('Description')

        event_description = str(event_description)
        event_description = (event_description[:250] + ' ...') \
            if len(event_description) > 250 else event_description

        event_supp = session.query(EventSuppression) \
                            .filter_by(alarm_id=string_event_type).first()

        event_mgmt_affecting = str(event_types.get(event_type).get(
            'Management_Affecting_Severity', 'warning'))

        event_degrade_affecting = str(event_types.get(event_type).get(
            'Degrade_Affecting_Severity', 'none'))

        if event_supp:
            event_supp.description = event_description
            event_supp.mgmt_affecting = event_mgmt_affecting
            event_supp.degrade_affecting = event_degrade_affecting
        else:
            event_supp = EventSuppression(created_at=event_created_at,
                                          uuid=event_uuid,
                                          alarm_id=string_event_type,
                                          description=event_description,
                                          suppression_status='unsuppressed',
                                          set_for_deletion=False,
                                          mgmt_affecting=event_mgmt_affecting,
                                          degrade_affecting=event_degrade_affecting)
            session.add(event_supp)
            logInfo("Created Event Type {} in event_suppression table.".format(string_event_type))

        session.commit()

event_supp = session.query(EventSuppression)
alarms = session.query(ialarm)
events = session.query(event_log).filter(event_log.state != 'log')

alarm_ids_in_use = set()
for alarm in alarms:
    alarm_ids_in_use.add(alarm.alarm_id)

for event in events:
    alarm_ids_in_use.add(event.event_log_id)

for event_type in event_supp:
    if event_type.alarm_id not in yaml_event_list:
        if event_type.alarm_id not in alarm_ids_in_use:
            event_supp = session.query(EventSuppression) \
                                .filter_by(alarm_id=event_type.alarm_id).first()
            session.delete(event_supp)
            logInfo("Deleted Event Type {} from event_suppression table.".format(event_type.alarm_id))
        else:
            event_supp.suppression_status = 'unsuppressed'
            event_supp.set_for_deletion = True
            logInfo("Event Type {} no longer in events.yaml, but still used by alarm in database.".format(event_type.alarm_id))
            logInfo("Event Type {} marked as set for deletion in event_suppression table.".format(event_type.alarm_id))

        session.commit()

session.close()

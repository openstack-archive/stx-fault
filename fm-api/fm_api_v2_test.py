# -*- encoding: utf-8 -*-
#
# Copyright (c) 2014 Wind River Systems, Inc.
#
# Author:
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

import sys
from fm_api import fm_api
from fm_api import constants

ser = fm_api.FaultAPIsV2()


def print_alarm(alarm):
    alarm_str = "alarm_id: " + alarm.alarm_id + ", "
    alarm_str += "uuid: " + alarm.uuid + ", "
    alarm_str += "alarm_type: " + alarm.alarm_type + "\n"
    alarm_str += "state: " + alarm.alarm_state + ", "
    alarm_str += "severity: " + alarm.severity + ", "
    alarm_str += "entity_type_id: " + alarm.entity_type_id + ", "
    alarm_str += "timestamp: " + alarm.timestamp + "\n"
    alarm_str += "entity_instance_id: " + alarm.entity_instance_id + ", "
    alarm_str += "probable cause:" + alarm.probable_cause + "\n"
    print(alarm_str)


def create():
    fault = fm_api.Fault(alarm_id=constants.FM_ALARM_ID_VM_FAILED,
                         alarm_state=constants.FM_ALARM_STATE_SET,
                         entity_type_id=constants.FM_ENTITY_TYPE_INSTANCE,
                         entity_instance_id=constants.FM_ENTITY_TYPE_INSTANCE + '=' + 'a4e4cdb7-2ee6-4818-84c8-5310fcd67b5d',
                         severity=constants.FM_ALARM_SEVERITY_CRITICAL,
                         reason_text="Unknown",
                         alarm_type=constants.FM_ALARM_TYPE_5,
                         probable_cause=constants.ALARM_PROBABLE_CAUSE_8,
                         proposed_repair_action=None,
                         service_affecting=False,
                         suppression=False)
    try:
        uuid = ser.set_fault(fault)
        print(uuid)
    except Exception:
        print("Fail to create fault")


def delete(alarm_id, instance_id):
    try:
        ser.clear_fault(alarm_id, instance_id)
        print("Delete fault success")
    except Exception:
        print("Fail to delete fault")


def del_all(instance_id):
    try:
        ser.clear_all(instance_id)
        print("Delete faults success")
    except Exception:
        print("Fail to delete faults")


def get(alarm_id, instance_id):
    try:
        a = ser.get_fault(alarm_id, instance_id)
        if a is not None:
            print_alarm(a)
        else:
            print("Alarm not found")
    except Exception:
        print("Fail to get alarm")


def get_all(instance_id):
    try:
        ll = ser.get_faults(instance_id)
        if ll is not None:
            print("Total alarm returned: %d\n"
                  % len(ll))
            for i in ll:
                print_alarm(i)
        else:
            print("No alarm returned")
    except Exception:
        print("Fail to get alarm")


def get_list(alarm_id):
    try:
        ll = ser.get_faults_by_id(alarm_id)
        if ll is not None:
            print("Total alarm returned: %d\n"
                  % len(ll))
            for i in ll:
                print_alarm(i)
        else:
            print("No alarm returned")
    except Exception:
        print("Fail to get alarm")


if __name__ == "__main__":
    if sys.argv[1] == "create":
        sys.exit(create())
    elif sys.argv[1] == "del":
        sys.exit(delete(sys.argv[2], sys.argv[3]))
    elif sys.argv[1] == "get":
        sys.exit(get(sys.argv[2], sys.argv[3]))
    elif sys.argv[1] == "get_all":
        sys.exit(get_all(sys.argv[2]))
    elif sys.argv[1] == "del_all":
        sys.exit(del_all(sys.argv[2]))
    elif sys.argv[1] == "get_list":
        sys.exit(get_list(sys.argv[2]))

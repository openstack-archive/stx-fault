#
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#

from oslo_versionedobjects import base as object_base

from fm.db import api as db_api
from fm.objects import base
from fm.objects import utils


class EventSuppression(base.FmObject):

    VERSION = '1.0'

    dbapi = db_api.get_instance()

    fields = {
        'id': int,
        'uuid': utils.uuid_or_none,
        'alarm_id': utils.str_or_none,
        'description': utils.str_or_none,
        'suppression_status': utils.str_or_none,
    }

    @object_base.remotable_classmethod
    def get_by_uuid(cls, context, uuid):
        return cls.dbapi.event_suppression_get(uuid)

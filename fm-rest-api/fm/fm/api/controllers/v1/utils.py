#
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#


import sys
import contextlib
import traceback
import pecan
import wsme
from oslo_config import cfg
from oslo_log import log
from oslo_utils import uuidutils

from fm.api.controllers.v1.sysinv import cgtsclient
from fm.common import exceptions
from fm.common.i18n import _


CONF = cfg.CONF

LOG = log.getLogger(__name__)

ALARM_ENTITY_TYPES_USE_UUID = ['port']
ENTITY_SEP = '.'
KEY_VALUE_SEP = '='


@contextlib.contextmanager
def save_and_reraise_exception():
    """Save current exception, run some code and then re-raise.

    In some cases the exception context can be cleared, resulting in None
    being attempted to be re-raised after an exception handler is run. This
    can happen when eventlet switches greenthreads or when running an
    exception handler, code raises and catches an exception. In both
    cases the exception context will be cleared.

    To work around this, we save the exception state, run handler code, and
    then re-raise the original exception. If another exception occurs, the
    saved exception is logged and the new exception is re-raised.
    """
    type_, value, tb = sys.exc_info()
    try:
        yield
    except Exception:
        LOG.error(_('Original exception being dropped: %s'),
                  traceback.format_exception(type_, value, tb))
        raise
    raise type_, value, tb


def validate_limit(limit):
    if limit and limit < 0:
        raise wsme.exc.ClientSideError(_("Limit must be positive"))

    return min(CONF.api.limit_max, limit) or CONF.api.limit_max


def validate_sort_dir(sort_dir):
    if sort_dir not in ['asc', 'desc']:
        raise wsme.exc.ClientSideError(_("Invalid sort direction: %s. "
                                         "Acceptable values are "
                                         "'asc' or 'desc'") % sort_dir)
    return sort_dir


def _get_port(host_name, port_name):
    hosts = cgtsclient(pecan.request.context).ihost.list()
    for h in hosts:
        if h.hostname == host_name:
            ports = cgtsclient(pecan.request.context).port.list(h.uuid)
            for p in ports:
                if p.name == port_name:
                    return p
    return None


def make_display_id(iid, replace=False):
    if replace:
        instance_id = replace_uuids(iid)
    else:
        instance_id = replace_name_with_uuid(iid)

    return instance_id


def replace_name_with_uuid(instance_id):
    hName = None
    port = None
    for keyvalue in instance_id.split(ENTITY_SEP):
        try:
            (key, value) = keyvalue.split(KEY_VALUE_SEP, 1)
        except ValueError:
            return instance_id

        if key == 'host':
            hName = value

        elif key == 'port':
            if hName and not uuidutils.is_uuid_like(value.strip()):
                try:
                    port = _get_port(hName, value)
                except exceptions.NodeNotFound:
                    LOG.error("Can't find the host by name %s", hName)
                    pass
                except exceptions.ServerNotFound:
                    LOG.error("Can't find the port for name %s", value)
                    pass

                if port:
                    new_id = key + KEY_VALUE_SEP + port.uuid
                    instance_id = instance_id.replace(keyvalue, new_id, 1)

    return instance_id


def replace_uuid_with_name(key, value):
    new_id = None
    if key == 'port':
        port = None
        try:
            port = cgtsclient(pecan.request.context).port.get(value)
        except exceptions.ServerNotFound:
            LOG.error("Can't find the port for uuid %s", value)
            pass

        if port is not None:
            new_id = key + KEY_VALUE_SEP + port.name

    return new_id


def replace_uuids(instance_id):
    for keyvalue in instance_id.split(ENTITY_SEP):
        try:
            (key, value) = keyvalue.split(KEY_VALUE_SEP, 1)
        except ValueError:
            return instance_id

        if key in ALARM_ENTITY_TYPES_USE_UUID:
            if uuidutils.is_uuid_like(value.strip()):
                new_id = replace_uuid_with_name(key, value)
            else:
                new_id = key + KEY_VALUE_SEP + value

            if new_id is not None:
                instance_id = instance_id.replace(keyvalue, new_id, 1)

    return instance_id

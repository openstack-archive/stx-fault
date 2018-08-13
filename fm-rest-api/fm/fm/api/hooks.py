#
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#

import webob
from pecan import hooks
from oslo_config import cfg
from oslo_log import log
from oslo_serialization import jsonutils

from fm.common import context
from fm.db import api as dbapi
from fm.common.i18n import _

CONF = cfg.CONF

LOG = log.getLogger(__name__)


class ContextHook(hooks.PecanHook):
    """Configures a request context and attaches it to the request.

    The following HTTP request headers are used:

    X-User-Name:
        Used for context.user_name.

    X-User-Id:
        Used for context.user_id.

    X-Project-Name:
        Used for context.project.

    X-Project-Id:
        Used for context.project_id.

    X-Auth-Token:
        Used for context.auth_token.# Copyright (c) 2013-2014 Wind River Systems, Inc.

    X-Roles:
        Used for context.roles.
    """

    def before(self, state):
        headers = state.request.headers
        environ = state.request.environ
        user_name = headers.get('X-User-Name')
        user_id = headers.get('X-User-Id')
        project = headers.get('X-Project-Name')
        project_id = headers.get('X-Project-Id')
        domain_id = headers.get('X-User-Domain-Id')
        domain_name = headers.get('X-User-Domain-Name')
        auth_token = headers.get('X-Auth-Token')
        roles = headers.get('X-Roles', '').split(',')
        catalog_header = headers.get('X-Service-Catalog')
        service_catalog = None
        if catalog_header:
            try:
                service_catalog = jsonutils.loads(catalog_header)
            except ValueError:
                raise webob.exc.HTTPInternalServerError(
                    _('Invalid service catalog json.'))

        auth_token_info = environ.get('keystone.token_info')
        auth_url = CONF.keystone_authtoken.auth_uri

        state.request.context = context.make_context(
            auth_token=auth_token,
            auth_url=auth_url,
            auth_token_info=auth_token_info,
            user_name=user_name,
            user_id=user_id,
            project_name=project,
            project_id=project_id,
            domain_id=domain_id,
            domain_name=domain_name,
            roles=roles,
            service_catalog=service_catalog
        )


class DBHook(hooks.PecanHook):
    """Attach the dbapi object to the request so controllers can get to it."""

    def before(self, state):
        state.request.dbapi = dbapi.get_instance()

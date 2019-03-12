#
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#

import six
import webob.exc
from oslo_utils._i18n import _
from oslo_log import log as logging

LOG = logging.getLogger(__name__)


class ApiError(Exception):

    message = _("An unknown exception occurred.")

    # 500 - HTTPInternalServerError
    code = webob.exc.HTTPInternalServerError.code

    def __init__(self, message=None, **kwargs):

        self.kwargs = kwargs

        if 'code' not in self.kwargs and hasattr(self, 'code'):
            self.kwargs['code'] = self.code

        if message:
            self.message = message

        try:
            super(ApiError, self).__init__(self.message % kwargs)
            self.message = self.message % kwargs
        except Exception:
            LOG.exception('Exception in string format operation, '
                          'kwargs: %s', kwargs)
            raise

    def __str__(self):
        return repr(self.value)

    def __unicode__(self):
        return self.message

    def format_message(self):
        if self.__class__.__name__.endswith('_Remote'):
            return self.args[0]
        else:
            return six.text_type(self)


class NotFound(ApiError):
    message = _("Resource could not be found.")
    # 404 - HTTPNotFound
    code = webob.exc.HTTPNotFound.code


class HTTPNotFound(NotFound):
    pass


class AlarmNotFound(NotFound):
    message = _("Alarm %(alarm)s could not be found.")


class EventLogNotFound(NotFound):
    message = _("Event Log %(eventLog)s could not be found.")


class NodeNotFound(NotFound):
    message = _("Node %(node)s could not be found.")


class ServerNotFound(NotFound):
    message = _("Server %(server)s could not be found.")


class Invalid(ApiError):
    message = _("Unacceptable parameters.")
    # 400 - HTTPBadRequest
    code = webob.exc.HTTPBadRequest.code


class PatchError(Invalid):
    message = _("Couldn't apply patch '%(patch)s'. Reason: %(reason)s")


class ConfigInvalid(Invalid):
    message = _("Invalid configuration file. %(error_msg)s")


class InvalidParameterValue(Invalid):
    message = _("%(err)s")


class InvalidIdentity(Invalid):
    message = _("Expected an uuid or int but received %(identity)s.")


class PolicyNotAuthorized(ApiError):
    message = _("Policy doesn't allow %(action)s to be performed.")
    # 401 - HTTPUnauthorized
    code = webob.exc.HTTPUnauthorized.code


class Conflict(ApiError):
    message = _('HTTP Conflict.')
    # 409 - HTTPConflict
    code = webob.exc.HTTPConflict.code


class AlarmAlreadyExists(Conflict):
    message = _("An Alarm with UUID %(uuid)s already exists.")

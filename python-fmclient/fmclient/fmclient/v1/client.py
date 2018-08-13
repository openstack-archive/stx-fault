#
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#


from fmclient.common import http
from fmclient.common.http import DEFAULT_VERSION
from fmclient.common.i18n import _
from fmclient.common import exceptions as exc
from fmclient.v1 import alarm
from fmclient.v1 import event_log
from fmclient.v1 import event_suppression


class Client(object):
    """Client for the FM v1 API.

    :param string endpoint: A user-supplied endpoint URL for the ironic
                            service.
    :param function token: Provides token for authentication.
    :param integer timeout: Allows customization of the timeout for client
                            http requests. (optional)
    """

    def __init__(self, endpoint=None, session=None, **kwargs):
        """Initialize a new client for the FM v1 API."""
        if not session:
            if kwargs.get('os_fm_api_version'):
                kwargs['api_version_select_state'] = "user"
            else:
                if not endpoint:
                    raise exc.EndpointException(
                        _("Must provide 'endpoint' if os_fm_api_version "
                          "isn't specified"))

            # If the user didn't specify a version, use a default version
            kwargs['api_version_select_state'] = "default"
            kwargs['os_fm_api_version'] = DEFAULT_VERSION

        self.http_client = http.get_http_client(endpoint, session, **kwargs)
        self.alarm = alarm.AlarmManager(self.http_client)
        self.event_log = event_log.EventLogManager(self.http_client)
        self.event_suppression = event_suppression.EventSuppressionManager(
            self.http_client)

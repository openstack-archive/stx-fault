#
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#


import pecan
import wsmeext.pecan as wsme_pecan
from pecan import rest
from wsme import types as wtypes

from fm.api.controllers.v1 import link
from fm.api.controllers.v1 import alarm
from fm.api.controllers.v1 import base
from fm.api.controllers.v1 import event_log
from fm.api.controllers.v1 import event_suppression


class MediaType(base.APIBase):
    """A media type representation."""

    base = wtypes.text
    type = wtypes.text

    def __init__(self, base, type):
        self.base = base
        self.type = type


class V1(base.APIBase):
    """The representation of the version 1 of the API."""

    id = wtypes.text
    "The ID of the version, also acts as the release number"

    media_types = [MediaType]
    "An array of supported media types for this version"

    links = [link.Link]
    "Links that point to a specific URL for this version and documentation"

    alarms = [link.Link]
    "Links to the alarm resource"

    event_log = [link.Link]
    "Links to the event_log resource"

    event_suppression = [link.Link]
    "Links to the event_suppression resource"

    @classmethod
    def convert(self):
        v1 = V1()
        v1.id = "v1"
        v1.links = [link.Link.make_link('self', pecan.request.host_url,
                                        'v1', '', bookmark=True),
                    link.Link.make_link('describedby',
                                        'http://www.windriver.com',
                                        'developer/fm/dev',
                                        'api-spec-v1.html',
                                        bookmark=True, type='text/html')
                    ]
        v1.media_types = [MediaType('application/json',
                          'application/vnd.openstack.fm.v1+json')]

        v1.alarms = [link.Link.make_link('self', pecan.request.host_url,
                                         'alarms', ''),
                     link.Link.make_link('bookmark',
                                         pecan.request.host_url,
                                         'alarms', '',
                                         bookmark=True)
                      ]

        v1.event_log = [link.Link.make_link('self', pecan.request.host_url,
                                            'event_log', ''),
                        link.Link.make_link('bookmark',
                                            pecan.request.host_url,
                                            'event_log', '',
                                            bookmark=True)
                        ]

        v1.event_suppression = [link.Link.make_link('self',
                                                    pecan.request.host_url,
                                                    'event_suppression', ''),
                                link.Link.make_link('bookmark',
                                                    pecan.request.host_url,
                                                    'event_suppression', '',
                                                    bookmark=True)
                                ]

        return v1


class Controller(rest.RestController):
    """Version 1 API controller root."""

    alarms = alarm.AlarmController()
    event_log = event_log.EventLogController()
    event_suppression = event_suppression.EventSuppressionController()

    @wsme_pecan.wsexpose(V1)
    def get(self):
        return V1.convert()


__all__ = ('Controller',)

#
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#

from fmclient.common import options
from fmclient.common import base


class Alarm(base.Resource):
    def __repr__(self):
        return "<Alarm %s>" % self._info


class AlarmManager(base.Manager):
    resource_class = Alarm

    @staticmethod
    def _path(id=None):
        return '/v1/alarms/%s' % id if id else '/v1/alarms'

    def list(self, q=None, limit=None, marker=None, sort_key=None,
             sort_dir=None, include_suppress=False):
        params = []

        if include_suppress:
            params.append('include_suppress=True')
        if limit:
            params.append('limit=%s' % str(limit))
        if marker:
            params.append('marker=%s' % str(marker))
        if sort_key:
            params.append('sort_key=%s' % str(sort_key))
        if sort_dir:
            params.append('sort_dir=%s' % str(sort_dir))

        return self._list(options.build_url(self._path(), q, params), 'alarms')

    def get(self, iid):
        try:
            return self._list(self._path(iid))[0]
        except IndexError:
            return None

    def delete(self, uuid):
        return self._delete(self._path(uuid))

    def summary(self, include_suppress=False):
        params = []
        if include_suppress:
            params.append('include_suppress=True')
        return self._list(options.build_url(self._path('summary'), None, params))

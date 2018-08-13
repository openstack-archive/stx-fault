#
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#


import sys

import eventlet
from oslo_config import cfg
from oslo_log import log as logging
from oslo_service import systemd
from oslo_service import wsgi

import logging as std_logging

from fm.common.i18n import _
from fm.api import app
from fm.api import config

api_opts = [
    cfg.StrOpt('bind_host',
               default="0.0.0.0",
               help=_('IP address for fm api to listen')),
    cfg.IntOpt('bind_port',
               default=18002,
               help=_('listen port for fm api')),
    cfg.IntOpt('api_workers', default=2,
               help=_("number of api workers")),
    cfg.IntOpt('limit_max',
               default=2000,
               help='the maximum number of items returned in a single '
                    'response from a collection resource')
]


CONF = cfg.CONF


LOG = logging.getLogger(__name__)
eventlet.monkey_patch(os=False)


def main():

    config.init(sys.argv[1:])
    config.setup_logging()

    application = app.load_paste_app()

    CONF.register_opts(api_opts, 'api')

    host = CONF.api.bind_host
    port = CONF.api.bind_port
    workers = CONF.api.api_workers

    if workers < 1:
        LOG.warning("Wrong worker number, worker = %(workers)s", workers)
        workers = 1

    LOG.info("Server on http://%(host)s:%(port)s with %(workers)s",
             {'host': host, 'port': port, 'workers': workers})
    systemd.notify_once()
    service = wsgi.Server(CONF, CONF.prog, application, host, port)

    app.serve(service, CONF, workers)

    LOG.debug("Configuration:")
    CONF.log_opt_values(LOG, std_logging.DEBUG)

    app.wait()


if __name__ == '__main__':
    main()

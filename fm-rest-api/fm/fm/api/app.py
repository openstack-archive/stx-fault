#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#


from oslo_service import service
from oslo_service import wsgi
from oslo_config import cfg
from oslo_log import log
import pecan

from fm.api import config
from fm.common import policy
from fm.common.i18n import _

CONF = cfg.CONF

LOG = log.getLogger(__name__)

_launcher = None


def get_pecan_config():
    # Set up the pecan configuration
    filename = config.__file__.replace('.pyc', '.py')
    return pecan.configuration.conf_from_file(filename)


def setup_app(config=None):
    policy.init()

    if not config:
        config = get_pecan_config()

    pecan.configuration.set_config(dict(config), overwrite=True)
    app_conf = dict(config.app)

    app = pecan.make_app(
        app_conf.pop('root'),
        debug=CONF.debug,
        logging=getattr(config, 'logging', {}),
        force_canonical=getattr(config.app, 'force_canonical', True),
        guess_content_type_from_ext=False,
        **app_conf
    )
    return app


def load_paste_app(app_name=None):
    """Loads a WSGI app from a paste config file."""
    if app_name is None:
        app_name = cfg.CONF.prog

    loader = wsgi.Loader(cfg.CONF)
    app = loader.load_app(app_name)
    return app


def app_factory(global_config, **local_conf):
    return setup_app()


def serve(api_service, conf, workers=1):
    global _launcher
    if _launcher:
        raise RuntimeError(_('serve() can only be called once'))

    _launcher = service.launch(conf, api_service, workers=workers)


def wait():
    _launcher.wait()

#
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#


try:
    import fmclient.client
    Client = fmclient.client.get_client
except ImportError:
    import warnings
    warnings.warn("Could not import fmclient.client", ImportWarning)

import pbr.version

version_info = pbr.version.VersionInfo('fmclient')

try:
    __version__ = version_info.version_string()
except AttributeError:
    __version__ = None

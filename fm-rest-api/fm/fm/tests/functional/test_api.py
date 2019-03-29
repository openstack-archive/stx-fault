# Copyright 2019 Intel Corporation
#
# SPDX-License-Identifier: Apache-2.0

"""REST API test loader"""

import os

from gabbi import driver
import openstack

# Location of the YAML files that define gabbi tests
TESTS_DIR = 'gabbits'

# The environment variable with the bare test endpoint
# TODO(dtroyer): Teach this how to get the endpoint from the service catalog
TEST_URL = "OS_TEST_URL"


def _get_endpoint():
    """Get the service URL from the service catalog"""
    conn = openstack.connect()
    ep = conn.config.get_session_endpoint('faultmanagement')
    if (ep.endswith("/v1")):
        ep = ep.replace("/v1", "")
    return ep

def load_tests(loader, tests, pattern):
    """Provide a TestSuite to the discovery process."""
    test_dir = os.path.join(os.path.dirname(__file__), TESTS_DIR)
    test_url = os.environ.get(TEST_URL, None)
    if test_url is None:
        test_url = _get_endpoint()

    # Pass "require_ssl=True" as an argument to force all tests
    # to use SSL in requests.
    return driver.build_tests(
        test_dir,
        loader,
        url=test_url,
        # intercept=wsgiapp.app,
        # fixture_module=fixtures,
    )

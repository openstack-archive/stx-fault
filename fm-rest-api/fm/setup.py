#!/usr/bin/env python
# Copyright (c) 2013 Hewlett-Packard Development Company, L.P.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# THIS FILE IS MANAGED BY THE GLOBAL REQUIREMENTS REPO - DO NOT EDIT

# In python < 2.7.4, a lazy loading of package `pbr` will break
# setuptools if some other modules registered functions in `atexit`.
# solution from: http://bugs.python.org/issue15881#msg170215

from setuptools import setup, find_packages

setup(
    name='fm',
    description='Titanium Cloud Fault Management',
    version='1.0.0',
    license='Apache-2.0',
    platforms=['any'],
    provides=['fm'],
    packages=find_packages(),
    package_data={},
    include_package_data=False,
    entry_points={
        'fm.database.migration_backend': [
            'sqlalchemy = oslo_db.sqlalchemy.migration',
        ],
        'console_scripts': [
            'fm-dbsync = fm.cmd.dbsync:main',
            'fm-api = fm.cmd.api:main'
        ],
    }
)

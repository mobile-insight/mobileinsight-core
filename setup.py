# -*- coding: utf-8 -*-
import os
import sys
from distutils.core import setup

# a hack to include data files in pure library path
from distutils.command.install import INSTALL_SCHEMES
for scheme in INSTALL_SCHEMES.values():
    scheme['data'] = scheme['purelib']

setup(
    name = 'MobileInsight',
    version = '1.0',
    description = 'Mobile network monitoring and analysis',
    author = 'Yuanjie Li, Jiayao Li',
    url = 'http://metro.cs.ucla.edu/mobile_insight',
    packages = ['mobile_insight',
                'mobile_insight.analyzer',
                'mobile_insight.monitor',
                'mobile_insight.monitor.dm_collector',
                'mobile_insight.monitor.dm_collector.dm_endec',
                ],
    data_files=[('ws_dissector', ['ws_dissector/ws_dissector'])],
)

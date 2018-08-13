#
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#


from fmclient.common import utils

from fmclient.v1 import alarm_shell
from fmclient.v1 import event_log_shell
from fmclient.v1 import event_suppression_shell


COMMAND_MODULES = [
    alarm_shell,
    event_log_shell,
    event_suppression_shell,
]


def enhance_parser(parser, subparsers, cmd_mapper):
    '''Take a basic (nonversioned) parser and enhance it with
    commands and options specific for this version of API.

    :param parser: top level parser :param subparsers: top level
        parser's subparsers collection where subcommands will go
    '''
    for command_module in COMMAND_MODULES:
        utils.define_commands_from_module(subparsers, command_module,
                                          cmd_mapper)

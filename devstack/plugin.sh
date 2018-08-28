#!/bin/bash

# devstack/plugin.sh
# Triggers stx_fault specific functions to install and configure Fault Management

# Dependencies:
#
# - ``functions`` file
# - ``DATA_DIR`` must be defined

# ``stack.sh`` calls the entry points in this order:
#
echo_summary "fault devstack plugin.sh called: $1/$2"
source $DEST/stx-fault/devstack/lib/stx-fault

# check for service enabled
if [[ ,${ENABLED_SERVICES} =~ ,"fm-" ]]; then
    if [[ "$1" == "stack" && "$2" == "install" ]]; then
        # Perform installation of source
        echo_summary "Installing fault service"
        install_fault

    elif [[ "$1" == "stack" && "$2" == "post-config" ]]; then
        # Configure after the other layer 1 and 2 services have been configured
        echo_summary "Configuring fault"
        configure_fault
        create_fault_user_group
        create_fault_accounts
    elif [[ "$1" == "stack" && "$2" == "extra" ]]; then
        # Initialize and start the service
        echo_summary "Initializing and start fault"
        init_fault
        start_fault
    elif [[ "$1" == "stack" && "$2" == "test-config" ]]; then
        # do sanity test
        echo_summary "do test-config"
    fi

    if [[ "$1" == "unstack" ]]; then
        # Shut down services
        echo_summary "Stop fault service"
        stop_fault
        :
    fi

    if [[ "$1" == "clean" ]]; then
        cleanup_fault
        :
    fi
fi

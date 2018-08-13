//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>

#include <fmAPI.h>
#include <fmConfig.h>
#include <fmLog.h>

void sig_handler(int signo) {
	if (signo == SIGHUP){
		fm_get_config_paramters();
		fmLoggingInit();
        }
}

int main(int argc, char *argv[]) {
	int c;
	const char *fn = NULL;

	if (argc < 3) {
		syslog(LOG_ERR, "Wrong arguments\n");
		exit(-1);
	}

	while ((c=getopt(argc,argv,"c:")) != -1) {
			switch(c) {
			case 'c':
				fn = optarg;
				break;
			default:
				syslog(LOG_ERR, "Invalid option...\n");
				exit(-1);
			}
	}

        /* ignore SIGPIPE */
        signal(SIGPIPE, SIG_IGN);

	if (signal(SIGHUP, sig_handler) ==  SIG_ERR){
		syslog(LOG_INFO, "Can't catch SIGHUP\n");
	}

	fm_server_create(fn);
	exit(0);
}

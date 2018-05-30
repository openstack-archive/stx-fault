//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include <stdio.h>
#include <stdlib.h>

#include <fmAPI.h>

#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>

void sig_handler(int signo) {
	int result = 0;
	if (signo == SIGHUP){
		result = setlogmask(LOG_UPTO (LOG_DEBUG));
		if (result == LOG_UPTO (LOG_DEBUG)){
			result = setlogmask(LOG_UPTO (LOG_INFO));
			syslog(LOG_INFO, "Received SIGHUP, set log level from %d to LOG_INFO", result);
		}else{
			syslog(LOG_INFO, "Received SIGHUP, set log level from %d to LOG_DEBUG", result);
		}
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

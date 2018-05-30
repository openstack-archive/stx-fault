//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include "fmTime.h"
#include "fmLog.h"
#include <string.h>
#include <errno.h>

#define FM_USEC_PER_SEC 1000000


static FMTimeT GetUptimeMicro() {
	uint64_t thetm = 0;
	struct timespec tv;

	if (clock_gettime(CLOCK_MONOTONIC_RAW,&tv)==0) {
		thetm = tv.tv_nsec/1000;
		thetm =  ((uint64_t)tv.tv_sec) * FM_USEC_PER_SEC;
	} else {
		FM_ERROR_LOG("clock_gettime() failed: error: (%d), (%s)", errno, strerror(errno));
	}
	return thetm;
}

FMTimeT fmGetCurrentTime() {

	struct timeval tv;

	memset(&tv,0,sizeof(tv));
	if (gettimeofday(&tv, NULL) != 0){
	    FM_ERROR_LOG("gettimeofday failed (errno:%d)",errno);
	    return 0;
	} else {
	    return (((FMTimeT)tv.tv_sec) * FM_USEC_PER_SEC) +
	            ((FMTimeT)tv.tv_usec);
	}
}

FMTimeT fmGetCurrentHrt() {
	return GetUptimeMicro();
}

FMTimeT CFmTimer::gethrtime(){
	return fmGetCurrentHrt();
}

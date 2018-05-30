//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef FM_THREAD_H_
#define FM_THREAD_H_

#include <stdbool.h>
//Set stack size of 2M (rather than 8M default).
#define FM_THREAD_STACK_SIZE (2 * 1024 * 1024)

#ifdef __cplusplus
extern "C" {
#endif
typedef void (*FmThreadFunctionT) (void * context);
bool fmCreateThread(FmThreadFunctionT fn, void *cntx) ;
bool fmThreadSleep(unsigned int msec) ;
void fmJobHandlerThread(void *context);
void fmRegHandlerThread(void *context);
void fmEventSuppressionMonitorThread(void *context);

#ifdef __cplusplus
}
#endif

#endif







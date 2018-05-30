//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include <pthread.h>
#include <stdio.h>
#include "fmLog.h"
#include "fmMutex.h"


bool CFmMutex::lock() {

  //FM_DEBUG_LOG("lock %X",cntx);
  return pthread_mutex_lock((pthread_mutex_t*)cntx)==0;

}

bool CFmMutex::unlock() {

  //FM_DEBUG_LOG("unlock %X",cntx);
  return pthread_mutex_unlock((pthread_mutex_t*)cntx)==0;

}

CFmMutex::CFmMutex() {

  cntx = NULL;

  //use recursive Mutex to allow one thread to lock it multiple times
  pthread_mutex_t tmutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
  pthread_mutex_t * pMutex = new pthread_mutex_t;
  if (pMutex!=NULL) {
    *pMutex = tmutex;
    cntx = pMutex;
  }
  
}

CFmMutex::~CFmMutex() {

  pthread_mutex_destroy((pthread_mutex_t*)cntx);
  delete ((pthread_mutex_t*)cntx);  //safe if cntx is null
  cntx = NULL;

}


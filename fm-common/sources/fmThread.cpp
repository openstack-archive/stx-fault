//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "fmThread.h"
#include "fmLog.h"


struct thread_struct {
  FmThreadFunctionT fn;
  void *cntx;
};

static void * ThreadProcL(void * lpParameter ) {

  thread_struct * p = (thread_struct *) lpParameter;
  
  p->fn(p->cntx);
  delete p;
  return 0;
  
}
#define FM_MSEC_PER_SEC 1000
#define FM_NSEC_PER_MILSEC 1000000
static void our_sleep(unsigned int msec) {

  struct timespec req;
  struct timespec left;
  
  memset(&left,0,sizeof(left));
  req.tv_sec = msec / FM_MSEC_PER_SEC;
  req.tv_nsec = (msec % FM_MSEC_PER_SEC) * FM_NSEC_PER_MILSEC;
  
  while (nanosleep(&req,&left)==-1 && errno==EINTR) {
    req = left;
  }
  
}


extern "C" {
bool fmCreateThread(FmThreadFunctionT fn, void *usrData)  {

  int rt = 0;
  pthread_t p_thread;
  pthread_attr_t  attr;

  //Initialize the thread attributes with the default values
  rt =pthread_attr_init(&attr);
  
  if (rt != 0){
	FM_ERROR_LOG("Initialize the thread attributes failed:error=%d",rt);
    return false;
  }

  // could change the default values here
  rt =pthread_attr_setstacksize(&attr, FM_THREAD_STACK_SIZE);
  if (rt != 0){
    FM_ERROR_LOG("pthread_attr_setstacksize failed:error=%d",rt);
    return false;
  }

  thread_struct * p = new thread_struct;
  if (p == NULL){
    FM_ERROR_LOG("Can't allocate the thread struct");
    return false;
  
  }
  
  p->fn = fn;
  p->cntx = usrData;
  rt= pthread_create(&p_thread,&attr,ThreadProcL,p);
  
  if (rt != 0){
    FM_ERROR_LOG("Failed to create the thread:error=%d",rt);
    delete p;
    return false;
  }

  return true;

}

bool fmThreadSleep(unsigned int msec)  {
  our_sleep(msec);
  return true;
}

}

//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef FM_TIME_H_
#define FM_TIME_H_


#include <time.h>
#include <sys/time.h>

#include <errno.h>
#include <stdio.h>
#include <stdint.h>


#include "fmAPI.h"
#include "fmLog.h"


// time in microseconds
FMTimeT fmGetCurrentTime();

// time in microseconds
FMTimeT fmGetCurrentHrt();

/**
 * Create a passive timer in microseconds
 */
class CFmTimer {
	FMTimeT m_start;
	FMTimeT m_offset;
  
  static bool isExpired(FMTimeT start, FMTimeT offset,
		  FMTimeT now) {
    return (now-start) >= offset;
  }
  
public:

  static FMTimeT gethrtime();
  
  unsigned long long getStartingTime() const { return m_start; }
  
  void reset() { 
    m_start = gethrtime(); 
  }
  
  void setExpired() { 
    m_start = gethrtime() - m_offset;
  }
  
  void setExpiry(FMTimeT timeout) {
    m_offset = timeout;
    reset();
  }
  
  CFmTimer(FMTimeT timeout=0) {
    setExpiry(timeout);
  }

  bool isExpired() const {
    return isExpired(m_start,m_offset,gethrtime());
  }
  
  FMTimeT remaining() const {
	  FMTimeT now = gethrtime();
    if (isExpired(m_start,m_offset,now)) {
      return 0;
    }
    return m_offset - (now-m_start);
  }
  FMTimeT sec_to_micro(unsigned int t) {
	  return ((FMTimeT)(t)) * 10000000;
  }
};

#endif


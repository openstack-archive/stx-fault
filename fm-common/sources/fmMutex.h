//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef FM_MUTEX_H_
#define FM_MUTEX_H_


class CFmMutex {

  void * cntx;
    
public:
  CFmMutex();
  ~CFmMutex();
  bool lock();
  bool unlock();
  
};

class CFmMutexGuard {

  CFmMutex & m;
  bool rc;
  
public:
  CFmMutexGuard(CFmMutex & mu) : m(mu) {
    rc = m.lock();
  }
  
  ~CFmMutexGuard() {
    m.unlock();
  }
  
  bool getRc() { 
    return rc; 
  }
};


#endif



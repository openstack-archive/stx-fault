//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef fmFILE_H_
#define fmFILE_H_

#include <stdio.h>
#include <string>
#include <vector>

class CfmFile {
  enum {MAX_LINE_LEN = 4096 };
  FILE * fp;  
public:
  enum eFileAccess {READ, READ_WRITE, WRITE, APPEND};
  
  CfmFile() : fp(NULL) {}
  
  virtual ~CfmFile() { if (fp!=NULL) fclose(fp); }
  
  bool valid() { return fp!=NULL; }
  
  bool open(const char *fn,eFileAccess a, bool binary=false);
  
  void close();

  virtual bool read_line(std::string &line);

};



#endif 

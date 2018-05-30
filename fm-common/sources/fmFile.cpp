//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include <unistd.h>
#include <algorithm>
#include <string.h>
#include "fmFile.h"
#include "fmLog.h"

#define FM_FILE_MODE_MAX 10
#define FM_FILE_SIZE_MAX 2056

bool CfmFile::open(const char *path,eFileAccess a, bool binary) {

    char attr[FM_FILE_MODE_MAX];
    
    switch(a) {
      case READ: strcpy(attr,"r"); 
      	break;
      case READ_WRITE: strcpy(attr,"r+"); 
      	break;
      case WRITE: strcpy(attr,"w"); 
      	break;
      case APPEND: strcpy(attr,"a"); 
      	break;
    }
    if (binary) {
    	strcat(attr,"b");
    }
    
    fp = fopen(path,attr);
    
    return fp!=NULL;
}

int is_return_char(int c) {
	return (((char)c)=='\n' || ((char)c)=='\r') ? 1 : 0;
}


bool CfmFile::read_line(std::string &s) {
	char st[MAX_LINE_LEN];

	bool b = fgets(st,sizeof(st)-1,fp)!=NULL;
	if (b) {
		s = st;
		s.erase(std::find_if(s.begin(), s.end(), std::ptr_fun<int, int>(is_return_char)),s.end());
	} else s = "";
	return b;
}

void CfmFile::close(){
	fclose(fp);
}

//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef _FM_MSG_H
#define _FM_MSG_H

#include <vector>
#include "fmAPI.h"
#include <stdint.h>

#define FM_MSG_VAL_KEY   (0x1a0ff11d)
#define FM_MSG_WAIT_FAIL (5)
#define FM_MSG_MAX_RET   (2)
#define FM_MSG_INF_WAIT  (0)

typedef enum {
  EFmMsgV1= 1,
  EFmMaxVersion
}EFmMsgVersionT;

typedef enum {
  EFmMsgRx=0,
  EFmMsgTx=1,
  EFmMsgMax
}EFmMsgTypesT;

typedef enum {
  EFmCreateFault = 0,
  EFmUpdateFault,
  EFmDeleteFault,
  EFmDeleteFaults,
  EFmGetFault,
  EFmGetFaults,
  EFmReturnUUID,
  EFmGetFaultsById,
  EFmActMax
}EFmMsgActionsT;


typedef struct {
  EFmMsgVersionT version;
  EFmMsgActionsT action;
  uint32_t msg_size;
  uint32_t msg_rc;	//filled in by server
} SFmMsgHdrT;

typedef std::vector<char> fm_buff_t;


EFmErrorT fm_msg_utils_prep_requet_msg(fm_buff_t &buff,
		EFmMsgActionsT act, const void * data, uint32_t len) ;

static inline void * ptr_to_data(fm_buff_t &buff) {
	return &(buff[sizeof(SFmMsgHdrT)]);
}

static inline SFmMsgHdrT * ptr_to_hdr(fm_buff_t &buff) {
	return (SFmMsgHdrT *)&(buff[0]);
}

static inline bool fm_valid_srv_msg(SFmMsgHdrT *msg, uint32_t exp_size) {
	return (msg->msg_size==exp_size);
}


#endif /* _FM_MSG_H */

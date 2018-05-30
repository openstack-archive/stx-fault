//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef FMMSGSERVER_H_
#define FMMSGSERVER_H_

#include <vector>
#include "fmSocket.h"
#include "fmMsg.h"
#include "fmDb.h"

class FmSocketServerProcessor : public FmSocketServer {

protected:
	virtual void handle_socket_data(int fd,std::vector<char> &data,
			CFmDBSession &sess);

	virtual void handle_create_fault(int fd, SFmMsgHdrT *hdr,
			std::vector<char> &rdata, CFmDBSession &sess);
	virtual void handle_delete_fault(int fd, SFmMsgHdrT *hdr,
			std::vector<char> &rdata, CFmDBSession &sess);
	virtual void handle_delete_faults(int fd, SFmMsgHdrT *hdr,
			std::vector<char> &rdata,CFmDBSession &sess);

	virtual void handle_get_fault(int fd, SFmMsgHdrT *hdr,
			std::vector<char> &rdata);
	virtual void handle_get_faults(int fd, SFmMsgHdrT *hdr,
			std::vector<char> &rdata);
	virtual void handle_get_faults_by_id(int fd, SFmMsgHdrT *hdr,
			std::vector<char> &rdata);
public:
	void send_response(int fd, SFmMsgHdrT *hdr, void *data, size_t len);
};

#endif /* FMMSGSERVER_H_ */

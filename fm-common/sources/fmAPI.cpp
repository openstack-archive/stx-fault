//
// Copyright (c) 2017 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <list>
#include <new>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#include "fmAPI.h"
#include "fmMsg.h"
#include "fmLog.h"
#include "fmSocket.h"
#include "fmMutex.h"
#include "fmThread.h"
#include "fmAlarmUtils.h"


#define FM_MGR_HOST_NAME "controller"
#define MAX_PENDING_REQUEST 1000

#define HANDLE_SERVER_RC(hdr) \
	if ((hdr)->msg_rc!=FM_ERR_OK) return (EFmErrorT) (hdr)->msg_rc

#define CHECK_RESPONSE(hdr,neededstruct) \
	if (!fm_valid_srv_msg(hdr,sizeof(neededstruct))) \
		return FM_ERR_COMMUNICATIONS

#define CHECK_LIST_FULL(l) \
	if (l.size() == MAX_PENDING_REQUEST) \
		return FM_ERR_NOT_ENOUGH_SPACE

#define CHECK_LIST_NOT_EMPTY(l) \
	if (l.size() != 0) \
		return FM_ERR_REQUEST_PENDING

static CFmSocket m_client;
static bool m_connected = false;
static bool m_thread = false;

typedef std::list<fm_buff_t>  FmRequestListT;
static FmRequestListT & GetListOfFmRequests(){
	static FmRequestListT reqs;
	return reqs;
}

static CFmMutex & getListMutex(){
	static CFmMutex *m = new CFmMutex;
	return *m;
}

static CFmMutex & getThreadMutex(){
	static CFmMutex *m = new CFmMutex;
	return *m;
}

CFmMutex & getAPIMutex(){
	static pthread_mutex_t ml = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
	static CFmMutex *m = NULL;
	if (m == NULL){
		pthread_mutex_lock(&ml);
		m = new CFmMutex;
		pthread_mutex_unlock(&ml);
	}
	return *m;
}

static void enqueue(fm_buff_t &req){
	CFmMutexGuard m(getListMutex());
	GetListOfFmRequests().push_back(req);
}

static bool dequeue(fm_buff_t &req){
	CFmMutexGuard m(getListMutex());
	if (GetListOfFmRequests().size() == 0){
		return false;
	}
	FmRequestListT::iterator it = GetListOfFmRequests().begin();
	req.clear();
	req = (*it);
	GetListOfFmRequests().pop_front();
	return true;
}

static bool fm_lib_reconnect() {
  char addr[INET6_ADDRSTRLEN];

  while (!m_connected) {
	struct addrinfo hints;
	struct addrinfo *result=NULL, *rp;
	memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = 0;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    int rc = getaddrinfo(FM_MGR_HOST_NAME,NULL,
                      &hints,
                      &result);
    if (rc!=0) {
    	FM_ERROR_LOG("controller lookup failed... errno:%d",errno);
    	break;
    } else {
		for (rp = result; rp != NULL; rp = rp->ai_next) {
			if (rp->ai_family==AF_INET||rp->ai_family==AF_INET6) {
                            if(rp->ai_family==AF_INET) {
                                inet_ntop(AF_INET, &(((sockaddr_in*)rp->ai_addr)->sin_addr), addr, sizeof(addr));
                            } else if (rp->ai_family==AF_INET6) {
                                inet_ntop(AF_INET6, &(((sockaddr_in6*)rp->ai_addr)->sin6_addr), addr, sizeof(addr));
                            }
				m_connected=m_client.connect(addr,8001,rp->ai_family);
				if (m_connected==true) {
					FM_INFO_LOG("Connected to FM Manager.");
					break;
				} else {
					FM_WARNING_LOG("Failed to connect to FM Manager.");
				}
			}
		 }
		freeaddrinfo(result);
    }
    break;
  }

  return (m_connected);
}

EFmErrorT fm_msg_utils_prep_requet_msg(fm_buff_t &buff,
		                               EFmMsgActionsT act,
		                               const void * data,
		                               uint32_t len) {

	try {
		buff.resize(sizeof(SFmMsgHdrT) + len);
	} catch (...) {
		FM_ERROR_LOG("Buff resize failed: errno:%d",errno);
		return FM_ERR_NOMEM;
	}
	SFmMsgHdrT *hdr = ptr_to_hdr(buff);
	hdr->action = act;
	hdr->msg_size = len;
	hdr->version = EFmMsgV1;
	hdr->msg_rc = 0;
	memcpy(ptr_to_data(buff),data,len);
	return FM_ERR_OK;
}

static void fmApiJobHandlerThread(void *context){

	while (true){
		fm_buff_t buff;
		buff.clear();
		while (dequeue(buff)){
			while (true) {
				while (!fm_lib_reconnect()){
					fmThreadSleep(200);
				}
				fm_log_request(buff);
				// protect from other sync APIs to access the same socket
				CFmMutexGuard m(getAPIMutex());
				if(m_client.write_packet(buff)) {
					fm_buff_t in_buff;
					in_buff.clear();
					if(!m_client.read_packet(in_buff)) {
						// retry after read failure
						fm_log_response(buff, in_buff, true);
						m_connected = false;
						continue;
					}
					else {
						fm_log_response(buff, in_buff);
						break;
					}
				}else{
					//retry after write failure
					fm_log_request(buff, true);
					m_connected = false;
					continue;
				}
			}
		}
		fmThreadSleep(50);
	}
}

static bool fm_lib_thread(){
	CFmMutexGuard m(getThreadMutex());
	if (!m_thread){
		FM_INFO_LOG("Creating thread");
		if (!fmCreateThread(fmApiJobHandlerThread,NULL)) {
			FM_ERROR_LOG("Fail to create API job thread");
		}
		else {
			m_thread = true;
		}
	}
	return m_thread;
}

static EFmErrorT fm_check_thread_pending_request(){
	CFmMutexGuard m(getThreadMutex());
	if (m_thread){
		CHECK_LIST_NOT_EMPTY(GetListOfFmRequests());
	}
	return FM_ERR_OK;
}

extern "C" {

EFmErrorT fm_init_lib() {
	signal(SIGINT,SIG_IGN);

	CFmMutexGuard m(getAPIMutex());
	if (!fm_lib_reconnect()) {
		FM_ERROR_LOG("Socket connection failed\n");
		return FM_ERR_NOCONNECT;
	}
	return FM_ERR_OK;
}

EFmErrorT fm_set_fault(const SFmAlarmDataT *alarm,
		                  fm_uuid_t *uuid){

	CFmMutexGuard m(getAPIMutex());
	if (!fm_lib_reconnect()) return FM_ERR_NOCONNECT;

	fm_buff_t buff;
	buff.clear();
	EFmErrorT erc = fm_msg_utils_prep_requet_msg(buff,EFmCreateFault,
			alarm,sizeof(*alarm));
	if (erc!=FM_ERR_OK) return erc;

	if(m_client.write_packet(buff)) {
		if(!m_client.read_packet(buff)) {
			m_connected = false;
			return FM_ERR_NOCONNECT;
		}

		HANDLE_SERVER_RC(ptr_to_hdr(buff));
		CHECK_RESPONSE(ptr_to_hdr(buff),fm_uuid_t);
		if (uuid != NULL)
			memcpy(*uuid,ptr_to_data(buff),sizeof(*uuid)-1);
	} else {
		m_connected = false;
		return FM_ERR_NOCONNECT;
	}

	return FM_ERR_OK;
}

EFmErrorT fm_clear_fault(AlarmFilter *filter){

	CFmMutexGuard m(getAPIMutex());
	if (!fm_lib_reconnect()) return FM_ERR_NOCONNECT;

	fm_buff_t buff;
	buff.clear();
	EFmErrorT erc = fm_msg_utils_prep_requet_msg(buff,EFmDeleteFault,
			                                     filter,sizeof(*filter));
	if (erc!=FM_ERR_OK) return erc;

	if(m_client.write_packet(buff)) {
		if(!m_client.read_packet(buff)) {
            m_connected = false;
			return FM_ERR_NOCONNECT;
		}
		HANDLE_SERVER_RC(ptr_to_hdr(buff));
	} else {
		m_connected = false;
		return FM_ERR_NOCONNECT;
	}
	return FM_ERR_OK;
}

EFmErrorT fm_clear_all(fm_ent_inst_t *inst_id){

	CFmMutexGuard m(getAPIMutex());
	if (!fm_lib_reconnect()) return FM_ERR_NOCONNECT;

	fm_buff_t buff;
	buff.clear();
	EFmErrorT erc = fm_msg_utils_prep_requet_msg(buff,EFmDeleteFaults,
			        (*inst_id), sizeof(*inst_id));
	if (erc!=FM_ERR_OK) return erc;

	if(m_client.write_packet(buff)) {
		if(!m_client.read_packet(buff)) {
			m_connected = false;
			FM_ERROR_LOG("Read ERR: return FM_ERR_NOCONNECT");
			return FM_ERR_NOCONNECT;
		}
		HANDLE_SERVER_RC(ptr_to_hdr(buff));
	} else {
		m_connected = false;
		FM_ERROR_LOG("Write ERR: return FM_ERR_NOCONNECT");
		return FM_ERR_NOCONNECT;
	}
	return FM_ERR_OK;
}


EFmErrorT fm_get_fault(AlarmFilter *filter, SFmAlarmDataT *alarm ){

	CFmMutexGuard m(getAPIMutex());
	if (!fm_lib_reconnect()) return FM_ERR_NOCONNECT;
	fm_check_thread_pending_request();

	fm_buff_t buff;
	buff.clear();
	EFmErrorT erc = fm_msg_utils_prep_requet_msg(buff,EFmGetFault,
			        filter,sizeof(*filter));
	if (erc!=FM_ERR_OK) return erc;

	if(m_client.write_packet(buff)) {
		if(!m_client.read_packet(buff)) {
			m_connected = false;
			return FM_ERR_NOCONNECT;
		}
		HANDLE_SERVER_RC(ptr_to_hdr(buff));
		CHECK_RESPONSE(ptr_to_hdr(buff),SFmAlarmDataT);
		SFmAlarmDataT * data = (SFmAlarmDataT * ) ptr_to_data(buff);
		*alarm = *data;
	} else {
		m_connected = false;
		return FM_ERR_NOCONNECT;
	}

	return FM_ERR_OK;
}

EFmErrorT fm_get_faults(fm_ent_inst_t *inst_id,
		 SFmAlarmDataT *alarm,  unsigned int *max_alarms_to_get) {

	CFmMutexGuard m(getAPIMutex());
	if (!fm_lib_reconnect()) return FM_ERR_NOCONNECT;
	fm_check_thread_pending_request();

	fm_buff_t buff;
	buff.clear();
	EFmErrorT erc = fm_msg_utils_prep_requet_msg(buff,EFmGetFaults,
			        (*inst_id),sizeof(*inst_id));
	if (erc!=FM_ERR_OK) return erc;

	if(m_client.write_packet(buff)) {
		if(!m_client.read_packet(buff)) {
            m_connected = false;
			return FM_ERR_NOCONNECT;
		}

		if (ptr_to_hdr(buff)->msg_rc != FM_ERR_OK){
			*max_alarms_to_get = 0;
			EFmErrorT rc = (EFmErrorT)ptr_to_hdr(buff)->msg_rc;
			return rc;
		}
		
		uint32_t pkt_size = ptr_to_hdr(buff)->msg_size;
		if (pkt_size < sizeof(uint32_t)) {
			FM_ERROR_LOG("Received invalid pkt size: %u\n",pkt_size );
			m_connected = false;
			return FM_ERR_COMMUNICATIONS;
		}
		pkt_size-=sizeof(uint32_t);

		char *dptr = (char*)ptr_to_data(buff);

		uint32_t *len = (uint32_t*)dptr;
		dptr+=sizeof(uint32_t);
		if (*max_alarms_to_get < *len) {
			return FM_ERR_NOT_ENOUGH_SPACE;
		}
		if (pkt_size < (*len*sizeof(SFmAlarmDataT)) ) {
			return FM_ERR_COMMUNICATIONS;
		}
		*max_alarms_to_get = *len;
		memcpy(alarm,dptr,pkt_size);
	} else {
		m_connected = false;
		return FM_ERR_NOCONNECT;
	}

	return FM_ERR_OK;
}

EFmErrorT fm_get_faults_by_id(fm_alarm_id *alarm_id, SFmAlarmDataT *alarm,
						unsigned int *max_alarms_to_get){

	CFmMutexGuard m(getAPIMutex());
	if (!fm_lib_reconnect()) return FM_ERR_NOCONNECT;
	fm_check_thread_pending_request();

	fm_buff_t buff;
	buff.clear();
	EFmErrorT erc = fm_msg_utils_prep_requet_msg(buff,EFmGetFaultsById,
			        (*alarm_id),sizeof(*alarm_id));
	if (erc!=FM_ERR_OK) return erc;

	if(m_client.write_packet(buff)) {
		if(!m_client.read_packet(buff)) {
            m_connected = false;
			return FM_ERR_NOCONNECT;
		}

		if (ptr_to_hdr(buff)->msg_rc != FM_ERR_OK){
			*max_alarms_to_get = 0;
			EFmErrorT rc = (EFmErrorT)ptr_to_hdr(buff)->msg_rc;
			return rc;
		}

		uint32_t pkt_size = ptr_to_hdr(buff)->msg_size;
		if (pkt_size < sizeof(uint32_t)) {
			FM_ERROR_LOG("Received invalid pkt size: %u\n",pkt_size );
			m_connected = false;
			return FM_ERR_COMMUNICATIONS;
		}
		pkt_size-=sizeof(uint32_t);

		char *dptr = (char*)ptr_to_data(buff);

		uint32_t *len = (uint32_t*)dptr;
		dptr+=sizeof(uint32_t);
		if (*max_alarms_to_get < *len) {
			return FM_ERR_NOT_ENOUGH_SPACE;
		}
		if (pkt_size < (*len*sizeof(SFmAlarmDataT)) ) {
			return FM_ERR_COMMUNICATIONS;
		}
		*max_alarms_to_get = *len;
		memcpy(alarm,dptr,pkt_size);
	} else {
		m_connected = false;
		return FM_ERR_NOCONNECT;
	}

	return FM_ERR_OK;
}

/*
 * APIs that enqueue the request and return ok for acknowledgment.
 * A backgroup thread will pick up the request and send it to the FM Manager
 */
EFmErrorT fm_set_fault_async(const SFmAlarmDataT *alarm, fm_uuid_t *uuid){

	if ( !fm_lib_thread()) return FM_ERR_RESOURCE_UNAVAILABLE;
	CHECK_LIST_FULL(GetListOfFmRequests());

	fm_uuid_t id;
	fm_buff_t buff;
	buff.clear();
	fm_uuid_create(id);
	EFmErrorT erc = fm_msg_utils_prep_requet_msg(buff,EFmCreateFault,
			alarm,sizeof(*alarm));
	if (erc!=FM_ERR_OK) return erc;
	memcpy(ptr_to_data(buff), id, sizeof(fm_uuid_t)-1);

	FM_INFO_LOG("Enqueue raise alarm request: UUID (%s) alarm id (%s) instant id (%s)",
			id, alarm->alarm_id, alarm->entity_instance_id);
	enqueue(buff);

	if (uuid != NULL){
		memcpy(*uuid,id,sizeof(*uuid)-1);
	}
	return FM_ERR_OK;
}

EFmErrorT fm_clear_fault_async(AlarmFilter *filter){

	if ( !fm_lib_thread()) return FM_ERR_RESOURCE_UNAVAILABLE;
	CHECK_LIST_FULL(GetListOfFmRequests());

	fm_buff_t buff;
	buff.clear();
	EFmErrorT erc = fm_msg_utils_prep_requet_msg(buff,EFmDeleteFault,
			                                     filter,sizeof(*filter));
	if (erc!=FM_ERR_OK) return erc;

	FM_INFO_LOG("Enqueue clear alarm request: alarm id (%s), instant id (%s)",
			filter->alarm_id, filter->entity_instance_id);
	enqueue(buff);

	return FM_ERR_OK;
}

EFmErrorT fm_clear_all_async(fm_ent_inst_t *inst_id){

	if ( !fm_lib_thread()) return FM_ERR_RESOURCE_UNAVAILABLE;
	CHECK_LIST_FULL(GetListOfFmRequests());

	fm_buff_t buff;
	buff.clear();
	EFmErrorT erc = fm_msg_utils_prep_requet_msg(buff,EFmDeleteFaults,
			        (*inst_id), sizeof(*inst_id));
	if (erc!=FM_ERR_OK) return erc;

	FM_INFO_LOG("Enqueue clear all alarm request: instant id (%s)", *inst_id);
	enqueue(buff);
	return FM_ERR_OK;
}

}

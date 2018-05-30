//
// Copyright (c) 2017 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/select.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "fmSocket.h"
#include "fmThread.h"
#include "fmMutex.h"
#include "fmLog.h"
#include "fmDbUtils.h"

CFmMutex & getConnectionMutex(){
	static CFmMutex *m = new CFmMutex;
	return *m;
}

void CFmSockAddr::set_type(socklen_t addr_len) {
	if (addr_len==sizeof(address.ipv4)) {
		type = AF_INET;
	} else if (addr_len==sizeof(address.ipv6)) {
		type = AF_INET6;
	} else {
		type = AF_UNIX;
	}
}


CFmSocket::CFmSocket() {
	m_fd = -1;
}

CFmSocket::~CFmSocket() {
	close();
}


void CFmSocket::close() {
	if (m_fd!=-1) {
		//FM_INFO_LOG("close fd:(%d)", m_fd);
		::close(m_fd);
	}
	m_fd = -1;

}

bool CFmSocket::create_socket() {
    int optval = 1;
    socklen_t optlen = sizeof(optval);

    close();
    m_fd = ::socket(address_family,SOCK_STREAM,0);
    if (m_fd == -1){
        FM_ERROR_LOG("Failed to create socket, error: (%d) (%s)", errno, strerror(errno));
        return false;
    }

    /* Set the KEEPALIVE option active */
    if(setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0) {
        FM_ERROR_LOG("Failed to setsockopt, error: (%d) (%s)", errno, strerror(errno));
        close();
        return false;
    }
    //FM_DEBUG_LOG("SO_KEEPALIVE set on socket\n");

    return true;
}

bool CFmSocket::connect(const char *host, int port, int address_family) {
	this->address_family = address_family;
	if (!create_socket()) {
		return false;
	}
	switch (address_family) {
		//When address is IPv4
		case AF_INET:
		{
			struct sockaddr_in addr;

			if (inet_aton(host,&(addr.sin_addr))==0) {
				FM_INFO_LOG("inet_aton() failed\n");
				return false;
			}
			addr.sin_port = htons(port);
			addr.sin_family = AF_INET;

			fd_set wset;
			FD_ZERO(&wset);
			FD_SET(m_fd,&wset);

			return  (::connect(m_fd,(const struct sockaddr *)&addr,sizeof(addr))==0);
		}
		//When address is IPv6
		case AF_INET6:
		{
			struct sockaddr_in6 addr;

			if (inet_pton(AF_INET6,host,&(addr.sin6_addr))<=0) {
				FM_INFO_LOG("inet_aton() failed\n");
				return false;
			}
			addr.sin6_port = htons(port);
			addr.sin6_family = AF_INET6;

			fd_set wset;
			FD_ZERO(&wset);
			FD_SET(m_fd,&wset);

			return  (::connect(m_fd,(const struct sockaddr *)&addr,sizeof(addr))==0);
		}
		//Should never get here, needed for completeness
		default:
		{
			return false;
		}
	}

}

bool CFmSocket::write(int fd, const void *data, long len) {
	int offset = 0;
	while (offset!=len) {
		int rc = ::write(fd, ((char*)data)+offset,len-offset);
		if (rc==0 || (rc==-1 && errno!=EINTR)) {
			FM_ERROR_LOG("Socket Error: Failed to write to fd:(%d), len:(%d), rc:(%d), error:(%s)",
					fd, len, rc, strerror(errno));
			return false;
		}
		if (rc==-1 && errno==EINTR) continue;
		offset+=rc;
	}
	return true;
}

bool CFmSocket::write_packet(int fd, const void *data, long plen) {
	uint32_t len = htonl(plen);
	bool rc = write(fd,&len,sizeof(len));
	if (!rc) return false;
	return write(fd,data,plen);
}

bool CFmSocket::write_packet(int fd, const std::vector<char> &data) {
	return write_packet(fd,&(data[0]),data.size());
}

bool CFmSocket::read_packet(int fd, std::vector<char> &data) {
	int32_t len = 0;
	long tlen = sizeof(len);
	int i = 10;
	for ( ; i > 0 ; --i) {
		if (!read(fd,&len,tlen)) {
			return false;
		}
		break;
	}
	if (tlen!=sizeof(len))
	{
		FM_ERROR_LOG("Socket Error: Length does not match the data size: (%ld), (%ld)", tlen, sizeof(len));
		return false;
	}
	len=ntohl(len);
	data.resize(len);
	tlen = len;

	if (!read(fd,&(data[0]),tlen)) return false;
	return true;
}

bool CFmSocket::read(int fd,void *data, long &len) {
	int offset = 0;
	while (offset!=len) {
		int rc = ::read(fd, ((char*)data)+offset,len-offset);
		if (rc==0 || (rc==-1 && errno!=EINTR)) {
			// return code 0 means graceful close of TCP socket
			if (rc !=0 ){
				FM_ERROR_LOG("Failed to read from fd:(%d), rc:(%d), error:(%s), len:(%d)",
						fd, rc, strerror(errno), len);
			}
			len = offset;
			return false;
		}
		if (rc==-1 && errno==EINTR) continue;
		offset+=rc;
	}
	return true;
}

bool CFmSocket::read_packet(std::vector<char> &data)  {
	return read_packet(m_fd,data);
}
bool CFmSocket::read(void *data, long &len) {
	return read(m_fd,data,len);
}

int CFmSocket::select(int *rfd, int rlen, int *wfds, int wlen,int timeout, int timeoutusec, bool &timedout) {
	fd_set rset,wset;
	FD_ZERO(&rset);
	wset=rset;
	int max_fd = -1;
	int ix = 0;
	for ( ; ix < rlen ; ++ix ) {
		if (max_fd < rfd[ix]) max_fd = rfd[ix];
		FD_SET(rfd[ix],&rset);
	}

	ix = 0;
	for ( ; ix < wlen ; ++ix ) {
		if (max_fd < wfds[ix]) max_fd = wfds[ix];
		FD_SET(wfds[ix],&wset);
	}

	struct timeval to;
	to.tv_sec = timeout;
	to.tv_usec = timeoutusec;
	timedout=false;
	int rc = 0;
	while (true) {
		rc = ::select(max_fd+1,&rset,&wset,NULL,&to);
		if (rc==-1 && errno!=EINTR) {
			break;
		}
		if (rc==-1) continue;
		if (rc==0) timedout = true;
		break;
	}
	if (rc>0) {
		ix = 0;
		for ( ; ix < rlen ; ++ix ) {
			if (!FD_ISSET(rfd[ix],&rset)) {
				rfd[ix]=-1;
			}
		}

		ix = 0;
		for ( ; ix < wlen ; ++ix ) {
			if (!FD_ISSET(wfds[ix],&wset)) {
				wfds[ix] = -1;
			}
		}
	}
	return rc;
}

int CFmSocket::select_read(int fd,int timeout, bool &timedout){
	return select(&fd,1,NULL,0,timeout,0,timedout);
}

bool CFmSocket::recvfrom(void *data, long &len, CFmSockAddr &addr) {
	socklen_t addr_len = sizeof(addr.address);
	int l = ::recvfrom(m_fd,data,len,0,addr.get_sockaddr(),&addr_len);
	if (l==-1) { len = errno; return false; }
	len = l;
	addr.set_type(addr_len);
	return true;
}

bool CFmSocket::write(const void *data, long len) {
	return write(m_fd,data,len);
}

void FmSocketServer::rm_socket(int sock) {
	CFmMutexGuard m(getConnectionMutex());
	conn_map_t::iterator it = connections.find(sock);
	if (it!=connections.end()) {
		connections.erase(it);
	}
}

void FmSocketServer::handle_socket_data(int fd, std::vector<char> &data,
		CFmDBSession &sess) {
	FM_INFO_LOG("Received data from sock:%d len %lu\n",fd,data.size());
}

bool FmSocketServer::good_socket(int sock) {
	bool timedout=false;
	int rc = select(&sock,1,NULL,0,0,0,timedout);
	return (rc!=-1);
}

void FmSocketServer::find_bad_fd() {
	if (!good_socket(m_fd)) {
		server_reset();
	}
	std::vector<int> lsock;
	to_sock_array(lsock);

	int ix = 0;
	int mx =lsock.size();
	for ( ; ix < mx ; ++ix) {
		if (!good_socket(lsock[ix])) {
			FM_INFO_LOG("Found bad fd, close it:(%d)", lsock[ix]);
			::close(lsock[ix]);
			rm_socket(lsock[ix]);
		}
	}
}

bool FmSocketServer::run() {
	CFmDBSession *sess;
	if (fm_db_util_create_session(&sess) != true){
		FM_ERROR_LOG("Fail to create DB session, exit ...\n");
		exit (-1);
	}
	while (true) {
		std::vector<int> lsock;
		to_sock_array(lsock);
		lsock.push_back(m_fd);

		bool timedout =false;
		int rc = select(&(lsock[0]),lsock.size(),NULL,0,1,0,timedout);
		if (timedout) continue;
		if (rc==-1) {
			find_bad_fd();
			continue;
		}
		//listening socket and close all current sockets..
		int ix = 0;
		int mx = lsock.size();
		for ( ; ix < mx ; ++ix ) {
			if (lsock[ix]==-1) continue;
			if (lsock[ix]==m_fd) {
				accept();
				continue;
			}
			std::vector<char>buff;
			buff.clear();
			bool rc = read_packet(lsock[ix],buff);
			if (!rc) {
				::close(lsock[ix]);
				rm_socket(lsock[ix]);
			} else {
				//FM_INFO_LOG("handle_socket_data\n");
				handle_socket_data(lsock[ix],buff, *sess);
			}

		}
	}
	return false;
}

FmSocketServer::~FmSocketServer() {
	conn_map_t::iterator it = connections.begin();
	conn_map_t::iterator end = connections.end();
	for (; it != end ; ++it) {
		::close(it->first);
	}
	connections.clear();
}
void FmSocketServer::to_sock_array(std::vector<int> &socks) {
	CFmMutexGuard m(getConnectionMutex());
	conn_map_t::iterator it = connections.begin();
	conn_map_t::iterator end = connections.end();
	for (; it != end ; ++it) {
		socks.push_back(it->first);
	}
}

bool FmSocketServer::server_reset() {
	if (!create_socket()) {
		FM_INFO_LOG("Failed to create socket for port:(%d)\n", server_port);
		return false;
	}
	switch (address_family) {
		//When address is IPv4
		case AF_INET:
		{
			int optval = 1;
			setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = inet_addr(server_addr.c_str());
			addr.sin_port = htons(server_port);

			int ipAddr = addr.sin_addr.s_addr;
			char str[INET_ADDRSTRLEN];
			memset(str,0, sizeof(str));
			inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );

			if (bind(m_fd,(const struct sockaddr *)&addr,sizeof(addr))!=0) {
				return false;
			}
			FM_INFO_LOG("FM server socket binds the addr:(%s) port:(%d)\n",str, htons(server_port));

			if (::listen(m_fd,10)==-1) {
				FM_INFO_LOG("listen on fd:(%d) failed, errno: (%d) (%s)\n",
							 m_fd, errno, strerror(errno));
			}
			return true;
		}
		//When address is IPv6
		case AF_INET6:
		{
			int optval = 1;
			setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

			struct sockaddr_in6 addr;
			addr.sin6_family = AF_INET6;
			inet_pton(AF_INET6,server_addr.c_str(),&(addr.sin6_addr));
			addr.sin6_port = htons(server_port);

			char str[INET6_ADDRSTRLEN];
			memset(str,0, sizeof(str));
			inet_ntop( AF_INET6, &addr.sin6_addr.s6_addr, str, INET6_ADDRSTRLEN );

			if (bind(m_fd,(const struct sockaddr *)&addr,sizeof(addr))!=0) {
				return false;
			}
			FM_INFO_LOG("FM server socket binds the addr:(%s) port:(%d)\n",str, htons(server_port));

			if (::listen(m_fd,10)==-1) {
				FM_INFO_LOG("listen on fd:(%d) failed, errno: (%d) (%s)\n",
							 m_fd, errno, strerror(errno));
			}
			return true;
		}
		//Should never get here, needed for completeness
		default:
		{
			return false;
		}
	}

}

bool FmSocketServer::server_sock(const char *bindaddr, int port, int address_family) {
	this->address_family = address_family;
	server_addr = bindaddr;
	server_port = port;
	return server_reset();
}

bool  FmSocketServer::accept() {
	client_conn con;
	socklen_t alen =  sizeof(con.addr);

	int fd = ::accept(m_fd,con.addr.get_sockaddr(),&alen);
	if (fd==-1) {
		FM_INFO_LOG("accept returns fd: (%d) errno: (%d) (%s)\n",
				fd, errno, strerror(errno));
		return false;
	}
	con.sock = fd;
	CFmMutexGuard m(getConnectionMutex());
	connections[fd] = con;
	return true;
}


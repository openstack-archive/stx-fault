//
// Copyright (c) 2014 Wind River Systems, Inc.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifndef FMSOCKET_H_
#define FMSOCKET_H_

#include "fmMutex.h"
#include "fmDb.h"

#include <vector>
#include <unistd.h>
#include <string>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <map>

typedef struct CFmSockAddr_s {
	int type;
	union {
		struct sockaddr_in ipv4;
		struct sockaddr_in6 ipv6;
		struct sockaddr_un un;
	} address;
	sockaddr * get_sockaddr() {
		return (sockaddr*)(&address);
	}
	void set_type(socklen_t len);
} CFmSockAddr;

class CFmSocket {
protected:
	int m_fd;
	int address_family;

	void close();
	bool create_socket();
public:
	CFmSocket();
	~CFmSocket();

	bool connect(const char *host, int port, int address_family);


	static bool write(int fd, const void *data, long len);
	static bool write_packet(int fd, const std::vector<char> &data);
	static bool write_packet(int fd, const void *data, long len);

	static bool read(int fd,void *data, long &len);
	static bool read_packet(int fd, std::vector<char> &data);

	// read length and then size of exact packet
	bool read_packet(std::vector<char> &data);
	bool write_packet(const std::vector<char> &data) {
		return write_packet(m_fd,data);
	}
	bool write_packet(const void *data, long len) {
		return write_packet(m_fd,data,len);
	}

	bool read(void *data, long &len);
	bool write(const void *data, long len);


	int get_fd() { return m_fd; }
	bool recvfrom(void *data, long &len, CFmSockAddr &addr );

	static int select(int *rfd, int rlen, int *wfds, int wlen,int timeout,int timeoutusec, bool &timedout);
	static int select_read(int fd,int timeout, bool &timedout);
};


class FmSocketServer : public CFmSocket {
protected:
	std::string server_addr;
	int server_port;

	typedef struct {
		CFmSockAddr addr;
		int sock;
	} client_conn;

	typedef std::map<int,client_conn> conn_map_t;
	conn_map_t connections;

	void to_sock_array(std::vector<int> &socks);

	bool accept();
	virtual void handle_socket_data(int fd,std::vector<char> &data,
			CFmDBSession &sess);

	void add_socket(int sock);
	void rm_socket(int sock);
	bool good_socket(int sock);
	void find_bad_fd();
public:
	bool server_sock(const char *bindaddr, int port, int address_family);
	bool server_reset();

	bool run();

	virtual ~FmSocketServer();
};

#endif /* DMSOCKET_H_ */

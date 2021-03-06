//
// Copyright 2009-2017 M. Shulhan (ms@kilabit.info). All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "Socket.hh"

namespace vos {

const char* Socket::__CNAME = "Socket";

size_t Socket::DFLT_BUFFER_SIZE = 65536;

/**
 * @method		: Socket::Socket
 * @param		:
 *	> bfr_size	: size of socket buffer.
 * @desc		: Socket object constructor.
 */
Socket::Socket(const size_t bfr_size) : File(bfr_size)
,	_family(0)
,	_type(0)
,	_recv_addr_l (0)
,	_locker()
{
	_name.resize(INET6_ADDRSTRLEN);
}

/**
 * @method	: Socket::~Socket
 * @desc	: Socket object destructor.
 */
Socket::~Socket()
{
	if (_type == SOCK_DGRAM) {
		reset();
	}
}

/**
 * Method `accept(server_fd,family,type) will accept connection from
 * `server_fd`.
 *
 * On success it will return NULL, otherwise it will return Error object.
 */
Error Socket::accept(int server_fd)
{
	socklen_t client_addrlen;
	struct sockaddr_in client_addr;
	struct sockaddr_in6 client_addr6;

	client_addrlen	= SockAddr::IN_SIZE;

	if (_family == AF_INET) {
		_d = ::accept(server_fd
			, (struct sockaddr*) &client_addr
			, &client_addrlen);
	} else {
		_d = ::accept(server_fd
			, (struct sockaddr*) &client_addr6
			, &client_addrlen);

	}
	if (_d < 0) {
		return Error::SYS();
	}

	if (_family == AF_INET) {
		inet_ntop(_family, &client_addr.sin_addr, (char*) _name.v()
		, socklen_t(_name.size()));
	} else {
		inet_ntop(_family, &client_addr6.sin6_addr, (char*) _name.v()
		, socklen_t(_name.size()));
	}

	_status = FILE_OPEN_SOCK;

	return NULL;
}

/**
 * @method		: Socket::create
 * @param		:
 *	> family	: address family (AF_LOCAL, AF_UNIX, AF_FILE, AF_INET,
 *				AF_INET6).
 *	> type		: socket type (SOCK_STREAM, SOCK_DGRAM, SOCK_RAW).
 * @return		:
 *	< 0		: success.
 *	< -1		: fail.
 * @desc		: create a new socket descriptor.
 */
int Socket::create(const int family, const int type)
{
	_d = ::socket(family, type, 0);
	if (_d < 0) {
		return -1;
	}

	_family	= family;
	_type	= type;

	if (type == SOCK_STREAM) {
		_status = FILE_OPEN_NO;
	} else {
		_status = FILE_OPEN_SOCK;
	}

	return 0;
}

int Socket::set_socket_opt (int optname, int optval)
{
	int s = 0;
	size_t len = sizeof(optval);

	s = setsockopt(_d, SOL_SOCKET, optname, &optval, socklen_t(len));
	if (s < 0) {
		return -1;
	}
	return 0;
}

int Socket::set_keep_alive (int val)
{
	return set_socket_opt (SO_KEEPALIVE, val);
}

int Socket::set_reuse_address (int val)
{
	return set_socket_opt (SO_REUSEADDR, val);
}

/**
 * @method	: Socket::connect_to
 * @param	:
 *	> sin	: socket address internet.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	: connect to end point as defined in sockaddr_in object 'sin'.
 */
int Socket::connect_to(struct sockaddr_in* sin)
{
	if (!sin) {
		return -1;
	}

	int s = ::connect(_d, (struct sockaddr*) sin, SockAddr::IN_SIZE);
	if (s < 0) {
		return -1;
	}

	inet_ntop(AF_INET, &sin->sin_addr, (char*) _name.v()
		, socklen_t(_name.size()));
	_status = FILE_OPEN_SOCK;

	return 0;
}

/**
 * @method	: Socket::connect_to6
 * @param	:
 *	> sin	: socket address internet.
 * @return	:
 *	< 0	: success.
 *	< -1	: fail.
 * @desc	: connect to end point address, through IPv6, as defined in
 * sockaddr_in6 object 'sin6'.
 */
int Socket::connect_to6(struct sockaddr_in6* sin6)
{
	if (!sin6) {
		return -1;
	}

	int s = ::connect(_d, (struct sockaddr*) sin6, SockAddr::IN_SIZE);
	if (s < 0) {
		return -1;
	}

	inet_ntop(AF_INET6, &sin6->sin6_addr, (char*) _name.v()
		, socklen_t(_name.size()));
	_status = FILE_OPEN_SOCK;

	return 0;
}

/**
 * @method		: Socket::connect_to_raw
 * @param		:
 *	> address	: destination hostname or IP address to connect to.
 *	> port		: destination port number.
 * @return		:
 *	< 0		: success.
 *	< -1		: fail.
 * @desc		: connect socket to 'address' with 'port'.
 */
int Socket::connect_to_raw(const char* address, const uint16_t port)
{
	if (!address) {
		return -1;
	}

	int s;

	if (_family == AF_INET6) {
		struct sockaddr_in6 sin6;

		s = SockAddr::CREATE_ADDR6(&sin6, address, port);
		if (s < 0) {
			return -1;
		}

		s = ::connect(_d, (struct sockaddr*) &sin6
				, SockAddr::IN6_SIZE);
	} else {
		struct sockaddr_in sin;

		s = SockAddr::CREATE_ADDR(&sin, address, port);
		if (s < 0) {
			return -1;
		}

		s = ::connect(_d, (struct sockaddr*) &sin, SockAddr::IN_SIZE);
	}
	if (s < 0) {
		return -1;
	}

	_status = FILE_OPEN_SOCK;

	Error err = _name.copy_raw(address);
	if (err != NULL) {
		return -1;
	}

	return s;
}

/**
 * @method	: Socket::send_udp
 * @param	:
 *	> addr	: address of end point.
 *	> bfr	: buffer object to be send.
 * @return	:
 *	< >=0	: success, number of bytes send.
 *	< -1	: fail.
 * @desc	: send 'bfr' to 'addr' using datagram protocol.
 */
long int Socket::send_udp(struct sockaddr_in* addr, Buffer* bfr)
{
	if (!addr) {
		return -1;
	}

	ssize_t s = 0;

	_locker.lock();

	if (!bfr) {
		if (_i > 0) {
			s = ::sendto(_d, _v, _i, 0
				, (struct sockaddr*) addr, SockAddr::IN_SIZE);
		}
	} else if (bfr->len() > 0) {
			s = ::sendto(_d, bfr->v(), bfr->len(), 0
				, (struct sockaddr*) addr, SockAddr::IN_SIZE);
	}

	_locker.unlock();

	return s;
}

/**
 * @method	: Socket::send_udp_raw
 * @param	:
 *	> addr	: address of end point.
 *	> bfr	: buffer object to be send.
 * @return	:
 *	< >=0	: success, number of bytes send.
 *	< -1	: fail.
 * @desc	:
 *	send 'bfr' with length is 'len' to 'addr' using datagram protocol.
 */
long int Socket::send_udp_raw(struct sockaddr_in* addr, const char* bfr
				, size_t len)
{
	if (!addr) {
		return -1;
	}

	_locker.lock();

	if (!bfr) {
		bfr = _v;
		len = _i;
	}
	if (!len) {
		len = strlen(bfr);
	}

	ssize_t s = ::sendto(_d, bfr, len, 0, (struct sockaddr*) addr
			, SockAddr::IN_SIZE);

	_locker.unlock();

	return s;
}

/**
 * @method	: Socket::recv_udp
 * @param	:
 *	> addr	: output, originating address of end point.
 * @return	:
 *	< >=0	: success, size of received data.
 *	< -1	: fail.
 * @desc	:
 *	received data from end point using datagram protocol and save the end
 *	point address to 'addr'.
 */
long int Socket::recv_udp(struct sockaddr_in* addr)
{
	if (!addr) {
		return 0;
	}

	_locker.lock();

	ssize_t s = 0;
	_recv_addr_l = SockAddr::IN_SIZE;

	s = ::recvfrom(_d, _v, _l, 0, (struct sockaddr*) addr, &_recv_addr_l);
	if (s >= 0) {
		_v[s] = '\0';
		_i = size_t(s);
	}

	_locker.unlock();

	return s;
}

} // namespace::vos
// vi: ts=8 sw=8 tw=80:

//
// Copyright 2009-2017 M. Shulhan (ms@kilabit.info). All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef _LIBVOS_RESOLVER_DNS_RR_HH
#define _LIBVOS_RESOLVER_DNS_RR_HH 1

#include <arpa/inet.h>
#include "Buffer.hh"
#include "DNSRecordType.hh"

namespace vos {

enum _RR_TYPE {
	QUERY_T_ADDRESS		= 1,
	QUERY_T_NAMESERVER,
	QUERY_T_MAIL_DEST,
	QUERY_T_MAIL_FORWARDER,
	QUERY_T_CNAME,
	QUERY_T_SOA,
	QUERY_T_MAIL_BOX,
	QUERY_T_MAIL_GROUP,
	QUERY_T_MAIL_RENAME,
	QUERY_T_NULL,
	QUERY_T_WKS,
	QUERY_T_PTR,
	QUERY_T_HINFO,
	QUERY_T_MINFO,
	QUERY_T_MX,
	QUERY_T_TXT		= 16,
	QUERY_T_AAAA		= 28,
	QUERY_T_SRV		= 33,
	QUERY_T_AXFR		= 252,
	QUERY_T_MAILB,
	QUERY_T_MAILA,
	QUERY_T_ALL
};

enum _RR_CLASS {
	QUERY_C_IN	= 1,
	QUERY_C_CS,
	QUERY_C_CH,
	QUERY_C_HS,
	QUERY_C_ALL	= 255
};

/**
 * @class		: DNS_rr
 * @attr		:
 *	- _type		: type of Resource Record.
 *	- _class	: class of Resource Record.
 *	- _ttl		: time-to-live of Resource Record.
 *	- _len		: length of Resource Record.
 *	- _name_len	: length of '_name'.
 *	- _name		: domain name.
 *	- _data		: extract value for A record, NS record, SOA MNAME
 *			field, PTR record, and HINFO CPU field.
 *	- _data2	: extract value for SOA RNAME field and HINFO OS
 *			field.
 *	- _serial	: SOA serial value.
 *	- _refresh	: SOA refresh value.
 *	- _retry	: SOA retry value.
 *	- _expire	: SOA expire value.
 *	- _minimum	: SOA minimum value.
 *	- _priority	: priority of RR MX.
 *	- _weight	: SRV, a relative weight for records with the same
 *			priority.
 *	- _port		: SRV, the TCP or UDP port on which the service is to
 *			be found.
 *	- RDATA_MAX_SIZE: static, maximum size for RR buffer.
 * @desc		:
 *	submodule of DNSQuery, for listing Resource Record of DNS packet.
 */
class DNS_rr : public Buffer {
public:
	explicit DNS_rr(const size_t bfr_size = RDATA_MAX_SIZE);
	~DNS_rr();
	const char* chars();

	int create_packet ();

	void reset();
	static DNS_rr* INIT (const char* name
				, uint16_t type, uint16_t clas
				, uint32_t ttl
				, uint16_t data_len, const char* data);

	uint16_t	_type;
	uint16_t	_class;
	uint32_t	_ttl;
	const char*	_ttl_p;
	uint16_t	_len;
	uint16_t	_name_len;
	Buffer		_name;
	/* A record, NS record, SOA MNAME, PTR record, HINFO CPU field */
	Buffer		_data;
	/* SOA RNAME, HINFO OS field */
	Buffer		_data2;
	/* SOA record */
	uint32_t	_serial;
	uint32_t	_refresh;
	uint32_t	_retry;
	uint32_t	_expire;
	uint32_t	_minimum;
	/* MX, SRV priority  field */
	uint16_t	_priority;
	/* SRV */
	uint16_t	_weight;
	uint16_t	_port;

	static int APPEND_DNS_LABEL (Buffer* b, const char* label
					, size_t len = 0);
	static size_t RDATA_MAX_SIZE;

	static const char* __cname;
private:
	DNS_rr(const DNS_rr&);
	void operator=(const DNS_rr&);
};

} /* namespace::vos */

#endif
// vi: ts=8 sw=8 tw=78:

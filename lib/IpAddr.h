/**
 * IPv4 and IPv6 address structure.
 */

#ifndef _LIB_IP_ADDR_H_
#define _LIB_IP_ADDR_H_

#include <config.h>

#include <arpa/inet.h>
#include <assert.h>
#include <string.h>
#include <string>

class IpAddr {
public:
	IpAddr();
	~IpAddr() {};

	bool isIp4Addr();
	bool isEmpty();
	void setEmpty();
	uint32_t getIp4Addr();
	uint64_t getIp6Addr(bool second);
	void setIp4Addr(uint32_t addr);
	void setIp6Addr(uint64_t addr, bool second);
	std::string toString();
	bool ParseIp4Addr(std::string &s);
	bool ParseIp6Addr(std::string &s);
	void ApplyPrefix(int prefix);

	static IpAddr ipAddrEmpty;

private:
	bool ip4Addr;
	union {
		in_addr ip4;
		in6_addr ip6;
	} addr;
};

inline IpAddr::IpAddr() {
	ip4Addr = true;
	memset(&addr, sizeof(addr), 0);
}

inline bool IpAddr::isIp4Addr() {
	return ip4Addr;
}

inline bool IpAddr::isEmpty() {
	if (ip4Addr)
		return addr.ip4.s_addr == 0;
	for (int i = 0; i < 16; i++) {
		if (addr.ip6.s6_addr[i] != 0)
			return false;
	}
	return true;
}

inline void IpAddr::setEmpty() {
	if (ip4Addr)
		memset((void*)addr.ip4.s_addr, sizeof(in_addr), 0);
	else
		memset((void*)addr.ip6.s6_addr, sizeof(in6_addr), 0);
}

// returns address in network-order
inline uint32_t IpAddr::getIp4Addr() {
	assert(ip4Addr == true);
	return (uint32_t)addr.ip4.s_addr;
}

// returns part of IPv6 address in network-order
inline uint64_t IpAddr::getIp6Addr(bool second) {
	assert(ip4Addr == false);
	return *(((uint64_t*)&addr.ip6.s6_addr) + (second ? 1 : 0));
}

// addr is in network-order
inline void IpAddr::setIp4Addr(uint32_t addr) {
	ip4Addr = true;
	this->addr.ip4.s_addr = addr;
}

// addr is in network-order
inline void IpAddr::setIp6Addr(uint64_t addr, bool second) {
	ip4Addr = false;
	*(((uint64_t*)&this->addr.ip6.s6_addr) + (second ? 1 : 0)) = addr;
}

inline std::string IpAddr::toString() {
	char s[INET6_ADDRSTRLEN];
	if (ip4Addr) {
		if (!inet_ntop(AF_INET, (const void*)addr.ip4.s_addr, s, INET6_ADDRSTRLEN))
			return NULL;
	} else {
		if (!inet_ntop(AF_INET6, (const void*)addr.ip6.s6_addr, s, INET6_ADDRSTRLEN))
			return NULL;
	}
	return s;
}

inline bool IpAddr::ParseIp4Addr(std::string &s) {
	ip4Addr = true;
	if (!inet_pton(AF_INET, s.c_str(), &addr.ip4.s_addr)) {
		setEmpty();
		return false;
	}
	return true;
}

inline bool IpAddr::ParseIp6Addr(std::string &s) {
	ip4Addr = false;
	if (!inet_pton(AF_INET6, s.c_str(), &addr.ip6.s6_addr)) {
		setEmpty();
		return false;
	}
	return true;
}

inline void ReverseIp6Addr(in6_addr *addr) {
	if (htons(0xabcd) != 0xabcd) {
		for (int i = 0; i < 8; i++) {
			uint8_t tmp = addr->s6_addr[15-i];
			addr->s6_addr[15-i] = addr->s6_addr[i];
			addr->s6_addr[i] = tmp;
		}
	}
}

inline void IpAddr::ApplyPrefix(int prefix) {
	if (ip4Addr) {
		addr.ip4.s_addr = htonl(ntohl(addr.ip4.s_addr) & (uint32_t)0xffffffff << (32-prefix));
	} else {
		ReverseIp6Addr(&addr.ip6);
		if (!prefix)
			prefix = 128;
		int a = prefix / 8;
		int b = prefix % 8;
		if (a < 16)
			addr.ip6.s6_addr[a] &= 0xFF & (0xFF << (8-b));
		for (int i = a+1; i < 16; i++)
			addr.ip6.s6_addr[i] = 0x00;
		ReverseIp6Addr(&addr.ip6);
	}
}

#endif

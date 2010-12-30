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
	IpAddr(bool ip4Addr, const std::string &s);
	~IpAddr() {};

	bool isIp4Addr();
	bool isEmpty();
	void setEmpty();
	uint32_t getIp4Addr();
	uint64_t getIp6Addr(bool first);
	void setIp4Addr(uint32_t addr);
	void setIp6Addr(uint64_t addr, bool first);
	bool ParseIp4Addr(const std::string &s);
	bool ParseIp6Addr(const std::string &s);
	void ReverseIp6Addr(in6_addr *addr);
	void ApplyPrefix(int prefix);
	bool Match(const std::string &addr, int prefix);
	bool Match(const std::string &addr);
	std::string toString();

	static IpAddr emptyIpAddr;

private:
	bool ip4Addr;
	union {
		in_addr ip4;
		in6_addr ip6;
	} addr;

	bool Compare(IpAddr *other, int prefix);
};

inline IpAddr::IpAddr() {
	ip4Addr = true;
	memset(&addr, 0, sizeof(addr));
}

inline IpAddr::IpAddr(bool ip4Addr, const std::string &s) {
	ip4Addr ? this->ParseIp4Addr(s) : this->ParseIp6Addr(s);
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
		memset((void*)&addr.ip4.s_addr, 0, sizeof(in_addr));
	else
		memset((void*)&addr.ip6.s6_addr, 0, sizeof(in6_addr));
}

// returns address in network-order
inline uint32_t IpAddr::getIp4Addr() {
	assert(ip4Addr == true);
	return (uint32_t)addr.ip4.s_addr;
}

// returns part of IPv6 address in network-order
inline uint64_t IpAddr::getIp6Addr(bool first) {
	assert(ip4Addr == false);
	return *(((uint64_t*)&addr.ip6.s6_addr) + (first ? 0 : 1));
}

// addr is in network-order
inline void IpAddr::setIp4Addr(uint32_t addr) {
	ip4Addr = true;
	this->addr.ip4.s_addr = addr;
}

// addr is in network-order
inline void IpAddr::setIp6Addr(uint64_t addr, bool first) {
	ip4Addr = false;
	*(((uint64_t*)&this->addr.ip6.s6_addr) + (first ? 0 : 1)) = addr;
}

#endif

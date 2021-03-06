#include <config.h>

#include "common.h"
#include "IpAddr.h"
using namespace std;

IpAddr IpAddr::emptyIpAddr;

bool IpAddr::ParseIp4Addr(const string &s) {
	ip4Addr = true;
	if (!inet_pton(AF_INET, s.c_str(), &addr.ip4.s_addr)) {
		SetEmpty();
		return false;
	}
	return true;
}

bool IpAddr::ParseIp6Addr(const string &s) {
	ip4Addr = false;
	if (!inet_pton(AF_INET6, s.c_str(), &addr.ip6.s6_addr)) {
		SetEmpty();
		return false;
	}
	return true;
}

void IpAddr::ReverseIp6Addr(in6_addr *addr) {
	if (htons(0xabcd) != 0xabcd) {
		for (int i = 0; i < 8; i++) {
			uint8_t tmp = addr->s6_addr[15-i];
			addr->s6_addr[15-i] = addr->s6_addr[i];
			addr->s6_addr[i] = tmp;
		}
	}
}

void IpAddr::ApplyPrefix(int prefix) {
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

bool IpAddr::Compare(IpAddr *other, int prefix) {
	if (ip4Addr) {
		return (ntohl(addr.ip4.s_addr) & (uint32_t)0xffffffff << (32-prefix)) == 
			(ntohl(other->GetIp4Addr()) & (uint32_t)0xffffffff << (32-prefix));
	} else {
		in6_addr ip6 = this->addr.ip6;
		ReverseIp6Addr(&ip6);
		in6_addr oip6;
		*(((uint64_t*)&oip6.s6_addr)) = other->GetIp6Addr(true);
		*(((uint64_t*)&oip6.s6_addr) + 1) = other->GetIp6Addr(false);
		ReverseIp6Addr(&oip6);
		return (*(((uint64_t*)&ip6.s6_addr)) == *(((uint64_t*)&oip6.s6_addr))
			 && *(((uint64_t*)&ip6.s6_addr) + 1) == *(((uint64_t*)&oip6.s6_addr) + 1));
	}
}

bool IpAddr::Match(const std::string &addr, int prefix) {
	IpAddr other = IpAddr(ip4Addr, addr);
	return this->Compare(&other, prefix);
}

bool IpAddr::Match(const std::string &addr) {
	size_t slash = addr.find('/');
	if (slash == string::npos || slash < 1) {
		return this->Match(addr, ip4Addr ? 32 : 128);
	} else {
		string a = addr.substr(0, slash);
		int prefix = atoi(addr.c_str()+slash+1);
		return this->Match(a, prefix);
	}
}


string IpAddr::ToString() {
	char s[INET6_ADDRSTRLEN];
	if (ip4Addr) {
		if (!inet_ntop(AF_INET, (const void*)&addr.ip4.s_addr, s, INET_ADDRSTRLEN))
			return NULL;
	} else {
		if (!inet_ntop(AF_INET6, (const void*)&addr.ip6.s6_addr, s, INET6_ADDRSTRLEN))
			return NULL;
	}
	return s;
}

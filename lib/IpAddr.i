%include "std_string.i"

%{
#include "IpAddr.h"
%}

class IpAddr {
public:
        IpAddr();
        IpAddr(bool ip4Addr, const std::string &s);
        ~IpAddr();

        bool IsIp4Addr();
        bool IsEmpty();
        void SetEmpty();
        uint32_t GetIp4Addr();
        uint64_t GetIp6Addr(bool second);
        void SetIp4Addr(uint32_t addr);
        void SetIp6Addr(uint64_t addr, bool second);
        std::string ToString();
        bool ParseIp4Addr(const std::string &s);
        bool ParseIp6Addr(const std::string &s);
        void ApplyPrefix(int prefix);
        bool Match(const std::string &addr, int prefix);
        bool Match(const std::string &addr);

        static IpAddr emptyIpAddr;
};

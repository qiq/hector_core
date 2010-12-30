%include "std_string.i"

%{
#include "IpAddr.h"
%}

class IpAddr {
public:
        IpAddr();
        IpAddr(bool ip4Addr, const std::string &s);
        ~IpAddr();

        bool isIp4Addr();
        bool isEmpty();
        void setEmpty();
        uint32_t getIp4Addr();
        uint64_t getIp6Addr(bool second);
        void setIp4Addr(uint32_t addr);
        void setIp6Addr(uint64_t addr, bool second);
        std::string toString();
        bool ParseIp4Addr(const std::string &s);
        bool ParseIp6Addr(const std::string &s);
        void ApplyPrefix(int prefix);
        bool Match(const std::string &addr, int prefix);
        bool Match(const std::string &addr);

        static IpAddr emptyIpAddr;
};

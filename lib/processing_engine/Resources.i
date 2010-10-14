%newobject ip4Addr2Str(ip4_addr_t *addr);
%newobject ip6Addr2Str(ip6_addr_t *addr);

%{
#include "Resources.h"
%}

class Resources {
        Resources();
        ~Resources();
        static Resource *CreateResource(int id);
        static int Name2Id(const char *name);
};

void ResourceQueuePush(std::queue<Resource*> *queue, Resource *resource);
Resource *ResourceQueuePop(std::queue<Resource*> *queue);

/* from lib/common.h */
int ResourceQueueSize(std::queue<Resource*> *queue);

%inline %{
ip4_addr_t *str2Ip4Addr_w(const char *s) {
        ip4_addr_t *addr = new ip4_addr_t;
        *addr = str2Ip4Addr(s);
        return addr;
}

char *ip4Addr2Str_w(ip4_addr_t *addr) {
        return ip4Addr2Str(*addr);
}

void ip4AddrDelete_w(ip4_addr_t *addr) {
        delete addr;
}

ip6_addr_t *str2Ip6Addr_w(const char *s) {
        ip6_addr_t *addr = new ip6_addr_t;
        *addr = str2Ip6Addr(s);
        return addr;
}

char *ip6Addr2Str_w(ip6_addr_t *addr) {
        return ip6Addr2Str(*addr);
}

void ip6AddrDelete_w(ip6_addr_t *addr) {
        delete addr;
}

%}

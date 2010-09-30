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
ip4_addr_t str2Ip4Addr(const char *s);
char *ip4Addr2Str(ip4_addr_t addr);
ip6_addr_t str2Ip6Addr(const char *s);
char *ip6Addr2Str(ip6_addr_t addr);

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
int ResourceQueueSize(std::queue<Resource*> *queue);

%{
#include "Resources.h"
%}

class Resources {
        Resources();
        ~Resources();
        static Resource *CreateResource(int id);
        static int Name2Id(const char *name);
};

void ResourceQueuePush(queue<Resource*> *queue, Resource *resource);
Resource *ResourceQueuePop(queue<Resource*> *queue);
int ResourceQueueSize(queue<Resource*> *queue);

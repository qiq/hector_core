%{
#include "Resources.h"
%}

Resource *ConstructResource(resource_t type, string *serial = NULL);
TestResource *Resource2TestResource(Resource *resource);
WebResource *Resource2WebResource(Resource *resource);

void ResourceQueuePush(queue<Resource*> *queue, Resource *resource);
Resource *ResourceQueuePop(queue<Resource*> *queue);
int ResourceQueueSize(queue<Resource*> *queue);

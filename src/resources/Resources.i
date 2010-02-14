%{
#include "Resources.h"
%}

Resource *ConstructResource(resource_t type, string *serial = NULL);
TestResource *Resource2TestResource(Resource *resource);
WebResource *Resource2WebResource(Resource *resource);

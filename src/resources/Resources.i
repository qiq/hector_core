%{
#include "Resources.h"
%}

Resource *ConstructResource(resource_t type, string *serial = NULL);
WebResource *Resource2WebResource(Resource *resource);

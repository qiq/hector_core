%include "std_string.i"

%{
#include "ResourceRegistry.h"
%}

class ResourceRegistry {
public:
        ResourceRegistry();
        ~ResourceRegistry();

        int NameToId(const char *name);
        Resource *AcquireResource(int id);
        void ReleaseResource(Resource *resource);
};

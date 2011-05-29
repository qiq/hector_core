%include "std_string.i"

%{
#include "ResourceRegistry.h"
%}

class ResourceRegistry {
public:
        ResourceRegistry();
        ~ResourceRegistry();

        int NameToId(const char *name);
        Resource *AcquireResource(int typeId);
        Resource *AcquireResource(const char *typeString);
        void ReleaseResource(Resource *resource);
};

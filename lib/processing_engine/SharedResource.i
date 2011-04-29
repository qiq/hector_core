%include "std_string.i"

%{
#include "SharedResource.h"
%}

class SharedResource : public Resource {
public:
        SharedResource() {};
        virtual ~SharedResource() {};
        virtual void Clear();
        virtual Resource *Clone() = 0;
        virtual int GetId();
        virtual void SetId(int id);
        virtual int GetStatus();
        virtual void SetStatus(int status);
        virtual void SetFlag(Resource::Flag flag);
        virtual void ResetFlag(Resource::Flag flag);
        virtual bool IsSetFlag(Resource::Flag flag);
        virtual Resource *GetAttachedResource();
        virtual void SetAttachedResource(Resource *attachedResource);
        virtual void ClearAttachedResource();
};

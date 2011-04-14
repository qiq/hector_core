%include "std_string.i"

%newobject MarkerResource::Serialize();

%{
#include "MarkerResource.h"
%}

class MarkerResourceInfo : public ResourceInfo {
public:
        MarkerResourceInfo();
};

class MarkerResource : public Resource {
public:
        MarkerResource();
        ~MarkerResource();
        Resource *Clone();
        void Clear();
        bool Serialize(ResourceOutputStream &output);
        bool Deserialize(ResourceInputStream &input);
        int GetSize();
        ResourceInfo *GetResourceInfo();
        std::string ToString(Object::LogLevel logLevel);

        void SetMark(int mark);
        int GetMark();

        static bool IsInstance(Resource *resource);
};

%include "std_string.i"

%newobject MarkerResource::Serialize();

%{
#include "MarkerResource.h"
%}

class MarkerResource : public Resource {
public:
        MarkerResource();
        ~MarkerResource();
        Resource *Clone();
        void Clear();
        bool Serialize(ResourceOutputStream &output);
        bool Deserialize(ResourceInputStream &input);
        std::vector<ResourceAttrInfo*> *GetAttrInfoList();
        int GetTypeId();
        const char *GetTypeString(bool terse = false);
        const char *GetObjectName();
        int GetSize();
        std::string ToString(Object::LogLevel logLevel);

        void SetMark(int mark);
        int GetMark();

        static bool IsInstance(Resource *resource);
};

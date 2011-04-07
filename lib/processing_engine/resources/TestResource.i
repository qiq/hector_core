%include "std_string.i"

%newobject TestResource::Serialize();

%{
#include "TestResource.h"
%}

class TestResource : public Resource {
public:
        TestResource();
        ~TestResource();
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

        void SetStr(const std::string &str);
        const std::string GetStr();

        static bool IsInstance(Resource *resource);
};

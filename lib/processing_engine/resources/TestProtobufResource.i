%include "std_string.i"

%newobject TestProtobufResource::Serialize();

%{
#include "TestProtobufResource.h"
%}

class TestProtobufResource : public Resource {
public:
        TestProtobufResource();
        ~TestProtobufResource();
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

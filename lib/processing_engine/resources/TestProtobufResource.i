%include "std_string.i"

%newobject TestProtobufResource::Serialize();

%{
#include "TestProtobufResource.h"
%}

class TestProtobufResourceInfo : public ResourceInfo {
public:
        TestProtobufResourceInfo();
};

class TestProtobufResource : public Resource {
public:
        TestProtobufResource();
        ~TestProtobufResource();
        Resource *Clone();
        void Clear();
        bool Serialize(ResourceOutputStream &output);
        bool Deserialize(ResourceInputStream &input);
        int GetSize();
        ResourceInfo *GetResourceInfo();
        std::string ToString(Object::LogLevel logLevel);

        void SetStr(const std::string &str);
        const std::string GetStr();

        static bool IsInstance(Resource *resource);
};

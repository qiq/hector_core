%include "std_string.i"

%newobject TestResource::Serialize();

%{
#include "TestResource.h"
%}

class TestResourceInfo : public ResourceInfo {
public:
        TestResourceInfo();
};

class TestResource : public Resource {
public:
        TestResource();
        ~TestResource();
        Resource *Clone();
        void Clear();
        bool Serialize(ResourceOutputStream &output);
        bool Deserialize(ResourceInputStream &input, bool headerOnly);
        bool Skip(ResourceInputStream &input);
        int GetSize();
        ResourceInfo *GetResourceInfo();
        std::string ToString(Object::LogLevel logLevel);

        void SetStr(const std::string &str);
        const std::string GetStr();

        static bool IsInstance(Resource *resource);
};

%inline %{
TestResource *ResourceToTestResource(Resource *r) {
        return r && TestResource::IsInstance(r) ? static_cast<TestResource*>(r) : NULL;
}
%}

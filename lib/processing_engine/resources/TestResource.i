%include "std_string.i"

%newobject TestResource::Serialize();
%newobject TestResource::toString();

%{
#include "TestResource.h"
%}

class TestResource : public Resource {
public:
        TestResource();
        ~TestResource();
        Resource *Clone();
        int getTypeId();
        const char *getTypeStr();
        const char *getModuleStr();
        int getId();
        void setId(int id);
        int getStatus();
        void setStatus(int status);

        std::string *Serialize();
        bool Deserialize(std::string *s);
        int getSize();
        std::string *toString(Object::LogLevel logLevel);

        void setStr(const std::string &str);
        const std::string &getStr();
};

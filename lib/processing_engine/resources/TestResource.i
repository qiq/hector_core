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
        std::string *Serialize();
        bool Deserialize(const char *data, int size);
        int getTypeId();
        const char *getTypeStr();
        const char *getModuleStr();
        int getId();
        void setId(int id);
        int getStatus();
        void setStatus(int status);
        int getSize();
        std::string toString(Object::LogLevel logLevel);

        void setStr(const std::string &str);
        const std::string &getStr();
};
